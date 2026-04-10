RESULTS

- Implemented cooperative user-level threads (`thread_create`, `thread_yield`, `thread_join`) and a simple mutex (`mutex_init`, `mutex_lock`, `mutex_unlock`).
- Added a user-mode context switch (`xv6/user_switch.S`) and per-thread stacks.
- Verified with `xv6/test_pc.c` (2 producers, 1 consumer) which prints consumer progress and ends with `test_pc: done`.


Running the program:
1. Make & Make clean

Run the "make clean" command and then the "make" command. 


2. Run and execute the test:

Then do "make qemu" command

type: test_pc  at the xv6 shell prompt:


Limitations:

- Scheduling is cooperative. Meaning hreads must call `thread_yield()` (or block in `mutex_lock`) to allow others to run.
- `thread_join` waits by yielding until the target thread finishes. This can waste the CPU.

- Limited call depth
- finished threads are not freed

