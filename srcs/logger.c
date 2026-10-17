/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:47:41 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


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
