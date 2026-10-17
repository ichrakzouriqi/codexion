/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_acquire.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:32:12 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


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
