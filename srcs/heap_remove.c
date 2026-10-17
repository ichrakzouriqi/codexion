/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_remove.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:32:50 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
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
