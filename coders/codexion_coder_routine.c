/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_coder_routine.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/26 17:42:53 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdio.h>      // printf

void	grab_dongles(t_coder *coder);
void	release_dongle(t_dongle *dongle, t_simulation *sim);


/**
 * @brief Prints log of coder action to the stdout.
 * Protected with log mutex.
 * @return True if logged, False if the simulation ended before the log.
 */
t_bool	log_print(t_simulation *sim, char *msg, int id)
{
	pthread_mutex_lock(&sim->log_mutex);
	if (end_of_simulation(sim))
	{
		pthread_mutex_unlock(&sim->log_mutex);
		return (FALSE);
	}
	printf("%-6ld %d %s\n", get_time_ms(sim->start_time), id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
	return (TRUE);
}

/**
 * @brief Coder compiles (log + wait + increase compilation counter
 * + store the last_compile_start). Then he releases both dongles.
 * Protected with log and coder mutex.
 */
static void	compile(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->simulation;
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_start = get_current_ms();
	pthread_mutex_unlock(&coder->mutex);
	if (log_print(sim, "is compiling", coder->id))
	{
		controlled_sleep(sim, sim->args->time_to_compile);
		release_dongle(coder->left_dongle, sim);
		release_dongle(coder->right_dongle, sim);
		pthread_mutex_lock(&coder->mutex);
		coder->compilations++;
		pthread_mutex_unlock(&coder->mutex);
	}
}

/**
 * @brief Coder debugs (log + wait). Protected with log mutex.
 */
static void	debug(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->simulation;
	if (log_print(sim, "is debugging", coder->id))
		controlled_sleep(sim, sim->args->time_to_debug);
}

/**
 * @brief Coder refactors (log + wait). Protected with log mutex.
 */
static void	refactor(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->simulation;
	if (log_print(sim, "is refactoring", coder->id))
		controlled_sleep(sim, sim->args->time_to_refactor);
}

/**
 * @brief While the simulation runs, coder grabs 2 dongles,
 * compiles, debugs, refactors in a loop. Protected with stop mutex.
 * @param arg t_coder *
 */
void	*coder_routine(void *arg)
{
	t_coder			*coder;
	t_simulation	*sim;

	coder = (t_coder *)arg;
	sim = coder->simulation;
	while (1)
	{
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
