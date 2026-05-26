/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_cleanup.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/07 14:52:30 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <stdlib.h>		// free

/**
 * @brief Free t_simulation struct and all nested structures inside.
 * Destroy all mutexes and cods inside.
 */
void	free_simulation(t_simulation *sim)
{
	int	i;

	if (!sim)
		return ;
	if (sim->args && sim->dongles)
	{
		i = 0;
		while (i < sim->args->number_of_coders)
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			pthread_cond_destroy(&sim->dongles[i].cond);
			pthread_mutex_destroy(&sim->coders[i].mutex);
			i++;
		}
		free(sim->dongles);
	}
	if (sim->coders)
		free(sim->coders);
	if (sim->coder_threads)
		free(sim->coder_threads);
	if (sim->args)
		free(sim->args);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	free(sim);
}
