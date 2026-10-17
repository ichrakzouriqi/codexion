/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:47:51 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:35 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


static void	run_simulation(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_thread_run, &sim->coders[i]) != 0)
			exit_with_error("failed to create a coder thread");
		i++;
	}
	if (pthread_create(&sim->monitor, NULL, monitor_thread_run, sim) != 0)
		exit_with_error("failed to create the monitor thread");
	i = 0;
	while (i < sim->params.number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_join(sim->monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_params	params;
	t_sim		sim;

	if (args_validate_all(argc, argv) == 0)
	{
		print_usage_error(argv[0]);
		return (1);
	}
	params_fill_from_args(&params, argv);
	if (sim_init(&sim, &params) == 0)
		exit_with_error("failed to initialize the simulation");
	run_simulation(&sim);
	sim_destroy(&sim);
	return (0);
}
