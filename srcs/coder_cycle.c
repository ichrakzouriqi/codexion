/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_cycle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:45:31 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Records the moment this compile started. This single timestamp is
** what the rest of the program uses both to detect this coder's next
** burnout deadline and, under edf, to prioritize it against others
** waiting for a dongle.
*/
static void	coder_mark_compile_start(t_coder *coder, long now)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_lock);
	coder->last_compile_start_ms = now;
	pthread_mutex_unlock(&sim->state_lock);
}

static void	coder_mark_compile_done(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_lock);
	coder->compiles_done++;
	pthread_mutex_unlock(&sim->state_lock);
}

/*
** Runs the compiling phase. Must be called only once both dongles are
** already held. Returns 1 if the compile ran to completion (and counts
** towards number_of_compiles_required), or 0 if it was cut short by the
** simulation stopping.
*/
int	coder_do_compile(t_coder *coder)
{
	long	now;

	now = time_now_ms();
	coder_mark_compile_start(coder, now);
	log_event(coder->sim, coder->id, "is compiling");
	if (sleep_interruptible(coder->sim, coder->sim->params.time_to_compile))
		return (0);
	coder_mark_compile_done(coder);
	return (1);
}

int	coder_do_debug(t_coder *coder)
{
	log_event(coder->sim, coder->id, "is debugging");
	if (sleep_interruptible(coder->sim, coder->sim->params.time_to_debug))
		return (0);
	return (1);
}

int	coder_do_refactor(t_coder *coder)
{
	log_event(coder->sim, coder->id, "is refactoring");
	if (sleep_interruptible(coder->sim, coder->sim->params.time_to_refactor))
		return (0);
	return (1);
}
