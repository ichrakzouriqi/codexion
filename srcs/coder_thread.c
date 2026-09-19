/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_thread.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 00:00:00 by your-login        #+#    #+#             */
/*   Updated: 2026/09/19 19:45:42 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** The full life of one coder thread: acquire both dongles, compile,
** release both dongles, debug, refactor, then immediately try to
** compile again — exactly the cycle described in the subject. Every
** step can end the loop early if the simulation has stopped, so no
** thread ever starts a new phase after that point.
*/
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
