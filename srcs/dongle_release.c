/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_release.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:46:11 by izouriqi          #+#    #+#             */
/*   Updated: 2026/09/19 19:46:16 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Frees the dongle and starts its mandatory cooldown: it becomes
** eligible to be taken again only once dongle_cooldown milliseconds have
** passed. Broadcasting here is what wakes up every coder currently
** waiting on this specific dongle so they can re-check whether it is now
** their turn.
*/
void	dongle_release(t_dongle *dongle, t_sim *sim)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->taken = 0;
	dongle->available_at_ms = time_now_ms() + sim->params.dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}
