/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:33:21 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


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
