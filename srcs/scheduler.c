/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:48:13 by izouriqi          #+#    #+#             */
/*   Updated: 2026/09/19 19:48:16 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Gives out a fresh, strictly increasing ticket number: the arrival
** order used by the fifo policy.
*/
static long	sequence_next(t_sim *sim)
{
	long	value;

	pthread_mutex_lock(&sim->seq_lock);
	sim->seq_counter++;
	value = sim->seq_counter;
	pthread_mutex_unlock(&sim->seq_lock);
	return (value);
}

/*
** A coder's current burnout deadline: last_compile_start_ms +
** time_to_burnout, exactly as defined by the subject. Reading
** last_compile_start_ms requires sim->state_lock because the coder's
** own thread updates it every time it starts a new compile.
*/
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

/*
** Builds the ticket a coder presents when it starts waiting for a
** dongle. This is the only place the active scheduler policy matters:
** - fifo: primary_key is the arrival ticket, so first request wins.
** - edf : primary_key is the coder's burnout deadline, so whoever is
**         closest to burning out is served first.
** secondary_key (the coder's own id) is only ever used to break a tie
** between two equal primary_key values.
*/
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

/*
** Returns 1 if entry a must be served before entry b. Lower keys win.
**
** Recode note: this is the single, isolated spot that decides EDF
** tie-breaking. As written, a lower coder_id wins a tie. To make a
** higher coder_id win instead, change the line below to:
**   return (a->secondary_key > b->secondary_key);
*/
int	wait_entry_before(t_wait_entry *a, t_wait_entry *b)
{
	if (a->primary_key != b->primary_key)
		return (a->primary_key < b->primary_key);
	return (a->secondary_key < b->secondary_key);
}
