/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:47:12 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Coders sit in a circle numbered 1..n. Dongle k (0-indexed) sits
** between coder (k+1) and coder (k+2), wrapping around, so coder index
** "index" (0-indexed, id = index + 1) has dongle "index" on its right
** and dongle "index - 1" (wrapping to n - 1) on its left. With a single
** coder this makes left and right the same one dongle, which is the
** intentional, always-burns-out edge case documented in
** coder_acquire_both_dongles().
*/
static void	coder_init_one(t_coder *coder, t_sim *sim, int index)
{
	int	n;

	n = sim->params.number_of_coders;
	coder->id = index + 1;
	coder->right_dongle = index;
	coder->left_dongle = (index - 1 + n) % n;
	coder->compiles_done = 0;
	coder->last_compile_start_ms = sim->start_time_ms;
	coder->sim = sim;
}

void	coders_init(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.number_of_coders)
	{
		coder_init_one(&sim->coders[i], sim, i);
		i++;
	}
}
