#ifndef SocketHandling__h
#define SocketHandling__h

BOOL OpenConnection(char* host);
BOOL CloseConnection();
BOOL CallServer(BYTE* data, int size);

#endif