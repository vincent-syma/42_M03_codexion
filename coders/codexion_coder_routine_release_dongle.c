/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_coder_routine_release_dongles.c           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/14 15:18:11 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

/**
 * @brief Coder releases the dongle. The dongle cooldown starts.
 * Coder lets know other coders waiting for the dongle.
 * Protected with dongle mutex.
 */
void	release_dongle(t_dongle *dongle, t_simulation *sim)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = TRUE;
	dongle->available_at = get_current_ms() + sim->args->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
