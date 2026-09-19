/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_acquire.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:45:47 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** A dongle can be handed to this coder right now only if all three hold:
** nobody currently holds it, its post-release cooldown has passed, and
** this coder is the one the active scheduler says should go next.
*/
static int	dongle_ready_for(t_dongle *dongle, int coder_id, long now)
{
	if (dongle->taken == 1)
		return (0);
	if (now < dongle->available_at_ms)
		return (0);
	if (heap_is_empty(&dongle->waiters) == 1)
		return (0);
	return (heap_top_is(&dongle->waiters, coder_id));
}

/*
** Goes back to sleep on the dongle's condition variable until something
** worth re-checking happens: either the current holder releases it (an
** indefinite wait, woken by a broadcast), or its cooldown is still
** running (a wait bounded to exactly when that cooldown ends, so a
** dongle nobody is holding never sits idle past its cooldown without
** being re-examined).
*/
static void	dongle_wait(t_dongle *dongle, long now)
{
	struct timespec	ts;

	if (dongle->taken == 0 && now < dongle->available_at_ms)
	{
		ms_to_timespec(dongle->available_at_ms, &ts);
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
	}
	else
		pthread_cond_wait(&dongle->cond, &dongle->lock);
}

/*
** Queues this coder for the dongle and blocks until it is granted, or
** until the simulation stops. The condition variable is broadcast on
** every state change (enqueue, grant) in addition to dongle_release(),
** so that whichever waiter now has the best claim always gets a chance
** to notice and take it, with no other timing assumptions needed.
** Returns 1 once the dongle is held, or 0 if the wait was abandoned
** because the simulation stopped (in which case this coder's ticket is
** removed from the queue before returning).
*/
int	dongle_acquire(t_dongle *dongle, t_coder *coder)
{
	t_wait_entry	entry;
	int				granted;

	entry = wait_entry_make(coder);
	pthread_mutex_lock(&dongle->lock);
	heap_push(&dongle->waiters, entry);
	pthread_cond_broadcast(&dongle->cond);
	granted = 0;
	while (granted == 0 && simulation_is_stopped(coder->sim) == 0)
	{
		if (dongle_ready_for(dongle, coder->id, time_now_ms()) == 1)
		{
			heap_pop_min(&dongle->waiters);
			dongle->taken = 1;
			granted = 1;
			pthread_cond_broadcast(&dongle->cond);
		}
		else
			dongle_wait(dongle, time_now_ms());
	}
	if (granted == 0)
		heap_remove_coder(&dongle->waiters, coder->id);
	pthread_mutex_unlock(&dongle->lock);
	return (granted);
}
