/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 15:29:59 by izouriqi          #+#    #+#             */
/*   Updated: 2026/10/17 15:48:20 by izouriqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>


# define CDX_SCHED_FIFO 0
# define CDX_SCHED_EDF 1
# define MAX_ARG_DIGITS 9
# define MONITOR_TICK_US 1000


typedef struct s_wait_entry
{
	int				coder_id;
	long			primary_key;
	long			secondary_key;
}					t_wait_entry;


typedef struct s_heap
{
	t_wait_entry	*entries;
	int				size;
	int				capacity;
}					t_heap;


typedef struct s_dongle
{
	int				taken;
	long			available_at_ms;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	t_heap			waiters;
}					t_dongle;


typedef struct s_params
{
	int				number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				number_of_compiles_required;
	long			dongle_cooldown;
	int				scheduler_mode;
}					t_params;

typedef struct s_sim	t_sim;


typedef struct s_coder
{
	int				id;
	int				left_dongle;
	int				right_dongle;
	long			last_compile_start_ms;
	int				compiles_done;
	pthread_t		thread;
	t_sim			*sim;
}					t_coder;


struct s_sim
{
	t_params		params;
	long			start_time_ms;
	long			seq_counter;
	int				stopped;
	pthread_mutex_t	seq_lock;
	pthread_mutex_t	log_lock;
	pthread_mutex_t	state_lock;
	pthread_mutex_t	stop_lock;
	pthread_cond_t	stop_cond;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_t		monitor;
};

/* args_check.c */
int					args_count_is_valid(int argc);
int					arg_is_valid_number(const char *s);
int					arg_is_valid_scheduler(const char *s);
int					args_validate_all(int argc, char **argv);

/* args_parse.c */
long				parse_positive_long(const char *s);
int					parse_scheduler_mode(const char *s);
void				params_fill_from_args(t_params *params, char **argv);

/* error_utils.c */
void				print_usage_error(const char *prog_name);
void				exit_with_error(const char *msg);

/* time_utils.c */
long				time_now_ms(void);
void				ms_to_timespec(long ms, struct timespec *ts);
int					sleep_interruptible(t_sim *sim, long duration_ms);

/* sim_stop.c */
int					simulation_is_stopped(t_sim *sim);
void				simulation_stop(t_sim *sim);

/* logger.c */
void				log_event(t_sim *sim, int coder_id, const char *action);

/* heap.c */
void				heap_init(t_heap *heap, t_wait_entry *storage, int cap);
int					heap_is_empty(t_heap *heap);
void				heap_push(t_heap *heap, t_wait_entry entry);
t_wait_entry		heap_pop_min(t_heap *heap);
int					heap_top_is(t_heap *heap, int coder_id);
void				heap_sift_up(t_heap *heap, int index);
void				heap_sift_down(t_heap *heap, int index);

/* heap_remove.c */
void				heap_remove_coder(t_heap *heap, int coder_id);

/* scheduler.c */
t_wait_entry		wait_entry_make(t_coder *coder);
int					wait_entry_before(t_wait_entry *a, t_wait_entry *b);

/* dongle_acquire.c */
int					dongle_acquire(t_dongle *dongle, t_coder *coder);

/* dongle_release.c */
void				dongle_release(t_dongle *dongle, t_sim *sim);

/* coder_dongles.c */
int					coder_acquire_both_dongles(t_coder *coder);
void				coder_release_both_dongles(t_coder *coder);

/* coder_cycle.c */
int					coder_do_compile(t_coder *coder);
int					coder_do_debug(t_coder *coder);
int					coder_do_refactor(t_coder *coder);

/* coder_thread.c */
void				*coder_thread_run(void *arg);

/* monitor_thread.c */
void				*monitor_thread_run(void *arg);

/* init_dongles.c */
int					dongles_init(t_sim *sim);

/* init_coders.c */
void				coders_init(t_sim *sim);

/* init_simulation.c */
int					sim_init(t_sim *sim, t_params *params);

/* free_simulation.c */
void				sim_destroy(t_sim *sim);

#endif
