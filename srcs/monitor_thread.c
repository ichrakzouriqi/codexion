/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_thread.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:48:01 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


static int	monitor_check_burnout(t_sim *sim, long now)
{
	int		i;
	int		burned_id;
	long	deadline;

	i = 0;
	burned_id = -1;
	pthread_mutex_lock(&sim->state_lock);
	while (i < sim->params.number_of_coders && burned_id == -1)
	{
		deadline = sim->coders[i].last_compile_start_ms
			+ sim->params.time_to_burnout;
		if (now > deadline)
			burned_id = sim->coders[i].id;
		i++;
	}
	pthread_mutex_unlock(&sim->state_lock);
	if (burned_id == -1)
		return (0);
	log_event(sim, burned_id, "burned out");
	simulation_stop(sim);
	return (1);
}

static int	monitor_check_all_done(t_sim *sim)
{
	int	i;
	int	all_done;

	i = 0;
	all_done = 1;
	pthread_mutex_lock(&sim->state_lock);
	while (i < sim->params.number_of_coders)
	{
		if (sim->coders[i].compiles_done
			< sim->params.number_of_compiles_required)
			all_done = 0;
		i++;
	}
	pthread_mutex_unlock(&sim->state_lock);
	if (all_done == 1)
		simulation_stop(sim);
	return (all_done);
}


void	*monitor_thread_run(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (1)
	{
		usleep(MONITOR_TICK_US);
		if (monitor_check_burnout(sim, time_now_ms()) == 1)
			break ;
		if (monitor_check_all_done(sim) == 1)
			break ;
	}
	return (NULL);
}
