/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:33:07 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


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
