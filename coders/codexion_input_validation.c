/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_input_validation.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssucha <ssucha@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:06 by ssucha            #+#    #+#             */
/*   Updated: 2026/05/14 16:01:49 by ssucha           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>		// printf
#include <string.h>     // strcmp
#include <limits.h>

/**
 * @brief Convert a string to a long.
 * @return The converted value or 0 on error.
 */
static long	ft_atol(const char *str)
{
	int		i;
	int		neg_count;
	long	result;

	i = 0;
	neg_count = 1;
	result = 0;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			neg_count = -neg_count;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * neg_count);
}

/**
 * @brief Validates if arguments are pure numbers not beginning wih 0 or +.
 * @return TRUE if arguments are ok, FALSE if they are not.
 */
static t_bool	validate_numbers(char **argv)
{
	int		i;
	int		j;
	long	val;

	i = 1;
	while (i < 8)
	{
		val = ft_atol(argv[i]);
		if (strlen(argv[i]) > 11 || val > INT_MAX || val < INT_MIN)
			return (FALSE);
		j = 0;
		if (argv[i][j] == '0' && strlen(argv[i]) > 1)
			return (FALSE);
		while (argv[i][j])
		{
			if (!(argv[i][j] >= '0' && argv[i][j] <= '9'))
				return (FALSE);
			j++;
		}
		i++;
	}
	return (TRUE);
}

/**
 * @brief Validates CLI arguments for Codexion program.
 * @return TRUE if arguments are ok, FALSE if they are not.
 */
t_bool	validate_args(int argc, char **argv)
{
	int		i;

	if (argc != 9)
	{
		print_error(ERR_ARGC);
		return (printf("Correct usage:\n" YEL "./codexion" RESET
				" <number_of_coders> <time_to_burnout> <time_to_compile> "
				"<time_to_debug>\n\t   <time_to_refactor> <compiles_required>"
				"<dongle_cooldown> <scheduler>\n"), FALSE);
	}
	if (ft_atol(argv[1]) > 1000)
		return (print_error(ERR_CODERS), FALSE);
	i = 1;
	while (i < 7)
		if (ft_atol(argv[i++]) <= 0)
			return (print_error(ERR_ARGS), FALSE);
	if (ft_atol(argv[7]) < 0)
		return (print_error(ERR_COMP), FALSE);
	if (!validate_numbers(argv))
		return (print_error(ERR_ARGS), FALSE);
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
		return (print_error(ERR_SCHED), FALSE);
	return (TRUE);
}
