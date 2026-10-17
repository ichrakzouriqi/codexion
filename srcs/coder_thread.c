/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_thread.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:32:01 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


void	*coder_thread_run(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (simulation_is_stopped(coder->sim) == 0)
	{
		if (coder_acquire_both_dongles(coder) == 0)
			break ;
		if (coder_do_compile(coder) == 0)
		{
			coder_release_both_dongles(coder);
			break ;
		}
		coder_release_both_dongles(coder);
		if (coder_do_debug(coder) == 0)
			break ;
		if (coder_do_refactor(coder) == 0)
			break ;
	}
	return (NULL);
}
