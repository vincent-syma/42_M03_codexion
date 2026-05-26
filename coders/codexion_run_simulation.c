/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_run_simulation.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/14 17:27:56 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

/**
 * @brief Creates the monitoring thread and coders threads and joins them.
 * Sets the starting time of the simulation.
 * Monitoring thread runs the monitor routine and coders run the coder routine.
 * @param arg t_simulation *
 */
void	run_simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	sim->start_time = get_current_ms();
	while (i < sim->args->number_of_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		i++;
	}
	i = 0;
	pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim);
	while (i < sim->args->number_of_coders)
	{
		pthread_create(&sim->coder_threads[i], NULL, coder_routine,
			&sim->coders[i]);
		i++;
	}
	pthread_join(sim->monitor_thread, NULL);
	i = 0;
	while (i < sim->args->number_of_coders)
	{
		pthread_join(sim->coder_threads[i], NULL);
		i++;
	}
}

/**
 * @brief Checks if the simulation flag stop is true or false.
 * Protected with stop mutex.
 * @return True if simulation should stop, False if it should continue.
 */
t_bool	end_of_simulation(t_simulation *sim)
{
	t_bool	stop;

	pthread_mutex_lock(&sim->stop_mutex);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stop);
}
