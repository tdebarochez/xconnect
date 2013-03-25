#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <windows.h> 
#include "PipeServer.h"
 
#define BUFSIZE 0x7F00 //2048
#define PIPE_TIMEOUT 60

static HWND FDSConnectionWnd = 0;
static UINT FDSModuleCallMessage = WM_USER + 256;
static BOOL ServerInitialized = FALSE;

BYTE data[BUFSIZE];

BOOL InitializeServer()
{
	FDSConnectionWnd = FindWindow("FDSConnection", NULL);
	if (!FDSConnectionWnd) {
		MyErrExit("Cannot find FDSConnection!");
		return FALSE;
	}

	ServerInitialized = TRUE;
	return TRUE;
}

void MyErrExit(LPTSTR mess)
{
	MessageBox(NULL, mess, "FDSWideConnectionServer Error", MB_OK);
	//exit(0);
}

void PipeServing()
{ 
   BOOL fConnected; 
   DWORD dwThreadId; 
   HANDLE hPipe, hThread; 
   LPTSTR lpszPipename = "\\\\.\\pipe\\FDSWideConnectionX"; 

   PSECURITY_DESCRIPTOR    pSD;
   SECURITY_ATTRIBUTES     sa;

   pSD = (PSECURITY_DESCRIPTOR) LocalAlloc( LPTR,
   SECURITY_DESCRIPTOR_MIN_LENGTH);
   InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);
   // Add a NULL DACL to the security descriptor..
   SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE);
   sa.nLength = sizeof(sa);
   sa.lpSecurityDescriptor = pSD;
   sa.bInheritHandle = TRUE;

// The main loop creates an instance of the named pipe and 
// then waits for a client to connect to it. When the client 
// connects, a thread is created to handle communications 
// with that client, and the loop is repeated. 
 
   for (;;) 
   { 
      hPipe = CreateNamedPipe( 
          lpszPipename,             // pipe name 
          PIPE_ACCESS_DUPLEX,       // read/write access 
          PIPE_TYPE_MESSAGE |       // message type pipe 
          PIPE_READMODE_MESSAGE |   // message-read mode 
          PIPE_WAIT,                // blocking mode 
          PIPE_UNLIMITED_INSTANCES, // max. instances  
          BUFSIZE,                  // output buffer size 
          BUFSIZE,                  // input buffer size 
          PIPE_TIMEOUT,             // client time-out 
          &sa);

      if (hPipe == INVALID_HANDLE_VALUE) 
          MyErrExit("CreatePipe"); 
 
      // Wait for the client to connect; if it succeeds, 
      // the function returns a nonzero value. If the function returns 
      // zero, GetLastError returns ERROR_PIPE_CONNECTED. 
 
      fConnected = ConnectNamedPipe(hPipe, NULL) ? 
         TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
 
      if (fConnected) 
      { 
      // Create a thread for this client. 
         hThread = CreateThread( 
            NULL,              // no security attribute 
            0,                 // default stack size 
            (LPTHREAD_START_ROUTINE) InstanceThread, 
            (LPVOID) hPipe,    // thread parameter 
            0,                 // not suspended 
            &dwThreadId);      // returns thread ID 

         if (hThread == NULL) 
            MyErrExit("CreateThread"); 
         else
            CloseHandle(hThread); 
 
      } 
      else 
        // The client could not connect, so close the pipe. 
         CloseHandle(hPipe); 
   } 
} 
 
void InstanceThread(LPVOID lpvParam) 
{
   DWORD cbBytesRead, cbReplyBytes, cbWritten; 
   BOOL fSuccess; 
   HANDLE hPipe; 
 
// The thread's parameter is a handle to a pipe instance. 
 
   hPipe = (HANDLE) lpvParam; 
 
   while (1) 
   { 
   // Read client requests from the pipe. 
      fSuccess = ReadFile( 
         hPipe,        // handle to pipe 
		 data,		   // buffer to receive data 
         BUFSIZE,      // size of buffer 
         &cbBytesRead, // number of bytes read 
         NULL);        // not overlapped I/O 

      if (! fSuccess || cbBytesRead == 0) 
         break; 
	  ProcessData(data, &cbReplyBytes); 

 
   // Write the reply to the pipe. 
      fSuccess = WriteFile( 
         hPipe,        // handle to pipe 
		 data,		   // buffer to write from 
         cbReplyBytes, // number of bytes to write 
         &cbWritten,   // number of bytes written 
         NULL);        // not overlapped I/O 

      if (! fSuccess || cbReplyBytes != cbWritten) break; 
  } 
 
// Flush the pipe to allow the client to read the pipe's contents 
// before disconnecting. Then disconnect the pipe, and close the 
// handle to this pipe instance. 
 
   FlushFileBuffers(hPipe); 
   DisconnectNamedPipe(hPipe); 
   CloseHandle(hPipe); 
} 

void ProcessData(BYTE* data, LPDWORD size)
{
	DWORD error;

	BYTE* pView;
	BYTE* pNext;
	DWORD* pdw = NULL;
	FDS_ACTION_READ_HDR* pHdrR = NULL;
	FDS_ACTION_WRITE_HDR* pHdrW = NULL;
	FDS_ACTION_READTOKEN_HDR* pHdrRT = NULL;
	FDS_ACTION_WRITETOKEN_HDR* pHdrWT = NULL;

	if (!ServerInitialized) {
		if (!InitializeServer()) {
			MyErrExit("Initialization error!");
			return;
		}
	}

	pView = data;
	if (pView) {
		pNext = pView;

		pdw = (DWORD*)pView;
		while (*pdw) {	
			switch (*pdw) {
				case FDS_ACTION_READ:
				case FDS_ACTION_READTRUE:
					{
						pHdrR = (FDS_ACTION_READ_HDR*)pdw;
						pNext += sizeof(FDS_ACTION_READ_HDR);
						pNext += pHdrR->size;
						pdw = (DWORD*)pNext;
					}
					break;
				case FDS_ACTION_WRITE:
				case FDS_ACTION_WRITETRUE:
					{
						pHdrW = (FDS_ACTION_WRITE_HDR*)pdw;
						pNext += sizeof(FDS_ACTION_WRITE_HDR);
						pNext += pHdrW->size;
						pdw = (DWORD*)pNext;
					}
					break;
				case FDS_ACTION_READTOKEN:
					{
						pHdrRT = (FDS_ACTION_READTOKEN_HDR*)pdw;
						pNext += sizeof(FDS_ACTION_READTOKEN_HDR);
						pNext += 8;
						pdw = (DWORD*)pNext;
					}
					break;
				case FDS_ACTION_WRITETOKEN:
					{
						pHdrWT = (FDS_ACTION_WRITETOKEN_HDR*)pdw;
						pNext += sizeof(FDS_ACTION_WRITETOKEN_HDR);
						pNext += 8;
						pdw = (DWORD*)pNext;
					}
					break;
				default:
					{
						*pdw = 0;
					}
					break;
			}
		}

		pNext += 4;
		*size = pNext-pView;

		error = SendMessage(FDSConnectionWnd,
							FDSModuleCallMessage,
							(WPARAM)(pNext - pView),
							(LPARAM)data
						   );

		if (error != FDS_RETURN_SUCCESS) {	
			MyErrExit("Error communication with FDSConnect");
		}
	}
}
