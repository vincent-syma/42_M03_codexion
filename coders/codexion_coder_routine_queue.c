/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_coder_routine_queue.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/22 18:10:46 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

int	join_queue(t_dongle *dongle, t_coder *coder)
{
	int				pos;
	t_simulation	*sim;

	sim = coder->simulation;
	pos = 0;
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->queue_ids[0] != 0)
		pos = 1;
	dongle->queue_ids[pos] = coder->id;
	dongle->fifo_queue_ts[pos] = get_current_ms();
	dongle->edf_queue_ts[pos] = coder->last_compile_start
		+ sim->args->time_to_burnout;
	pthread_mutex_unlock(&dongle->mutex);
	return (pos);
}

void	leave_queue(t_dongle *dongle, int pos)
{
	dongle->queue_ids[pos] = 0;
	dongle->fifo_queue_ts[pos] = 0;
	dongle->edf_queue_ts[pos] = 0;
	pthread_mutex_unlock(&dongle->mutex);
}
