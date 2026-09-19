/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args_check.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:44:57 by izouriqi          #+#    #+#             */
/*   Updated: 2026/09/19 19:45:05 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** The program expects exactly 8 arguments after its own name:
** number_of_coders time_to_burnout time_to_compile time_to_debug
** time_to_refactor number_of_compiles_required dongle_cooldown scheduler
*/
int	args_count_is_valid(int argc)
{
	return (argc == 9);
}

static int	char_is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

/*
** Accepts only strings made of one or more digits, at most MAX_ARG_DIGITS
** long. This rejects empty strings, signs (so negative numbers are
** rejected too), decimal points, letters, and anything long enough to
** risk overflowing atoi()'s int result.
*/
int	arg_is_valid_number(const char *s)
{
	int	i;
	int	len;

	len = (int)strlen(s);
	if (len == 0 || len > MAX_ARG_DIGITS)
		return (0);
	i = 0;
	while (i < len)
	{
		if (char_is_digit(s[i]) == 0)
			return (0);
		i++;
	}
	return (1);
}

int	arg_is_valid_scheduler(const char *s)
{
	if (strcmp(s, "fifo") == 0)
		return (1);
	if (strcmp(s, "edf") == 0)
		return (1);
	return (0);
}

/*
** number_of_coders is the one argument that also needs a semantic check
** beyond "is a valid non-negative integer": with 0 coders there would be
** nothing to simulate and a circle of 0 seats is not well defined, so it
** must be at least 1.
*/
int	args_validate_all(int argc, char **argv)
{
	int	i;

	if (args_count_is_valid(argc) == 0)
		return (0);
	i = 1;
	while (i <= 7)
	{
		if (arg_is_valid_number(argv[i]) == 0)
			return (0);
		i++;
	}
	if (arg_is_valid_scheduler(argv[8]) == 0)
		return (0);
	if (atoi(argv[1]) < 1)
		return (0);
	return (1);
}
