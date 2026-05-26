/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_test_prints.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/26 19:07:54 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>		// printf

// static void	debug_print_dongles(t_simulation *sim);
static void	debug_print_coders(t_simulation *sim, t_bool success);

/**
 * @brief Print the simulation args + each dongle and coder info.
 */
void	debug_print_simulation(t_simulation *sim)
{
	// printf("\n=== SIMULATION ===\n");
	// printf("number_of_coders:         %d\n", sim->args->number_of_coders);
	// printf("time_to_burnout:          %d\n", sim->args->time_to_burnout);
	// printf("time_to_compile:          %d\n", sim->args->time_to_compile);
	// printf("time_to_debug:            %d\n", sim->args->time_to_debug);
	// printf("time_to_refactor:         %d\n", sim->args->time_to_refactor);
	// printf("number_of_compiles_req:   %d\n",
	// 	sim->args->number_of_compiles_required);
	// printf("dongle_cooldown:          %d\n", sim->args->dongle_cooldown);
	// printf("scheduler:                %s\n", sim->args->scheduler);
	// printf("start_time:               %ld\n", sim->start_time);

	if (sim->stop)
	{
		if (sim->burnout > 0)
		{
			printf(RED BOLD "\nENDED WITH BURNOUT AT:  %ld\n" RESET,
				sim->burnout - sim->start_time); // identify the coder?
			debug_print_coders(sim, FALSE);
		}

		else
		{
			printf(GRN BOLD "\nENDED SUCCESFULLY WITH AT LEAST %ld COMPILATIONS FOR "
				"EACH CODER\n" RESET, sim->args->number_of_compiles_required);
			debug_print_coders(sim, TRUE);
		}
		// debug_print_dongles(sim);
	}
	else
	    printf(RED BOLD "ERROR:" RESET "The simulation did not stop!\n");
	printf("\n========================\n");
}

// /**
//  * @brief Print each dongle info.
//  */
// static void	debug_print_dongles(t_simulation *sim)
// {
// 	int	i;

// 	i = 0;
// 	printf("\n=== DONGLES ===\n");
// 	while (i < sim->args->number_of_coders)
// 	{
// 		printf("dongle[%d] - ID %d:\n", i, sim->dongles[i].id);
// 		if (sim->dongles[i].available)
// 			printf(" - availability: free (available_at=%ld)\n",
// 				sim->dongles[i].available_at - sim->start_time);
// 		else
// 			printf(" - availability: taken (at %ld)\n",
// 				sim->dongles[i].available_at - sim->start_time);
// 		printf(" - waiting coders=[%d,%d] with deadlines=[%ld,%ld]\n",
// 			sim->dongles[i].coders_waiting[0],
// 			sim->dongles[i].coders_waiting[1],
// 			sim->dongles[i].coder_deadlines[0],
// 			sim->dongles[i].coder_deadlines[1]);
// 		i++;
// 	}
// }

/**
 * @brief Print each coder info.
 */
static void	debug_print_coders(t_simulation *sim, t_bool success)
{
	int	i;

	i = 0;
	printf("\n=== CODERS ===\n\n");
	while (i < sim->args->number_of_coders)
	{
		if (success == FALSE) // (sim->coders[i].compilations <
			// sim->args->number_of_compiles_required)
		{
			printf("coder %d (left_dongle=%d right_dongle=%d):\n",
				sim->coders[i].id, sim->coders[i].left_dongle->id,
			sim->coders[i].right_dongle->id);
			printf(" - compiled %dx (last time at %ld)\n",
				sim->coders[i].compilations,
				sim->coders[i].last_compile_start - sim->start_time);
			printf(RED "ERROR: " RESET "Coder have not compiled enough!\n");
		}
		else
			printf("coder %d: compiled %dx (last time at %ld)\n",
				sim->coders[i].id, sim->coders[i].compilations,
				sim->coders[i].last_compile_start - sim->start_time);
		if (sim->coders[i].last_compile_start - get_current_ms() // get the ending time better
			> sim->args->time_to_burnout)
			printf(RED BOLD "BURNOUT ALERT: " RESET "This coder should have burned out");
		i++;
	}
}
