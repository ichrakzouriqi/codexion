/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:32:38 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
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
