/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_dongles.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:45:37 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	lower_dongle(t_coder *coder)
{
	if (coder->left_dongle < coder->right_dongle)
		return (coder->left_dongle);
	return (coder->right_dongle);
}

static int	higher_dongle(t_coder *coder)
{
	if (coder->left_dongle > coder->right_dongle)
		return (coder->left_dongle);
	return (coder->right_dongle);
}

/*
** Acquires a coder's two dongles in a fixed, global order: the
** lower-indexed one first, then the higher-indexed one. Every coder
** follows this same rule, which is what makes a circular wait (and
** therefore a deadlock) impossible: a coder can only ever be blocked
** waiting for a dongle held by someone who is not, in turn, waiting on
** a lower-indexed dongle that this coder holds.
** With a single coder (number_of_coders == 1) both indexes are the same
** dongle; the second acquisition then blocks forever (there is only one
** dongle to hand out twice), which is intentional: this coder can never
** actually compile, and the monitor thread's burnout detection is what
** eventually ends the simulation for this legitimately infeasible case.
** Returns 1 if both dongles are now held, or 0 if the attempt was
** abandoned because the simulation stopped (in which case any dongle
** already held by this coder is released before returning).
*/
int	coder_acquire_both_dongles(t_coder *coder)
{
	t_sim	*sim;
	int		lo;
	int		hi;

	sim = coder->sim;
	lo = lower_dongle(coder);
	hi = higher_dongle(coder);
	if (dongle_acquire(&sim->dongles[lo], coder) == 0)
		return (0);
	log_event(sim, coder->id, "has taken a dongle");
	if (dongle_acquire(&sim->dongles[hi], coder) == 0)
	{
		dongle_release(&sim->dongles[lo], sim);
		return (0);
	}
	log_event(sim, coder->id, "has taken a dongle");
	return (1);
}

void	coder_release_both_dongles(t_coder *coder)
{
	t_sim	*sim;
	int		lo;
	int		hi;

	sim = coder->sim;
	lo = lower_dongle(coder);
	hi = higher_dongle(coder);
	dongle_release(&sim->dongles[lo], sim);
	if (hi != lo)
		dongle_release(&sim->dongles[hi], sim);
}
