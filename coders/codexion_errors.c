/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_errors.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/05 19:01:05 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>      // printf, fprintf

/**
 * @brief Print a custom error message on stderr.
 */
void	print_error(char *message)
{
	fprintf(stderr, RED "Error: " RESET "%s\n", message);
}

/**
 * @brief Print custom error message on stderr and free simulation.
 * @return NULL
 */
void	*init_error(t_simulation *sim, char *message)
{
	print_error(message);
	free_simulation(sim);
	return (NULL);
}
