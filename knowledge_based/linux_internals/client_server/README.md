# Client / Server model

The files in this section implement a basic client / server architecture using different forms of IPC. The API is divided in 3 categories: server API, which defines functions for server creation and accepting new clients; client API, which includes a function to connect to a server, and a third, misc category, that includes methods that are common to the server and client - these are the functions used to send a message, receive a message, and close a connection.

The implementation of the API (and corresponding arguments to each function) may change depending on the underlying IPC structure used. The section below describes the generic API.

As usual in the client/server model, the problem of finding a given server arises. We simply assume that when a server is created, it is given a name, and that the clients somehow know the server's name.

## Server API

As mentioned, the server API consists of 3 functions:

- `new_server()` - Creates a new server.
- `destroy_server()` - Destroys the central point of contact used by new clients to connect to the server.
- `accept_client()` - Blocks until a new client connection arrives.

## Client API
- `connect_to()` - Connects to a server

## Misc
- `recv_info()` - Blocks until a new message arrives in the underlying communication channel.
- `send_info()` - Send a new message to the other end.
- `disconnect_from()` - Disconnects from the other end. User code is responsible for closing both ends of the connection, possibly exchanging application-level protocol specific data so that both ends know when to terminate a connection.

# FIFO implementation

One possible approach is to use FIFOs as communication channels. Creating a server is equivalent to creating a FIFO. This is the central point of contact where clients initially contact the server for new connection requests. It is assumed that the clients know the server's address, which in this case is the FIFO name and path in the filesystem. The client uses this central point of contact to initiate a connection request. On success, a pair of FIFOs is created to facilitate communication between the server and the client. 2 FIFOs are needed because portability requires us to assume that FIFOs are half-duplex. There will be a FIFO for the data to flow from the server to the client, and another for data flowing from the client to the server.

One problem of using a FIFO for connection requests is that clients can send data to the server's central point of contact, but they can't receive a reply, because the data in a FIFO is consumed as soon as it is read, and a client would never know when it was his turn to read from the FIFO. To solve this, the client and the server follow a convention when creating the client/server specific FIFO. A good method to ensure uniqueness (albeit not bullet-proof) is to have the client/server dedicated FIFO name to depend on the client's PID.

So, to initiate a connection request, a client simply opens the well-known central FIFO and writes its PID in there. Then, it creates the pair of FIFOs that will be used to communicate with the server using some convention based on the PID, and the server, upon reading the PID from the FIFO (which is done inside `accept_client()`), opens the pair of FIFOs created by the client. Once both ends open the pair of FIFOs, the client and the server are ready to communicate using a dedicated communication channel.

Note that `open(2)` blocks when opening a FIFO until the other end has opened it too. This means that we must be careful with the order in which we call `open(2)` to avoid deadlocks: if the client opens its read end first and then the write end, then the server must open its write end first (which is the client's read end) to unblock the first `open(2)` in the client, and only then can it proceed to opening the read end (which is the client's write end).

One can wonder if the client really needs to create the pair of FIFOs - why not let the server create them? The problem is that we would have a race condition: consider a scenario where the client writes its PID into the main server's FIFO and then immediately tries to open the dedicated pair of FIFOs, without giving the chance to the server process to create them. Opening would fail in the client. If the client creates the FIFOs before sending the request to the server, this will not happen.

This is the general idea behind the FIFOs implementation. Of course, this solution has some limitations. One of the major drawbacks is that it isn't safe at all: any process is free to open the pair of FIFOs of another process and read from or write to there. Also, nothing stops a malicious process from sending a fake PID of another process to the server's central FIFO, making it very easy to impersonate a process when speaking to the server. There are numerous ways to improve or otherwise make this a little bit safer, but it would (generally) require superuser privileges on the server process.

Another problem is that if the server crashes or suffers any other abnormal termination that causes `destroy_server()` to never be called, the main FIFO is left around in the filesystem. The same can happen with client FIFOs, although a careful implementation avoids this by having the client or the server call `unlink(2)` on the client-specific FIFOs after both ends are opened (this also makes it harder for other processes to snoop in other process's FIFOs, but not impossible - there is a window of time between the server and the client opening the FIFOs and calling `unlink()`, and a malicious process could use this window of time to open the FIFOs). Unlinking clients FIFOs after opening them is a good approach, but this can't be done on the server's main FIFO, since the goal is to have it discoverable by other processes. There is no bullet-proof solution: either we are guaranteed that the server always terminates gracefully by calling `destroy_server()`, or some other mechanism must be used to delete the FIFO.

Last but not least, note that the server's main FIFO shall remain opened for reading and writing, even though the server process won't write to the FIFO. The reason for this is that `read(2)` returns end of file when the FIFO has no active writers; if the client connection request count decreases to 0, the server would have to deal with this special case explicitly. To avoid doing so, we just open the FIFO for reading and writing. POSIX says that opening a FIFO for reading and writing is undefined behavior, but most platforms, including Linux, don't bother with that. For maximum portability, we recommend opening the FIFO twice: first, open for reading with `O_NONBLOCK`. Then, open for writing. `O_NONBLOCK` is needed because we don't want to block when opening for reading (because we are about to be a writer!). We need to open for reading first because opening for writing with `O_NONBLOCK` will fail with `ENXIO`. In either case, we can't forget to disable nonblocking mode in the file descriptors after opening both channels. 

# UNIX Sockets

The code in `unix_sockets` implements the same interface using UNIX domain sockets (`AF_UNIX`). The server creates an `AF_UNIX` socket and binds a known address to it. In UNIX domain sockets, addresses are mapped into filenames in the filesystem that the clients can use to find the socket to connect to.

The advantage of using UNIX domain sockets is that most of the interface is already implemented by the filesystem: we don't explicitly have to set up a channel for each communication flow direction, because sockets are inherently bidirectional.

Clients connecting to the server bind their local socket to an address that corresponds to their PID; this is also mapped into a filename in the filesystem that the server can (rudimentarily) use to make some validations:

* It checks that the file is recent (has been created within the last 10 seconds), because a malicious process could use an old file created by the victim some time ago
* It checks that the file is indeed a socket file
* It checks that the only permissions in the file are user read, write and execute

Of course, there is still a time window where an attack would be successful, but hey, it's better than nothing. The interface described in *Advanced Programming in the UNIX Environment* also suggests returning the effective user ID of the owner of the client socket file to usercode, but we don't think this is of much use - there is no portable way to get the effective user ID of a process given its PID, and even if there was, that information could be invalid by the time we use it to check for the correct permissions (this is known as a TOCTTOU error, Time Of Check To Time Of Use). So, returning the effective user id of the socket owner could only provide a false sense of security, not to mention that it would make the interface uglier without added benefit.

Once connection is established, the server and the clients can communicate in pretty much the same way they would using network sockets, or any other type of sockets.

We have to worry about cleaning up the filesystem. The filename created by the client socket (the one created as a side effect of having the client binding its local socket to an address) is deleted by the server once the necessary permission checks have taken place. Note that only the server can do this - having the client deleting the file would create a race condition on the server side, since the client could delete it before the server had the chance to run the permission checks.

The main server socket used by client processes to rendezvous with the server is problematic. There is no reliable and effective way to clean up the filesystem. We never know when new clients will be arriving, so the socket must be kept open and public as long as the server is running. We could add additional data structures and information to keep track of the server socket's filename to unlink it inside `destroy_server()`, but that would only complicate matters, and if the server terminates abnormally, the file would still be laying around.

Instead, we opt by deleting any remaining old files in `new_server()` by calling `unlink(2)` on the address used before binding it again. We ignore any errors reported by `unlink(2)`, because it might actually be the case that no file with that name exists. On the other hand, a file may exist that we're not allowed to remove (for example, the file is owned by someone else and the directory where it lives has the sticky bit). Another problem with this approach is that we only remove old filenames if we're about to use them again - other filenames from other servers will not be removed.

In reality, though, none of this matters because the filenames used by the clients to rendezvous with the server are kept in `/tmp`, so they will eventually be deleted once the system reboots.
