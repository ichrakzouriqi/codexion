/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_stop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:33:50 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	simulation_is_stopped(t_sim *sim)
{
	int	value;

	pthread_mutex_lock(&sim->stop_lock);
	value = sim->stopped;
	pthread_mutex_unlock(&sim->stop_lock);
	return (value);
}


static void	wake_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].lock);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].lock);
		i++;
	}
}

void	simulation_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_lock);
	sim->stopped = 1;
	pthread_cond_broadcast(&sim->stop_cond);
	pthread_mutex_unlock(&sim->stop_lock);
	wake_all_dongles(sim);
}
