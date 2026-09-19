/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args_parse.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:45:21 by izouriqi          #+#    #+#             */
/*   Updated: 2026/09/19 19:45:25 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** By the time this is called, args_validate_all() has already confirmed
** the string is only digits and short enough not to overflow an int, so
** atoi() (the only numeric-conversion function this project is allowed
** to use) is safe here.
*/
long	parse_positive_long(const char *s)
{
	return ((long)atoi(s));
}

int	parse_scheduler_mode(const char *s)
{
	if (strcmp(s, "fifo") == 0)
		return (CDX_SCHED_FIFO);
	return (CDX_SCHED_EDF);
}

void	params_fill_from_args(t_params *params, char **argv)
{
	params->number_of_coders = (int)parse_positive_long(argv[1]);
	params->time_to_burnout = parse_positive_long(argv[2]);
	params->time_to_compile = parse_positive_long(argv[3]);
	params->time_to_debug = parse_positive_long(argv[4]);
	params->time_to_refactor = parse_positive_long(argv[5]);
	params->number_of_compiles_required = (int)parse_positive_long(argv[6]);
	params->dongle_cooldown = parse_positive_long(argv[7]);
	params->scheduler_mode = parse_scheduler_mode(argv[8]);
}
