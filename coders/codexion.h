/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/09 19:52:23 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/26 17:32:30 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
// # include <stdbool.h>

typedef int					t_bool;
# define TRUE	1
# define FALSE	0

// ANSI COLORS for prints
# define RESET   "\033[0m"
# define BOLD    "\033[1m"
# define RED     "\033[31m"
# define GRN     "\033[32m"
# define YEL     "\033[33m"
# define BLU     "\033[34m"

// ERROR MESSAGES
# define ERR_ARGC	"Too few/many arguments. 8 expected."
# define ERR_CODERS	"Too many coders. Max: 1000"
# define ERR_ARGS	"Arguments 1-7 must be valid positive integers"
# define ERR_SCHED	"Last argument <scheduler> must be either 'fifo' or 'edf'"
# define ERR_COMP	"Argument 7 <compiles_required> cannot be negative."

typedef struct s_args
{
	int		number_of_coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	long	number_of_compiles_required;
	long	dongle_cooldown;
	char	*scheduler;
}	t_args;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_bool			available;
	int				id;					// for debugging purposes mainly
	int				queue_ids[2];		// coder IDs waiting (0=empty)
	long			fifo_queue_ts[2];	// request timestamps (0=empty)
	long			available_at;		// timestamp in future
	long			edf_queue_ts[2];	// coders burnout deadlines
										// - timestamps in future
}	t_dongle;

typedef struct s_simulation	t_simulation;

typedef struct s_coder
{
	int				id;
	int				compilations;
	long			last_compile_start;
	pthread_mutex_t	mutex;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_simulation	*simulation;
}	t_coder;

typedef struct s_simulation
{
	t_args			*args;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_t		*coder_threads; // add them inside coders?
	pthread_t		monitor_thread;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	stop_mutex;
	t_bool			stop;
	long			burnout;
	long			start_time;
}	t_simulation;

// ERROR CHECK

t_bool				validate_args(int argc, char **argv);
void				print_error(char *message);
void				*init_error(t_simulation *sim, char *message);

// TIME

struct timespec		ms_to_timespec(long ms);
long				get_current_ms(void);
long				get_time_ms(long start);

// THE SIMULATION

t_simulation		*create_simulation(char **argv);
void				run_simulation(t_simulation *sim);
t_bool				end_of_simulation(t_simulation *sim);

void				*coder_routine(void *arg);
// int					join_queue(t_dongle *dongle, t_coder *coder);
// void				leave_queue(t_dongle *dongle, int pos);
// void				grab_dongles(t_coder *coder);
// void				release_dongle(t_dongle *dongle, t_simulation *sim);
t_bool				log_print(t_simulation *sim, char *msg, int id);
t_bool				controlled_sleep(t_simulation *sim, long ms);

void				*monitor_routine(void *arg);

// CLEANUP

void				free_simulation(t_simulation *sim);

// DEBUG PRINT

void				debug_print_simulation(t_simulation *sim);

#endif