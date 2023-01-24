# unix-socket-programming

## What is a socket?

### Port
Port is number and unique identifier that specific application on your computer that’s attempting to access the network. This application can be both client or server.

### Socket
Socket is simply combination of an IP address and port. It acts as an two-way communication link for receiving or sending information over the network.

_"A socket is bound to a port number so that the TCP layer can identify the application that data is destined to be sent to."_

## Different type of sockets?
A `ServerSocket` is used to `listen for incoming client connections`, and is `used to create a new Socket for each client that connects to it`.

Here are some key differences between `Sockets` and `ServerSockets`:

* A Socket is typically used by a client to connect to a server, while a ServerSocket is used by a server to listen for and accept incoming client connections.

* A Socket is created when a connection is established between a client and a server, while a ServerSocket is created at the beginning of the communication, and is used to listen for and accept incoming connections.

* Both Sockets and ServerSockets can be used to send and receive data, but the data transfer typically occurs over a Socket, while a ServerSocket is mainly used to establish connections and create Sockets.

* A Socket is created when a connection is established and is closed when the connection is terminated. A ServerSocket is created at the start of the communication and remains open until it is explicitly closed or the program ends.

Overall, Sockets and ServerSockets are used for different purposes in network communication. Sockets are used to establish a connection and transfer data between two devices, while ServerSockets are used to listen for and accept incoming connections from clients.

## How socket works?

1. `socket()` API creates an endpoint for communications and returns a socket descriptor that represents the endpoint.

2. `bind()` API associates the socket with local address i.e. IP Address, port and address family.

3. `listen()` indicates a readiness to accept client connection requests. It transforms an `active socket` into a `passive socket`, means socket can never be used as an active socket to initiate connection requests.

<p align="center">
  <img src="https://www.ibm.com/docs/en/ssw_ibm_i_73/gifs/rxab6500.gif" />
</p>

4. Normally, the server process is put to sleep in the call to `accept()`, waiting for a client connection to arrive and be accepted. A TCP connection uses what is called a three-way handshake to establish a connection. When this handshake completes, accept returns, and the return value from the function is a new descriptor that is called the connected descriptor. This new descriptor is used for communication with the new client. A new descriptor is returned by `accept()` for each client that connects to our server.

4. Once the client is connected to the server, a new socket is created for communication with the client. On the other hand, a client creates a socket and connects to a remote IP address and port number. `A polling mechanism can be used (select(), poll(), epoll(), etc) to monitor the socket for information from the server without blocking the thread.`

5. Once connection is established, you can use any of the socket API data transfer APIs. Clients and servers have many data transfer APIs from which to choose, such as send(), recv(), read(), write(), and others. `Client write data to socket (underlying network will send this data to server), ServerSocket recieves the request and create a new socket, server then reads data from newly created socket, do some processing and writes response back to the newly created socket.`

6. When a server or client wants to stop operations, it issues a `close()` API to release any system resources acquired by the socket. `The server closes its connection with the client by calling close. This initiates the normal TCP connection termination sequence: a FIN is sent in each direction and each FIN is acknowledged by the other end. We will say much more about TCP's three-way handshake and the four TCP packets used to terminate a TCP connection.`

## How to read data from socket?

With a byte-stream protocol, these n bytes can be returned in numerous ways: a single TCP segment containing all n bytes of data, in n TCP segments each containing 1 byte of data, or any other combination that totals to n bytes. Normally, a single segment containing all n bytes of data is returned, but with larger data sizes, we cannot assume that the server's reply will be returned by a single read. Therefore, when reading from a TCP socket, we always need to code the read in a loop and terminate the loop when either read returns 0 (i.e., the other end closed the connection) or a value less than 0 (an error).

## How to write data to socket?
YTD
