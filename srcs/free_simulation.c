/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:46:46 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	dongles_destroy(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].lock);
		pthread_cond_destroy(&sim->dongles[i].cond);
		free(sim->dongles[i].waiters.entries);
		i++;
	}
}

/*
** Tears down a fully-initialized simulation: every dongle's mutex,
** condition variable and heap storage, both top-level arrays, and every
** simulation-wide lock. Only called once every coder and the monitor
** thread have already been joined, so nothing is still using any of it.
*/
void	sim_destroy(t_sim *sim)
{
	dongles_destroy(sim);
	free(sim->dongles);
	free(sim->coders);
	pthread_mutex_destroy(&sim->seq_lock);
	pthread_mutex_destroy(&sim->log_lock);
	pthread_mutex_destroy(&sim->state_lock);
	pthread_mutex_destroy(&sim->stop_lock);
	pthread_cond_destroy(&sim->stop_cond);
}
