# Asynchronous I/O with libevent

## Synchronous I/O

**Source:** https://libevent.org/libevent-book

An IO call is synchronous if, when you call it, it does not return until the operation is completed, or until time out. For example, When you call `connect()` on a TCP connection, your operating system `queues a SYN packet to the host on the other side` of the TCP connection. It does not return control back to your application until either it has received a SYN ACK packet from the opposite host, or until connect timeout.

`connect()` does not return until it has connected; the `recv()` calls do not return until they have received data or a close; and the `send()` call does not return until it has at least flushed its output to the kernel’s write buffers.

**So where is the problem?**

In synchronous IO, if you want to read and write data from second connection, you can't do it untill you read it from first connection because Server is not concurrent.

**So what's the solution?**

Multithreading / multiprocessing can be one way to fix this issue but multithreading (thread pool) will not scale well and multiprocessing is expensive in nature.

The oldest solution that people still use for this problem is select(). The `select()` call takes three sets of fds (implemented as bit arrays): one for reading, one for writing, and one for "exceptions". It waits until a socket from one of the sets is ready and alters the sets to contain only the sockets ready for use. Unfortunately, generating and reading the select() bit arrays takes time proportional to the largest fd that you provided for select(), the select() call scales terribly when the number of sockets are high.

## Asynchronous I/O

Libevent is a library for writing fast portable nonblocking IO. 

### Events in libevent
Libevent’s basic unit of operation is the event. Every event represents a set of conditions - 
* A file descriptor being ready to read from or write to.
* A timeout expiring.
* A user-triggered event.
* A signal occurring.



### event_base()
Structure to hold information and state for a Libevent dispatch loop. It keeps track of all pending and active events, and notifies your application of the active ones. This is an opaque structure; you can allocate one using `event_base_new()` or `event_base_new_with_config()`.

More details: https://libevent.org/doc/structevent__base.html#details

### event_base_loop()

Once you have an event_base with some events registered, you will want Libevent to wait for events and alert you about them. The `event_base_loop()` function runs an event_base until there are no more events registered in it.

To run the loop, it repeatedly checks whether any of the registered events has triggered (for example, if a read event’s file descriptor is ready to read, or if a timeout event’s timeout is ready to expire). Once this happens, it marks all triggered events as "active", and starts to run them.

If `EVLOOP_ONCE` flag in event_base_loop() is set, then the loop will wait until some events become active, then run active events until there are no more to run, then return. In case of `EVLOOP_NONBLOCK`, loop will not wait for events to trigger: it will only check whether any events are ready to trigger immediately, and run their callbacks if so.

When it is done, event_base_loop() returns 0 if it exited normally, -1 if it exited because of some unhandled error in the backend, and 1 if it exited because there were no more pending or active events.

PseudoCode:

``` CPP
while (any events are registered with the loop,
        or EVLOOP_NO_EXIT_ON_EMPTY was set) {

    if (EVLOOP_NONBLOCK was set, or any events are already active)
        If any registered events have triggered, mark them active.
    else
        Wait until at least one event has triggered, and mark it active.

    for (p = 0; p < n_priorities; ++p) {
       if (any event with priority of p is active) {
          Run all active events with priority of p.
          break; /* Do not run any events of a less important priority */
       }
    }

    if (EVLOOP_ONCE was set or EVLOOP_NONBLOCK was set)
       break;
}
```
