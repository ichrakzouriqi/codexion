/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:47:32 by izouriqi          #+#    #+#             */
/*   Updated: 2026/09/19 19:47:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	sim_init_locks(t_sim *sim)
{
	pthread_mutex_init(&sim->seq_lock, NULL);
	pthread_mutex_init(&sim->log_lock, NULL);
	pthread_mutex_init(&sim->state_lock, NULL);
	pthread_mutex_init(&sim->stop_lock, NULL);
	pthread_cond_init(&sim->stop_cond, NULL);
}

static int	sim_init_arrays(t_sim *sim)
{
	int	n;

	n = sim->params.number_of_coders;
	sim->dongles = malloc(sizeof(t_dongle) * (size_t)n);
	sim->coders = malloc(sizeof(t_coder) * (size_t)n);
	if (sim->dongles == NULL || sim->coders == NULL)
		return (0);
	return (1);
}

/*
** Builds the whole simulation from validated parameters: the clock
** origin, every lock, both arrays, every dongle and every coder. Nothing
** here is a global: sim is a single struct the caller owns, and every
** thread created later receives a pointer into it.
*/
int	sim_init(t_sim *sim, t_params *params)
{
	sim->params = *params;
	sim->dongles = NULL;
	sim->coders = NULL;
	sim->start_time_ms = time_now_ms();
	sim->seq_counter = 0;
	sim->stopped = 0;
	sim_init_locks(sim);
	if (sim_init_arrays(sim) == 0)
		return (0);
	if (dongles_init(sim) == 0)
		return (0);
	coders_init(sim);
	return (1);
}
