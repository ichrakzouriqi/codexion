/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:47:41 by izouriqi          #+#    #+#             */
/*   Updated: 2026/09/19 19:47:46 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Converts a non-negative long into decimal digits written at buf,
** returning how many characters were written. Hand-rolled because this
** project's allowed function list has no snprintf/sprintf to do it for
** us, and because write() (used below) needs a plain byte buffer rather
** than something only printf/fprintf know how to fill.
*/
static int	long_to_str(long value, char *buf)
{
	char	tmp[32];
	int		len;
	int		i;

	if (value == 0)
	{
		buf[0] = '0';
		return (1);
	}
	len = 0;
	while (value > 0)
	{
		tmp[len] = (char)('0' + (value % 10));
		value = value / 10;
		len++;
	}
	i = 0;
	while (i < len)
	{
		buf[i] = tmp[len - 1 - i];
		i++;
	}
	return (len);
}

static int	copy_str(const char *src, char *dst)
{
	int	i;

	i = 0;
	while (src[i] != '\0')
	{
		dst[i] = src[i];
		i++;
	}
	return (i);
}

/*
** Prints one log line in the exact required format:
**   "<timestamp_in_ms> <coder_id> <action>\n"
** The timestamp is captured before the log lock is acquired, so that
** waiting for the lock never distorts the reported time. write() is
** used instead of printf/fprintf so the line reaches the terminal (or
** the file it is redirected to) immediately, with no libc stdio
** buffering delay that could threaten the burnout log's 10ms precision
** requirement. The lock guarantees the whole line is written as a
** single, uninterrupted write() call, so two coders' lines can never
** interleave on screen.
*/
void	log_event(t_sim *sim, int coder_id, const char *action)
{
	char	buf[128];
	int		pos;
	long	elapsed;

	elapsed = time_now_ms() - sim->start_time_ms;
	pthread_mutex_lock(&sim->log_lock);
	pos = long_to_str(elapsed, buf);
	buf[pos++] = ' ';
	pos += long_to_str((long)coder_id, buf + pos);
	buf[pos++] = ' ';
	pos += copy_str(action, buf + pos);
	buf[pos++] = '\n';
	write(1, buf, pos);
	pthread_mutex_unlock(&sim->log_lock);
}
