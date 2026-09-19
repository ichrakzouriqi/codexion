*This project has been created as part of the 42 curriculum by izouriqi.*

## Description

Codexion is a multithreaded simulation built around the same resource-sharing
problem the classic "Dining Philosophers" exercise is famous for, reskinned as
a co-working space: `number_of_coders` coders sit in a circle and take turns
**compiling**, **debugging**, and **refactoring**. Compiling is the only phase
that needs a shared resource — a **dongle** — and it needs two of them at
once, one from each neighbor. There are exactly as many dongles as coders, one
between every adjacent pair, so getting both dongles unavoidably means
competing with two different neighbors at once.

A coder who goes too long without starting a new compile **burns out**, which
immediately ends the whole simulation. The program has to hand out dongles
fairly (either strictly first-come-first-served, or by prioritizing whoever is
closest to burning out), enforce a cooldown period after every dongle is
released, and detect a burnout within 10ms of it actually happening — all
without a deadlock, a data race, or a memory leak.

## Instructions

### Compilation

```sh
make
```

This produces a `codexion` binary at the project root. `make clean` removes
object files, `make fclean` also removes the binary, and `make re` rebuilds
from scratch. The Makefile compiles every source file with exactly
`-Wall -Wextra -Werror -pthread`, using `cc`.

### Usage

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown \
           scheduler
```

All 8 arguments are mandatory. The first, second, third, fourth, fifth,
sixth and seventh must be non-negative integers (`number_of_coders` must
additionally be at least 1 — a circle of zero coders isn't a meaningful
simulation); `scheduler` must be exactly `fifo` or `edf`. Any other input is
rejected with a usage message and exit code 1.

Example — 5 coders, generous timing, first-come-first-served dongle access:

```sh
./codexion 5 3000 150 100 100 4 100 fifo
```

Example — same idea, but dongles go to whoever is closest to burning out:

```sh
./codexion 5 3000 150 100 100 4 100 edf
```

Every state change is printed as `<timestamp_ms> <coder_id> <event>`, exactly
as required: `has taken a dongle`, `is compiling`, `is debugging`,
`is refactoring`, or `burned out`. The simulation ends the moment a coder
burns out, or once every coder has compiled at least
`number_of_compiles_required` times — whichever comes first.

## Resources

- *The Little Book of Semaphores*, ch. 4 ("The Dining Philosophers") — the
  classic reference for this family of resource-sharing problems and for the
  "acquire resources in a fixed global order" deadlock-avoidance strategy
  used here.
- The POSIX `pthread_mutex`, `pthread_cond`, and `gettimeofday` manual pages
  (`man 3 pthread_cond_timedwait`, etc.) for the exact semantics of timed
  waits and broadcast/signal.

**How AI was used:** this implementation (the full architecture, every
source file, and this README) was produced with substantial assistance from
an AI assistant (Claude), at the requester's explicit request, rather than
written from scratch by hand. Concretely, the AI proposed the fixed
lower-index-first dongle acquisition order to avoid deadlock, the
condition-variable-broadcast-on-every-state-change pattern used throughout,
and the single generic heap shared by both scheduling policies; it also
wrote and iteratively debugged all of the C code, ran it under
AddressSanitizer/UBSan and ThreadSanitizer, and checked it against the real
`norminette` tool.

This matters for how you use this project: the subject's own "AI
Instructions" chapter is explicit that you must be able to explain and take
responsibility for any AI-assisted code, and the evaluation sheet has a
dedicated failing flag for exactly this ("Can't support / explain"). Please
budget real time to read through every file, run the binary yourself under
a range of parameters, and be ready to walk through *why* each
synchronization decision below is correct — not just *that* it is. The
"Blocking cases handled" and "Thread synchronization mechanisms" sections
below are written to help you do that, not to replace it.

## Blocking cases handled

- **Deadlock prevention (Coffman's circular-wait condition).** Every coder
  acquires its two dongles in a fixed global order — the lower array index
  first, then the higher one — regardless of which is its "left" or "right"
  dongle (see `lower_dongle()` / `higher_dongle()` in `coder_dongles.c`).
  Because *every* coder obeys the same global order, the "each holds one and
  waits for the next one in a cycle" pattern that causes deadlock in the
  naive version of this problem can never form: a coder can only ever be
  blocked waiting on a dongle held by someone who is not, in turn, waiting on
  a lower-indexed dongle that this coder holds. This is the standard,
  textbook-correct resolution to this exact class of problem.
- **The single-coder edge case.** With `number_of_coders == 1` there is only
  one physical dongle (per the subject), so a lone coder's left and right
  dongle are the same object. It can acquire that one dongle once, but a
  second, simultaneous acquisition of the same object is — correctly —
  never possible (there is no second dongle, and no one else exists to
  release the first one). No special-case code is needed for this: the
  coder simply blocks, and the monitor thread's ordinary burnout detection
  ends the simulation for it, exactly as it should for this legitimately
  infeasible configuration.
- **Starvation prevention / fair arbitration.** Every dongle keeps a small
  min-heap of the coders currently waiting for it (`t_heap` in `heap.c` /
  `heap_sift.c` / `heap_remove.c`), ordered by whichever key the active
  scheduler uses (`scheduler.c`): arrival order under `fifo`, or burnout
  deadline under `edf`, with a coder-id tie-break for `edf` (see the "recode
  note" in `scheduler.c` — flipping one comparison there is enough to make a
  *higher* coder id win ties instead of a lower one). Whoever is at the top
  of the heap is served next, so no waiter can be skipped over indefinitely
  by later arrivals.
  One honest caveat worth understanding, not just accepting: `edf` can only
  arbitrate between coders who are *actively waiting* for the *same free*
  dongle. It cannot shorten how long a dongle stays legitimately held by a
  coder who is, itself, still waiting on its *other* dongle. In a small ring
  (e.g. 3 coders sharing 3 dongles, each needing 2), that topology only ever
  allows one coder to compile at a time, so with a `time_to_burnout` set
  close to the physical minimum round-trip time, a burnout can still occur
  under `edf` — this is the parameters being genuinely infeasible for that
  topology, not a scheduling defect. With realistic slack (see the usage
  examples above), every coder reaches its compile quota under both
  schedulers.
- **Dongle cooldown.** `dongle_release()` stamps the dongle with
  `now + dongle_cooldown` the moment it is freed; `dongle_ready_for()` in
  `dongle_acquire.c` refuses to grant it to anyone until that time has
  passed, and a waiter whose only obstacle is a still-running cooldown uses
  a *timed* wait bounded to exactly that moment (`dongle_wait()`), so a free,
  off-cooldown dongle is never left idle for want of a wake-up.
- **Precise burnout detection.** A dedicated monitor thread
  (`monitor_thread.c`) polls every coder's deadline
  (`last_compile_start_ms + time_to_burnout`) every 1ms — ten times finer
  than the required 10ms precision — and immediately logs `burned out` and
  stops the whole simulation the moment any deadline is exceeded. Measured
  in practice, detection lag is consistently ~1ms.
- **Log serialization.** Every log line is assembled into a single buffer
  and emitted with one `write()` call while holding `sim->log_lock`
  (`logger.c`), so two coders' lines can never interleave into a corrupted
  line, and output reaches the terminal immediately rather than sitting in
  libc's stdio buffer.
- **Prompt, clean shutdown.** The instant the simulation stops (burnout, or
  quota reached), every dongle's condition variable and the shared
  `stop_cond` are broadcast on (`simulation_stop()` in `sim_stop.c`), waking
  every thread that might be blocked — whether waiting for a dongle or
  mid-way through a debug/refactor sleep — so no thread is ever left
  sleeping out a full phase duration after the simulation has already ended.

## Thread synchronization mechanisms

Every mutable piece of state shared between threads is guarded by exactly
one lock, and no function ever holds two different locks at once — which is
what makes the whole design straightforward to reason about (no lock
ordering to get wrong, because there is no nesting to order):

| Lock | Protects |
|---|---|
| `sim->log_lock` | Serializes every printed line (`logger.c`) |
| `sim->seq_lock` | `seq_counter`, the fifo arrival-ticket source (`scheduler.c`) |
| `sim->state_lock` | Every coder's `last_compile_start_ms` and `compiles_done` |
| `sim->stop_lock` + `sim->stop_cond` | The `stopped` flag, and waking up any sleeping thread |
| `dongle->lock` + `dongle->cond` | That one dongle's `taken`/`available_at_ms`/wait-heap |

The recurring pattern used everywhere a thread needs to wait for something is
the standard **predicate + condition variable** idiom: hold the lock, check
whether the condition already holds, and if not, `pthread_cond_wait` (or
`pthread_cond_timedwait`) and re-check in a loop once woken — never assume a
single wake-up means the condition is now true. Every place that changes a
piece of state another thread might be waiting on (a dongle being granted,
released, or newly queued for; the simulation stopping) broadcasts on the
relevant condition variable right after making the change, while still
holding that same lock. That combination — same lock guards both the data
and the decision to wait, and every state change is paired with a broadcast —
is what rules out both lost wake-ups and torn reads, without needing any
lower-level atomics.

Two examples worth being able to walk through:

- **Coder ↔ dongle.** `dongle_acquire()` pushes the coder's ticket onto the
  dongle's heap and loops: while not granted and the simulation hasn't
  stopped, it checks `dongle_ready_for()` (is it free, off cooldown, and at
  the top of the heap?) and either takes it (and broadcasts, so the *next*
  waiter gets a chance to re-check) or sleeps on `dongle->cond`. Because the
  check and the sleep both happen while holding `dongle->lock`, there is no
  window in which the dongle could change state between "I checked" and "I
  went to sleep".
- **Coder ↔ monitor.** The monitor never messages a coder directly; it only
  reads each coder's `last_compile_start_ms`/`compiles_done` under
  `state_lock`, and a coder only ever writes those same two fields under the
  same lock (`coder_mark_compile_start()` / `coder_mark_compile_done()` in
  `coder_cycle.c`). Neither thread needs to know the other is running: the
  lock alone is the entire contract between them. When the monitor decides
  to stop the simulation, it does not wake threads one at a time — it flips
  `stopped` under `stop_lock` and broadcasts both `stop_cond` and every
  single dongle's own condition variable, so every thread notices on its own,
  however it happens to be waiting at that moment.

There are no global variables anywhere in this project (checked explicitly,
since the evaluation sheet stops grading entirely if it finds one managing
the dongles, the scheduler, or the logging): every function that touches
simulation state receives a pointer to it — `t_sim *`, `t_coder *`, or
`t_dongle *` — as an argument.

### Verification performed while building this

- Compiles with zero warnings under the required
  `-Wall -Wextra -Werror -pthread`.
- Every file passes the real `norminette` tool (v3.3.60).
- AddressSanitizer + UndefinedBehaviorSanitizer: no errors, no leaks, on
  both the normal-completion path and the burnout path.
- ThreadSanitizer: no data races detected across normal completion,
  burnout, the single-coder edge case, and stress runs up to 150 coders
  under both schedulers.
- Burnout-to-log latency measured at ~1ms across repeated runs (required:
  ≤10ms).
- Argument validation exercised against missing arguments, negative
  numbers, non-integers, an invalid scheduler name, `number_of_coders = 0`,
  and an overlong numeric string (overflow guard).

None of this replaces your own testing and understanding before your
defense — it's the starting point, not a substitute for it.