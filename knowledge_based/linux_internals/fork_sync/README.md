# Syncing forked processes

This section implements 3 primitives to sync a parent with any of its children, and vice versa, using different methods.

It is based on the ideas exposed in *Advanced Programming in the UNIX Environment*, 3rd ed. The book presents a couple of different ways on how to implement a set of functions that can be used as a synchronization mechanism between a parent process and a child process. These primitives are:

- `TELL_WAIT(void)`: an initializer that sets things up and does some house keeping. This must be called if a parent process plans to synchronize with a child process in the future. It must be called before forking.

- `TELL_PARENT(void)`: called by a child process to notify the parent process, possibly unblocking the parent process out of `WAIT_CHILD()`.

- `TELL_CHILD(pid_t p)`: called by the parent process to notify the child process with pid `p`, possibly unblocking the child process out of `WAIT_PARENT()`.

- `WAIT_PARENT(void)`: called by a child process to wait for a notification from the parent process. This may cause the child process to sleep (depending on the implementation). This will only return after the parent calls `TELL_CHILD()` with this child's process ID, or if the parent called `TELL_CHILD()` before the corresponding child called `WAIT_PARENT()`.

- `WAIT_CHILD(void)` - called by a parent process to wait for a notification from any of its children. This may cause the parent process to sleep (depending on the implementation). This will only return after a child calls `TELL_PARENT()`, or if a child called `TELL_PARENT()` before the parent called `WAIT_CHILD()`.

Here, however, we implement a slightly different (and more generic) approach. The API implemented here consists of the following 3 primitives:

- `NOTIFY_INIT(void)` - an initializer that sets things up and does some house keeping. This must be called if a parent process plans to synchronize with a child process in the future. It must be caled before forking.

- `NOTIFY(pid_t p)` - notifies the process with PID `p`, possibly unblocking it out of `WAIT_NOTIFY()`.

- `WAIT_NOTIFY(void)` - wait for a notification to be sent to this process. A notification is seen as a sign that it is now safe to move on. This may cause the caller to sleep (depending on the implementation). This will only return after another process (typically, the parent) calls `NOTIFY()` with this process's ID.

`TELL_WAIT()`, `TELL_PARENT()`, `TELL_CHILD()`, `WAIT_PARENT()` and `WAIT_CHILD()` are easily implemented on top of `NOTIFY_INIT()`, `NOTIFY()` and `WAIT_NOTIFY()`.

## Method 1: signals

One possible approach to do this is using signals. We use one of the user-reserved signals, `SIGUSR1`, to serve as the notification mechanism. First, we block `SIGUSR1` in `NOTIFY_INIT()`. This prevents both the parent and the child from receiving `SIGUSR1` until either the signal is ignored or unblocked. So, `NOTIFY()` merely sends a signal to the target process with `kill(2)`. Meanwhile, `WAIT_NOTIFY()` calls `sigsuspend(2)` with the process's original signal mask (but without `SIGUSR1`), so that it atomically unblocks `SIGUSR1` and is put to sleep until a signal is delivered to the process. However, once `sigsuspend()` returns, we are not guaranteed that this was due to a `SIGUSR1`. To solve this, we have a global flag variable that is set to 1 in the signal handler for `SIGUSR1`, so that we can test it inside `WAIT_NOTIFY()` each time `sigsuspend()` returns.

So, in short: `NOTIFY_INIT()` initializes the flag to 0, sets up a custom signal handler for `SIGUSR1`, and blocks `SIGUSR1`. We have to be careful not to change the other signals disposition (this is achieved by passing `SIG_BLOCK` to `sigprocmask(2)` rather than `SIG_SETMASK`). The signal handler for `SIGUSR1` sets the flag variable to 1. `NOTIFY()` raises the signal in the target process with `kill(2)`, and `WAIT_NOTIFY()` calls `sigsuspend()` with a mask that unblocks `SIGUSR1`, checks the flag variable upon return from `sigsuspend()`, and either returns (if the flag is 1), or calls `sigsuspend()` again (if the flag is 0, which means that the process received another signal that is not related to the synchronization mechanism).

The drawbacks of this approach are:

- The synchronization mechanisms eat up one of the user signals, and this implementation detail is "leaked" to the rest of the code. That is, a program must know that it can't use `SIGUSR1` because it is being used by the notify routines.

- There may be subtle problems related to how the system handles blocked signals. Most systems do not queue pending signals: if a signal is blocked and it is raised more than once for a specific process, when the process unblocks the signal, it is only delivered once (in most systems). This means that we can have one process issue a bunch of `NOTIFY()`s that all end up being consumed by a single `WAIT_NOTIFY()`, which can even lead to a deadlock on the waiting process if that process assumes that one `NOTIFY()` matches with exactly one (and only one) `WAIT_NOTIFY()`. However, as long as a process invokes `WAIT_NOTIFY()` between a pair of `NOTIFY()`, this isn't a problem.

- A process that receives a lot of signals other than `SIGUSR1` will end up wasting CPU resources testing the flag variable each time a new signal arrives.

## Method 2: record locks

This method makes use of record locking: the initializer function creates a temporary unique file (with `mkstemp(3)`) that is used by the notifying routines. The idea is that the process sending the notification writes the target process's ID into the file, and the receiving process reads the file, compares the contents to its PID, and returns from `WAIT_NOTIFY()` if they match.

So, `NOTIFY()` locks the file, writes the target PID into the file, and unlocks it. `WAIT_NOTIFY()` locks the file, reads its contents, unlocks it, and checks if the contents match its PID. If there is a match, `WAIT_NOTIFY()` returns, otherwise, we repeat the same steps again.

The main problem of this approach is obvious: polling. The waiting processes waste CPU time by repeatedly reading the file contents, which is clearly suboptimal, but it is the only way to do it with record locks (recall that locks are not inherited by a child process, so any lock that the initializer function acquires will stay with the parent only).

## Method 3: pipes

A parent and its child can be synchronized with a pair of half-duplex pipes: one pipe is used to flow messages from the parent to the child, and another is used to flow messages from the child to the parent. Thus, the initializer function creates a pair of pipes, and then notifying consists of writing something into the pipe, whereas waiting is as simple as issuing a blocking read on the pipe. The main advantage of this approach is that it is extremely easy and fast to implement, but there is a major drawback: we can't implement the generic notify API; this method is only usable with the more constrained API showed in the book. Furthermore, it only works for a parent with a single child. This stems from the fact that pipes are different from regular files in that there isn't the concept of a file offset; once we read the data from the pipe, it's gone. If multiple children are blocked in a read from the same pipe, only one will succeed, and there is no way to guarantee that this is the correct child that the parent wanted to notify in the first place.

For these reasons, the pipe implementation, although perhaps the easiest to understand and implement, places some severe constraints on the API design and usage.
