/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_sift.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:46:57 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	heap_swap(t_heap *heap, int a, int b)
{
	t_wait_entry	tmp;

	tmp = heap->entries[a];
	heap->entries[a] = heap->entries[b];
	heap->entries[b] = tmp;
}

void	heap_sift_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (wait_entry_before(&heap->entries[index],
				&heap->entries[parent]) == 0)
			break ;
		heap_swap(heap, index, parent);
		index = parent;
	}
}

void	heap_sift_down(t_heap *heap, int index)
{
	int	left;
	int	right;
	int	smallest;

	left = index * 2 + 1;
	right = index * 2 + 2;
	smallest = index;
	if (left < heap->size
		&& wait_entry_before(&heap->entries[left], &heap->entries[smallest]))
		smallest = left;
	if (right < heap->size
		&& wait_entry_before(&heap->entries[right], &heap->entries[smallest]))
		smallest = right;
	if (smallest != index)
	{
		heap_swap(heap, index, smallest);
		heap_sift_down(heap, smallest);
	}
}
