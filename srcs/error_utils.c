/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:32:29 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_usage_error(const char *prog_name)
{
	fprintf(stderr, "Usage: %s number_of_coders time_to_burnout ", prog_name);
	fprintf(stderr, "time_to_compile time_to_debug time_to_refactor ");
	fprintf(stderr, "number_of_compiles_required dongle_cooldown scheduler\n");
	fprintf(stderr, "All 8 arguments are mandatory non-negative integers, ");
	fprintf(stderr, "except scheduler (\"fifo\" or \"edf\"); ");
	fprintf(stderr, "number_of_coders must additionally be at least 1.\n");
}


void	exit_with_error(const char *msg)
{
	fprintf(stderr, "codexion: error: %s\n", msg);
	exit(1);
}
