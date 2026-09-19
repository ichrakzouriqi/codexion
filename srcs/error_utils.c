/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:46:24 by izouriqi         ###   ########.fr       */
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

/*
** Used only for the essentially-unreachable failure of a resource
** allocation (malloc/pthread_create/...) very early in startup, before
** any coder or monitor thread exists. Exiting immediately is safe: the
** operating system reclaims every allocation, mutex and thread the
** process owns as soon as it exits.
*/
void	exit_with_error(const char *msg)
{
	fprintf(stderr, "codexion: error: %s\n", msg);
	exit(1);
}
