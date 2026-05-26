/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_create_simulation.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/22 18:10:58 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>		// malloc, free, atoi
#include <string.h>     // memset

/**
 * @brief Allocate the t_args struct and fill it with argv.
 * @return t_args struct
 */
static t_args	*store_args(char **argv)
{
	t_args	*args;

	args = malloc(sizeof(t_args));
	if (!args)
		return (NULL);
	args->number_of_coders = atoi(argv[1]);
	args->time_to_burnout = (long)atoi(argv[2]);
	args->time_to_compile = (long)atoi(argv[3]);
	args->time_to_debug = (long)atoi(argv[4]);
	args->time_to_refactor = (long)atoi(argv[5]);
	args->number_of_compiles_required = (long)atoi(argv[6]);
	args->dongle_cooldown = (long)atoi(argv[7]);
	args->scheduler = argv[8];
	return (args);
}

/**
 * @brief Create array of t_dongle structs and initialize them.
 * @return array of t_dongle structs
 */
static t_dongle	*create_dongles(int number_of_coders)
{
	t_dongle	*dongles;
	int			i;

	i = 0;
	dongles = malloc(sizeof(t_dongle) * number_of_coders);
	if (!dongles)
		return (NULL);
	while (i < number_of_coders)
	{
		dongles[i].id = i + 1;
		dongles[i].available = TRUE;
		dongles[i].queue_ids[0] = 0;
		dongles[i].queue_ids[1] = 0;
		dongles[i].fifo_queue_ts[0] = 0;
		dongles[i].fifo_queue_ts[1] = 0;
		dongles[i].edf_queue_ts[0] = 0;
		dongles[i].edf_queue_ts[1] = 0;
		dongles[i].available_at = get_current_ms();
		pthread_mutex_init(&dongles[i].mutex, NULL);
		pthread_cond_init(&dongles[i].cond, NULL);
		i++;
	}
	return (dongles);
}

/**
 * @brief Create array of t_coder structs and initialize them.
 * @return array of t_coder structs
 */
static t_coder	*create_coders(int number_of_coders,
	t_dongle *dongles, t_simulation *sim)
{
	t_coder	*coders;
	int		i;

	i = 0;
	coders = malloc(sizeof(t_coder) * number_of_coders);
	if (!coders)
		return (NULL);
	while (i < number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].compilations = 0;
		coders[i].last_compile_start = 0;
		coders[i].left_dongle = &dongles[i];
		if (i < number_of_coders - 1)
			coders[i].right_dongle = &dongles[i + 1];
		else
			coders[i].right_dongle = &dongles[0];
		coders[i].simulation = sim;
		pthread_mutex_init(&coders[i].mutex, NULL);
		i++;
	}
	return (coders);
}

/**
 * @brief Allocate array of pthread_t.
 * @return array of pthread_t
 */
static pthread_t	*create_coder_threads(int number_of_coders)
{
	pthread_t	*threads;

	threads = malloc(sizeof(pthread_t) * number_of_coders);
	if (!threads)
		return (NULL);
	return (threads);
}

/**
 * @brief Create the simulation struct and initialize all the variables
 * and structs inside.
 * @return t_simulation struct
 */
t_simulation	*create_simulation(char **argv)
{
	t_simulation	*sim;

	sim = malloc(sizeof(t_simulation));
	if (!sim)
		return (NULL);
	sim->args = store_args(argv);
	if (sim->args == NULL)
		return (init_error(sim, "Failed to store args"));
	sim->dongles = create_dongles(sim->args->number_of_coders);
	if (sim->dongles == NULL)
		return (init_error(sim, "Failed to create dongles"));
	sim->coders = create_coders(sim->args->number_of_coders, sim->dongles, sim);
	if (sim->coders == NULL)
		return (init_error(sim, "Failed to create coders"));
	sim->coder_threads = create_coder_threads(sim->args->number_of_coders);
	if (sim->coder_threads == NULL)
		return (init_error(sim, "Failed to create coder threads"));
	memset(&sim->monitor_thread, 0, sizeof(pthread_t));
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->stop_mutex, NULL);
	sim->start_time = 0;
	sim->stop = FALSE;
	sim->burnout = 0;
	return (sim);
}
