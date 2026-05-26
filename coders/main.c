/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/26 17:33:14 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
// #include <stdio.h>      // printf, fprintf

// static void	debug_print_simulation(t_simulation *sim); // helper

int	main(int argc, char **argv)
{
	t_simulation	*sim;

	if (!validate_args(argc, argv))
		return (1);
	sim = create_simulation(argv);
	if (sim == NULL)
		return (init_error(NULL, "Failed to create the simulation"), 1);
	run_simulation(sim);
	debug_print_simulation(sim);	// the simulation summary
	free_simulation(sim);
	return (0);
}
