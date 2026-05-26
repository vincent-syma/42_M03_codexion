/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_monitor_routine.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/26 17:30:33 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <unistd.h>     // usleep
#include <stdio.h>      // printf

/**
 * @brief Sets the stop flag to TRUE and broadcasts to all coders
 * waiting for dongles that they should wake up because the simulation ended.
 * Protected with stop and dongle mutex.
 */
static void	stop_simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = TRUE;
	pthread_mutex_unlock(&sim->stop_mutex);
	while (i < sim->args->number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

/**
 * @brief Checks all coders in the simulation if they reached the burnout
 * deadline. If any coder reached that, prints the burnout log, changes the
 * simulation stop flag to TRUE and returns TRUE.
 * Protected with coder, log and stop mutex.
 * @return TRUE if any coder reached burnout and the simulation should end,
 * FALSE if not.
 */
static t_bool	check_burnout(t_simulation *sim)
{
	int		i;
	int		burnout;
	long	last;

	i = 0;
	burnout = sim->args->time_to_burnout;
	while (i < sim->args->number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].mutex);
		last = sim->coders[i].last_compile_start;
		pthread_mutex_unlock(&sim->coders[i].mutex);
		if (get_current_ms() - last > burnout)
		{
			sim->burnout = get_current_ms();
			log_print(sim, "burned out", sim->coders[i].id);
			stop_simulation(sim);
			return (TRUE);
		}
		i++;
	}
	return (FALSE);
}

/**
 * @brief Checks all coders in the simulation if they reached the number
 * of required compilations. If all coders reached that, changes the
 * simulation stop flag to TRUE and returns TRUE.
 * Protected with coder and stop mutex.
 * @return TRUE if all coders reached enough compilations
 * and the simulation should end, FALSE if not.
 */
static t_bool	check_compilations(t_simulation *sim)
{
	int	i;
	int	compiled_enough;
	int	treshold;
	int	compilations;

	i = 0;
	compiled_enough = 0;
	treshold = sim->args->number_of_compiles_required;
	while (i < sim->args->number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].mutex);
		compilations = sim->coders[i].compilations;
		pthread_mutex_unlock(&sim->coders[i].mutex);
		if (compilations >= treshold)
			compiled_enough++;
		i++;
	}
	if (compiled_enough == sim->args->number_of_coders)
	{
		stop_simulation(sim);
		return (TRUE);
	}
	return (FALSE);
}

/**
 * @brief Monitoring thread routine. Checks coder burnouts and achieved
 * compilations each 1 ms and after reaching the condition it sets the
 * stop flag to TRUE to stop the simulation.
 * @param arg t_simulation *
 */
void	*monitor_routine(void *arg)
{
	t_simulation	*sim;

	sim = (t_simulation *)arg;
	while (1)
	{
		if (end_of_simulation(sim))
			break ;
		if (check_burnout(sim) || check_compilations(sim))
			break ;
		usleep(100);
	}
	return (NULL);
}
