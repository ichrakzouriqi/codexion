/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:33:30 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
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


static int	dongle_alloc_heap(t_dongle *dongle, int capacity)
{
	t_wait_entry	*storage;

	storage = malloc(sizeof(t_wait_entry) * (size_t)capacity);
	if (storage == NULL)
		return (0);
	heap_init(&dongle->waiters, storage, capacity);
	return (1);
}


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
