/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_coder_routine_grab_dongles.c              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/26 17:42:31 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdio.h>	  // printf
#include <string.h>	 // strcmp

int		join_queue(t_dongle *dongle, t_coder *coder);
void	leave_queue(t_dongle *dongle, int pos);

static void	wait_for_dongle(t_dongle *dongle, t_coder *coder, int pos)
{
	t_simulation	*sim;
	struct timespec	ts;

	sim = coder->simulation;
	pthread_mutex_lock(&dongle->mutex);
	while (!dongle->available || dongle->available_at > get_current_ms()
		|| (strcmp(sim->args->scheduler, "fifo") == 0
			&& dongle->fifo_queue_ts[pos] > dongle->fifo_queue_ts[1 - pos]
			&& dongle->queue_ids[1 - pos] != 0)
		|| (strcmp(sim->args->scheduler, "edf") == 0
			&& ((dongle->edf_queue_ts[pos] > dongle->edf_queue_ts[1 - pos]
				&& dongle->queue_ids[1 - pos] != 0))))
	{
		if (end_of_simulation(sim))
			return (leave_queue(dongle, pos), (void)0);
		if (dongle->available_at > get_current_ms())
		{
			ts = ms_to_timespec(dongle->available_at);
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
		if (end_of_simulation(sim))
			return (leave_queue(dongle, pos), (void)0);
	}
}

/**
 * @brief When his turn comes, coder takes the dongle
 * and erases himself from the queue.
 * Prints a log with timestamp if the simulation is still on.
 * Protected with dongle and log mutex.
 */
static void	grab_dongle(t_dongle *dongle, t_coder *coder, int pos)
{
	t_simulation	*sim;

	sim = coder->simulation;
	dongle->queue_ids[pos] = 0;
	dongle->fifo_queue_ts[pos] = 0;
	dongle->edf_queue_ts[pos] = 0;
	// leave queue without coder mutex unlock
	dongle->available = FALSE;
	pthread_mutex_unlock(&dongle->mutex);
	if (end_of_simulation(sim))
		return ;
	log_print(sim, "has taken a dongle", coder->id);	// add dongle ids
}

static void	grab_both_dongles(
	t_coder *coder, t_dongle *first, t_dongle *second)
{
	int				pos_1;
	int				pos_2;
	t_simulation	*sim;

	sim = coder->simulation;
	pos_1 = join_queue(first, coder);
	pos_2 = join_queue(second, coder);
	if (end_of_simulation(sim))
		return ;
	wait_for_dongle(first, coder, pos_1);
	if (end_of_simulation(sim))
	{
		pthread_mutex_lock(&second->mutex);
		return (leave_queue(second, pos_2));
	}
	grab_dongle(first, coder, pos_1);
	if (end_of_simulation(sim))
	{
		pthread_mutex_lock(&second->mutex);
		return (leave_queue(second, pos_2));
	}
	wait_for_dongle(second, coder, pos_2);
	if (end_of_simulation(sim))
		return ;
	grab_dongle(second, coder, pos_2);
}

/**
 * @brief Coder takes left and right dongle.
 * The order is different for the last coder id.
 */
void	grab_dongles(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->simulation;
	if (end_of_simulation(sim))
		return ;
	if ((sim->args->number_of_coders % 2 == 0
			&& coder->id % 2 == 0)
		|| (sim->args->number_of_coders % 2 != 0
			&& coder->id == sim->args->number_of_coders))
		grab_both_dongles(coder, coder->left_dongle, coder->right_dongle);
	else
		grab_both_dongles(coder, coder->right_dongle, coder->left_dongle);
}
