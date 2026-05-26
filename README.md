# 42_M03: codexion

## Table of Contents
- [Description](#description)
	- [The coders situation](#the-coders-situation)
	- [Parameters](#parameters)
- [Instructions](#instructions)
	- [Compilation](#compilation)
	- [Execution](#execution)
- [Resources](#resources)
- [Implementation](#implementation)
	- [Blocking cases handled](#blocking-cases-handled)
	- [Thread synchronization mechanisms](#thread-synchronization-mechanisms)

___

## Description

The goal of this project was to write a program called `codexion`, which simulates situation described below. It is basicly a reskin of `Dining Philosophers`.

### The coders situation
- One or more `coders` sit in a circular inclusive co-working hub.
In the center, there is a shared Quantum Compiler.
- The `coders` alternatively `compile`, `debug`, or `refactor`.
While compiling, they are not debugging nor refactoring;
while debugging, they are not compiling nor refactoring;
and, of course, while refactoring, they are not compiling nor debugging.
- There are USB `dongles` on the table. There are as many `dongles` as `coders`.
- Compiling quantum code requires two dongles plugged in simultaneously,
one in each hand: a `coder` takes their left and right `dongles` to `compile`.
- When a coder finishes compiling, they put both dongles back on the table and start debugging. Once debugging is done, they start refactoring.
- The simulation stops when a coder `burns out` due to lack of compiling or if all coders compiled at least that many times as the program argument `<number_of_compiles_required>` is set to.
- Every coder needs to compile regularly and should never burn out.
- Coders do not communicate with each other and do not know if another coder is about to burn out.

### Parameters
- `<number_of_coders>` The number of coders and also the number of dongles.
- `<time_to_burnout>` If a coder did not start compiling within `<time_to_burnout>` milliseconds since the beginning of their last compile or the beginning of the simulation, they burn out.
- `<time_to_compile>` The time it takes for a coder to compile. During that time, they must hold two dongles.
- `<time_to_debug>` The time a coder will spend debugging.
- `<time_to_refactor>` The time a coder will spend refactoring. After completing the refactoring phase, the coder will immediately attempt to
acquire dongles and start compiling again.
- `<number_of_compiles_required>` If all coders have compiled at least this
many times, the simulation stops. Otherwise, it stops when a coder burns
out.
- `<dongle_cooldown>` After being released, a dongle is unavailable until its cooldown has passed.
- `<scheduler>` The arbitration policy used by dongles to decide who gets them when multiple coders request them. The value must be exactly one of: `fifo` (First In, First Out) or `edf` (Earliest Deadline First).


## Instructions

### Compilation
```c
make / make all		# compile the codexion program
make clean			# delete the temporary files
make fclean			# delete the temporary files + the binary
make re				# make fclean and make all together
```
### Execution
```bash
# execute the sorting program:
./codexion <number_of_coders> <time_to_burnout> <time_to_compile>
           <time_to_debug> <time_to_refactor> <compiles_required>
           <dongle_cooldown> <scheduler>
# (time is in ms, scheduler = fifo/edf)

# check for leaks and thread races:
valgrind --tool=helgrind ./codexion <args>

# check the number of actions performed:
./codexion <args> | wc -l

# example:
# 2 coders, should compile 3 times each and stop (expected time 1450 ms)
./codexion 2 800 200 100 100 3 50 fifo
valgrind --tool=helgrind ./codexion 2 800 200 100 100 3 50 fifo
./codexion 2 800 200 100 100 3 50 fifo | grep compiling | wc -l
```

## Resources
- discussing thread using, deadlocks, mutexes, norm polishing etc. + debugging with `LLMs (Claude, ChatGPT)`
- talking to my `peers` about the subject and certain parts of it and letting them test my program (they broke it succesfully)
- reading the `42 global Slack channel` discussions regarding this project


## Implementation

### Blocking cases handled
#### Deadlock Prevention
Deadlock occurs when all coders hold one dongle and wait for the next, creating a circular dependency.

I solved this by dividing coders into two groups. The first one grabs the right dongle first and the second goes for the left one instead.

The group division differs according to the total number of coders:

1) even number of coders: even and odd coders behave differently
2) odd number of coders: all behave the same, except for the last one

```c
void	grab_dongles(t_coder *coder)
{
...
	if ((sim->args->number_of_coders % 2 == 0
			&& coder->id % 2 == 0)
		|| (sim->args->number_of_coders % 2 != 0
			&& coder->id == sim->args->number_of_coders))
		grab_both_dongles(coder, coder->left_dongle, coder->right_dongle);
	else
		grab_both_dongles(coder, coder->right_dongle, coder->left_dongle);
...
}
```
#### Starvation Prevention
With FIFO scheduling, coders are served in arrival order ensuring no coder waits indefinitely.

With EDF scheduling, the coder with the earliest burnout deadline gets priority, preventing the most urgent coder from starving.

#### Cooldown Handling
After a dongle is released, it enters a cooldown period. Waiting coders use `pthread_cond_timedwait` to sleep exactly until the cooldown expires, then recheck availability — avoiding both busy waiting and missing the window.

Also described in the section [Thread synchronization mechanisms](#thread-synchronization-mechanisms).

#### Precise Burnout Detection
A dedicated monitor thread checks all coders every 0,1ms. It checks for burnouts and achieved compilations in order to stop the simulation soon enough.

Also described in the section [Thread synchronization mechanisms](#thread-synchronization-mechanisms).

#### Log Serialization
All log messages are protected by a shared log_mutex. The stop flag is checked while holding the mutex, ensuring no messages are printed after burnout is announced.

Also described in the section [Thread synchronization mechanisms](#thread-synchronization-mechanisms).

### Thread synchronization mechanisms
The simulation has its own monitoring thread which goes through a `monitor routine`.
```c
pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim);
pthread_join(sim->monitor_thread, NULL);

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;

	sim = (t_simulation *)arg;
	while (1)
	{
		// checks if the stop flag is TRUE
		if (end_of_simulation(sim))
			break ;
		// checks for burnout or achieved enough compilations
		if (check_burnout(sim) || check_compilations(sim))
			break ;
		// repeats each 0,1 ms
		usleep(100);
	}
	return (NULL);
}
```
Each coder in the simulation is represented by one thread and goes through a `coder routine`.
```c
i = 0;
while (i < sim->args->number_of_coders)
{
	pthread_create(&sim->coder_threads[i], NULL, coder_routine,
		&sim->coders[i]);
	i++;
}
i = 0;
while (i < sim->args->number_of_coders)
{
	pthread_join(sim->coder_threads[i], NULL);
	i++;
}

void	*coder_routine(void *arg)
{
	t_coder			*coder;
	t_simulation	*sim;

	coder = (t_coder *)arg;
	sim = coder->simulation;
	while (1)
	{
		// after each step checks for the end of simulation
		if (end_of_simulation(sim))
			break ;
		grab_dongles(coder);
		if (end_of_simulation(sim))
			break ;
		compile(coder);
		if (end_of_simulation(sim))
			break ;
		debug(coder);
		if (end_of_simulation(sim))
			break ;
		refactor(coder);
	}
	return (NULL);
}
```

#### Mutexes

Because threads are running their routines simultaneously, it's necessary to prevent them from `race conditions` - like two coders taking the same dongle or writing a log on the same line.

This is achieved by using `mutexes` - basicly a locks that prevent ,multiple threads from accessing shared resources.

If a thread is trying to reach something that is currently "locked", it waits until it's free again.

In my code, I use mutexes for:

- Dongles - so they are not taken by multiple coders
- Coders - so `last_compile_start` and `compilations` are not read at the same time as being updated
- `Stop` flag - so it is not read at the same time as being updated
- Logs - so there is only one log at a line

For dongles specifically I have also a `condition` set up.

Normally the threads would try to reach the data under mutex constantly. But since the dongle unavailability lasts a longer time (when it's held and used during compilation, cooldown), for the CPU efficiency it is better to set up a condition. In this case the threads just wait for the condition to come true before trying again - the condition can be specified timestamp or a specific signal called `broadcast`.

```c
static void	wait_for_dongle(t_dongle *dongle, t_coder *coder, int pos)
{
...
	if (dongle->available_at > get_current_ms())
	{
		ts = ms_to_timespec(dongle->available_at);
		// wait for specific timestamp in the future or a signal
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	}
	else
		// just wait for signal (broadcast)
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
...
}

void	release_dongle(t_dongle *dongle, t_simulation *sim)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = TRUE;
	dongle->available_at = get_current_ms() + sim->args->dongle_cooldown;
	// heyy, I am released! check me!
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
```

---

## 👤 Author

**Simona Sucha**
*(also known as ssucha or vincent_syma)* <br>
Python & C · Software Developer · 42 student

🖥️ GitHub: https://github.com/vincent-syma/ <br>
🔗 LinkedIn: https://www.linkedin.com/in/simona-such%C3%A1-5a1b1928b <br>
✉️ Email: vincent.f.syma@email.cz <br>