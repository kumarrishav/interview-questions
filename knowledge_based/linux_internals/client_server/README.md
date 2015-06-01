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

So, to initiate a connection request, a client simply opens the well-known central FIFO and writes its PID in there. Then, it creates the pair of FIFOs that will be used to communicate with the server using some convention based on the PID, and the server, upon reading the PID from the FIFO (which is done inside `accept_client()`, opens the pair of FIFOs created by the client. Once both ends open the pair of FIFOs, the client and the server are ready to communicate using a dedicated communication channel.

Note that `open(2)` blocks when opening a FIFO until the other end has opened it too. This means that we must be careful with the order in which we call `open()` to avoid deadlocks: if the client opens the read end first and then the write end, then the server must open the write end first (to unblock the first `open()` in the client) and then the read end.

One can wonder if the client really needs to create the pair of FIFOs - why not let the server create them? The problem is that we would have a race condition: consider a scenario where the client writes its PID into the main server's FIFO and then immediately tries to open the dedicated pair of FIFOs, without giving the chance to the server process of creating them. Opening would fail in the client. If the client creates the FIFOs before sending the request to the server, this will not happen.

This is the general idea behind the FIFOs implementation. Of course, this solution has some limitations. One of the major drawbacks is that it isn't safe at all: any process is free to open the pair of FIFOs of another process and read from or write to there. Also, nothing stops a malicious process from sending a fake PID of another process to the server's central FIFO, making it very easy to impersonate a process when speaking to the server. There are numerous ways to improve or otherwise make this a little bit safer, but it would (generally) require superuser privileges on the server process.

Another problem is that if the server crashes or suffers any other abnormal termination that causes `destroy_server()` to never be called, the main FIFO is left around in the filesystem. The same can happen with client FIFOs, although a careful implementation avoids this by having the client or the server call `unlink(2)` on the client-specific FIFOs after both ends are opened (this also makes it harder for other processes to snoop in other process's FIFOs, but not impossible - there is a window of time between both the server and the client open the FIFOs and call `unlink()`, and a malicious process could use this window of time to open the FIFOs). Unlinking clients FIFOs are opening them is a good approach, but this can't be done on the server's main FIFO, since the goal is to have it discoverable by other processes. There is no bullet-proof solution: either we are guaranteed that the server always terminates gracefully by calling `destroy_server()`, or some other mechanism must be used to delete the FIFO.
