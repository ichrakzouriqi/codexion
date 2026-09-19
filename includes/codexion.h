/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: izouriqi <izouriqi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 19:49:28 by izouriqi          #+#    #+#             */
/*   Updated: 2026/09/19 19:49:31 by izouriqi         ###   ########.fr       */
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

/*
** CDX_SCHED_FIFO / CDX_SCHED_EDF: the two arbitration policies a dongle can use
** to decide who gets it next when several coders are waiting for it.
**
** MAX_ARG_DIGITS: a numeric argument longer than this is rejected before
** it ever reaches atoi(). atoi() has no way to report overflow, so this
** length cap is what keeps every accepted value safely inside int range
** (10^9 already covers any realistic coder count or millisecond delay).
**
** MONITOR_TICK_US: how often (in microseconds) the monitor thread wakes
** up to check for burnout / completion. 1000us = 1ms, ten times finer
** than the 10ms precision the subject requires, with room to spare for
** scheduling jitter.
*/
# define CDX_SCHED_FIFO 0
# define CDX_SCHED_EDF 1
# define MAX_ARG_DIGITS 9
# define MONITOR_TICK_US 1000

/*
** t_wait_entry: one coder's ticket while it waits for a specific dongle.
** primary_key / secondary_key are plain comparison keys: whichever entry
** has the smallest primary_key is served first; secondary_key only
** breaks ties. What the keys actually mean (arrival order vs. deadline)
** is decided in scheduler.c, so the heap itself never needs to know
** which scheduling policy is active.
*/
typedef struct s_wait_entry
{
	int				coder_id;
	long			primary_key;
	long			secondary_key;
}					t_wait_entry;

/*
** t_heap: a fixed-capacity binary min-heap of t_wait_entry. "entries" is
** a single pre-allocated array (capacity = number_of_coders, the most
** waiters a single dongle could ever have), so no resizing is needed.
*/
typedef struct s_heap
{
	t_wait_entry	*entries;
	int				size;
	int				capacity;
}					t_heap;

/*
** t_dongle: one shared resource. "taken" and "available_at_ms" record its
** current state, "lock"/"cond" protect and signal that state, and
** "waiters" is the fair-arbitration queue described above.
*/
typedef struct s_dongle
{
	int				taken;
	long			available_at_ms;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	t_heap			waiters;
}					t_dongle;

/* t_params: the 8 command-line arguments, parsed and validated. */
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

/*
** t_coder: one coder's identity, seating position and progress.
** left_dongle / right_dongle are indexes into sim->dongles.
** last_compile_start_ms and compiles_done are shared state: only ever
** read or written while holding sim->state_lock (never a per-coder
** lock, to keep the locking scheme small and easy to reason about).
*/
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

/*
** t_sim: the whole simulation. Everything mutable that more than one
** thread can touch lives in here, each field guarded by exactly one of
** the mutexes below:
**   - seq_lock   : seq_counter        (fifo arrival-order ticket source)
**   - log_lock   : serializes every printed line
**   - state_lock : every coder's last_compile_start_ms / compiles_done
**   - stop_lock + stop_cond : the "stopped" flag and waking up sleepers
** dongles/coders are allocated once before any thread starts and never
** resized, so the arrays themselves need no lock, only the fields above.
** There are no global variables anywhere in this project: every function
** that needs simulation state receives a pointer to it as an argument.
*/
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
