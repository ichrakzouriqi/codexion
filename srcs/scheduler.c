/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:48:13 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


static long	sequence_next(t_sim *sim)
{
	long	value;

	pthread_mutex_lock(&sim->seq_lock);
	sim->seq_counter++;
	value = sim->seq_counter;
	pthread_mutex_unlock(&sim->seq_lock);
	return (value);
}


static long	coder_deadline(t_coder *coder)
{
	t_sim	*sim;
	long	start;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_lock);
	start = coder->last_compile_start_ms;
	pthread_mutex_unlock(&sim->state_lock);
	return (start + sim->params.time_to_burnout);
}


t_wait_entry	wait_entry_make(t_coder *coder)
{
	t_wait_entry	entry;
	t_sim			*sim;

	sim = coder->sim;
	entry.coder_id = coder->id;
	if (sim->params.scheduler_mode == CDX_SCHED_FIFO)
		entry.primary_key = sequence_next(sim);
	else
		entry.primary_key = coder_deadline(coder);
	entry.secondary_key = coder->id;
	return (entry);
}


int	wait_entry_before(t_wait_entry *a, t_wait_entry *b)
{
	if (a->primary_key != b->primary_key)
		return (a->primary_key < b->primary_key);
	return (a->secondary_key < b->secondary_key);
}
