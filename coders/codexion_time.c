/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_time.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/20 14:38:56 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <sys/time.h>   // gettimeofday
#include <unistd.h>     // usleep
#include <stdio.h>      // printf

struct timespec	ms_to_timespec(long ms)
{
	struct timespec	ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	return (ts);
}

/**
 * @brief Gets current timestamp in miliseconds.
 * @return
 */
long	get_current_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/**
 * @brief Subtracts the current timestamp from the start.
 * @return
 */
long	get_time_ms(long start)
{
	return (get_current_ms() - start);
}

/**
 * @brief Sleep for given amount of time in ms but check each ms
 * if the simulation is still on.
 * Protected with stop mutex.
 * @return True if sleep was finished,
 * False if the simulation ended before the sleep end.
 */
t_bool	controlled_sleep(t_simulation *sim, long ms)
{
	long	end;
	int		i;

	end = get_current_ms() + ms;
	while (get_current_ms() < end)
	{
		i = 0;
		while (i < 10)
		{
			if (end_of_simulation(sim))
				return (FALSE);
			usleep(100);
			i++;
		}
	}
	return (TRUE);
}
