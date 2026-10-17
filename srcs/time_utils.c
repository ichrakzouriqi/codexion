/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:49:04 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	time_now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long)tv.tv_sec * 1000 + (long)tv.tv_usec / 1000);
}

void	ms_to_timespec(long ms, struct timespec *ts)
{
	ts->tv_sec = ms / 1000;
	ts->tv_nsec = (ms % 1000) * 1000000;
}


int	sleep_interruptible(t_sim *sim, long duration_ms)
{
	long			deadline;
	struct timespec	ts;
	int				stopped_now;

	deadline = time_now_ms() + duration_ms;
	ms_to_timespec(deadline, &ts);
	pthread_mutex_lock(&sim->stop_lock);
	while (sim->stopped == 0 && time_now_ms() < deadline)
		pthread_cond_timedwait(&sim->stop_cond, &sim->stop_lock, &ts);
	stopped_now = sim->stopped;
	pthread_mutex_unlock(&sim->stop_lock);
	return (stopped_now);
}
