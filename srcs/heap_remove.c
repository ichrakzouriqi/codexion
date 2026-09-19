/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_remove.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:46:51 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	heap_find_index(t_heap *heap, int coder_id)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->entries[i].coder_id == coder_id)
			return (i);
		i++;
	}
	return (-1);
}

/*
** Removes one coder's own ticket from a dongle's wait heap. Needed when
** a coder gives up on a dongle it was waiting for (because the
** simulation just stopped) without ever being granted it: without this,
** a stale ticket would be left behind pointing at a coder that is no
** longer waiting for anything.
** The replaced entry may need to move either up or down to restore the
** heap property, so both are attempted; whichever one does not apply
** simply does nothing.
*/
void	heap_remove_coder(t_heap *heap, int coder_id)
{
	int	index;

	index = heap_find_index(heap, coder_id);
	if (index == -1)
		return ;
	heap->size--;
	heap->entries[index] = heap->entries[heap->size];
	if (index < heap->size)
	{
		heap_sift_down(heap, index);
		heap_sift_up(heap, index);
	}
}
