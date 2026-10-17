/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_dongles.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:31:40 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
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
