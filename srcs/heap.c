/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:47:02 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** A plain binary min-heap of t_wait_entry, ordered by wait_entry_before()
** (see scheduler.c). The heap itself has no notion of "fifo" or "edf":
** it only ever asks "does a belong before b?", so it works unchanged for
** either scheduling policy. heap_sift_up()/heap_sift_down(), used below
** to keep the heap property after every insert/removal, live in
** heap_sift.c.
*/
void	heap_init(t_heap *heap, t_wait_entry *storage, int cap)
{
	heap->entries = storage;
	heap->size = 0;
	heap->capacity = cap;
}

int	heap_is_empty(t_heap *heap)
{
	return (heap->size == 0);
}

void	heap_push(t_heap *heap, t_wait_entry entry)
{
	heap->entries[heap->size] = entry;
	heap_sift_up(heap, heap->size);
	heap->size++;
}

t_wait_entry	heap_pop_min(t_heap *heap)
{
	t_wait_entry	top;

	top = heap->entries[0];
	heap->size--;
	heap->entries[0] = heap->entries[heap->size];
	heap_sift_down(heap, 0);
	return (top);
}

int	heap_top_is(t_heap *heap, int coder_id)
{
	return (heap->entries[0].coder_id == coder_id);
}
