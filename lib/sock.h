// Wrapper function on socket.
int Socket(int, int, int);
// Wrapper function on bind.
void Bind(int, const struct sockaddr *, int);
// Wrapper function on connect.
void Connect(int, const struct sockaddr *, int);