/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:47:18 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	dongle_init_one(t_dongle *dongle)
{
	dongle->taken = 0;
	dongle->available_at_ms = 0;
	pthread_mutex_init(&dongle->lock, NULL);
	pthread_cond_init(&dongle->cond, NULL);
	heap_init(&dongle->waiters, NULL, 0);
}

/*
** A dongle can never have more waiters than there are coders in total
** (each coder only ever waits for one dongle at a time), so a fixed
** capacity of number_of_coders per heap is always enough; no resizing
** is ever needed.
*/
static int	dongle_alloc_heap(t_dongle *dongle, int capacity)
{
	t_wait_entry	*storage;

	storage = malloc(sizeof(t_wait_entry) * (size_t)capacity);
	if (storage == NULL)
		return (0);
	heap_init(&dongle->waiters, storage, capacity);
	return (1);
}

/*
** There is one dongle for every coder (one between each adjacent pair
** in the circle); see init_coders.c for how left/right indexes are
** derived from that.
*/
int	dongles_init(t_sim *sim)
{
	int	i;
	int	n;

	n = sim->params.number_of_coders;
	i = 0;
	while (i < n)
	{
		dongle_init_one(&sim->dongles[i]);
		if (dongle_alloc_heap(&sim->dongles[i], n) == 0)
			return (0);
		i++;
	}
	return (1);
}
