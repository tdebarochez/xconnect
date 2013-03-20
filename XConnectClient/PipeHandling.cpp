/*
This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "PipeHandling.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <windows.h> 

void MyErrExit(LPTSTR mess)
{
	AfxMessageBox(mess);
	exit(0);
}

BOOL PipeCallServer(LPVOID lpvParam, int size, LPTSTR ServerName) 
{
   HANDLE hPipe; 
   BOOL fSuccess; 
   DWORD cbRead, cbWritten, dwMode;
   char lpszPipename[100];
   char buf[100];
   sprintf(lpszPipename, "\\\\%s\\pipe\\FDSWideConnection", ServerName);

// Try to open a named pipe; wait for it, if necessary. 
 
   while (1) 
   { 
      hPipe = CreateFile( 
         lpszPipename,   // pipe name 
         GENERIC_READ |  // read and write access 
         GENERIC_WRITE, 
         0,              // no sharing 
         NULL,           // no security attributes
         OPEN_EXISTING,  // opens existing pipe 
         0,              // default attributes 
         NULL);          // no template file 
 
   // Break if the pipe handle is valid. 
 
      if (hPipe != INVALID_HANDLE_VALUE) 
         break; 
 
      // Exit if an error other than ERROR_PIPE_BUSY occurs. 
 
      if (GetLastError() != ERROR_PIPE_BUSY) {
		 //FILE* str = fopen("FDSWideConnectionClient.log", "a+");
		 //sprintf(buf, "Error: %d\n", GetLastError());
		 //fprintf(str, buf);
		 //fclose(str);
		 return FALSE;
	  } 
      // All pipe instances are busy, so wait for 20 seconds. 
 
      if (! WaitNamedPipe(lpszPipename, 20000) ) {
		 //FILE* str = fopen("FDSWideConnectionClient.log", "a+");
		 //fprintf(str, "Error: WaitNamedPipe!\n");
		 //fclose(str);
		 return FALSE;
	  } 

   } 
 
// The pipe connected; change to message-read mode. 
 
   dwMode = PIPE_READMODE_MESSAGE; 
   fSuccess = SetNamedPipeHandleState( 
      hPipe,    // pipe handle 
      &dwMode,  // new pipe mode 
      NULL,     // don't set maximum bytes 
      NULL);    // don't set maximum time 
   if (!fSuccess) {
	  //FILE* str = fopen("FDSWideConnectionClient.log", "a+");
	  //fprintf(str, "Error: SetNamedPipeHandleState!\n");
  	  //fclose(str);
	  return FALSE;
   }
 
// Send a message to the pipe server. 
 
   fSuccess = WriteFile( 
      hPipe,                  // pipe handle 
      lpvParam,               // message 
      size,				      // message length 
      &cbWritten,             // bytes written 
      NULL);                  // not overlapped 
   if (! fSuccess) {
	  //FILE* str = fopen("FDSWideConnectionClient.log", "a+");
	  //fprintf(str, "Error: SetNamedPipeHandleState!\n");
  	  //fclose(str);
	  return FALSE;
   }
 
   do 
   { 
   // Read from the pipe. 
 
      fSuccess = ReadFile( 
         hPipe,    // pipe handle 
         lpvParam, // buffer to receive reply 
         size,     // size of buffer 
         &cbRead,  // number of bytes read 
         NULL);    // not overlapped 
 
      if (! fSuccess && GetLastError() != ERROR_MORE_DATA) 
         break; 
  
   } while (! fSuccess);  // repeat loop if ERROR_MORE_DATA 
 
   CloseHandle(hPipe); 

   return TRUE;
}
