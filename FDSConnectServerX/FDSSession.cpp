

#include <stdio.h>
#include "SocketServer.h"
#include "FDSSession.h"
#include "Common.h"

DWORD dwThreadId;
HANDLE hThread;

void module_init(HINSTANCE dllInstance)
{
	FILE* str = fopen("FDSWideConnection.out", "w");
	fprintf(str, "FDSWideConnectionServer init OK!\n");


	FDSMemBlock = new BYTE[0x10000];

	for (int i = 0; i < 0x10000; i++) {
		FDSMemBlock[i] = 0;
	}

	//InitializeCriticalSection(&GDILock);
    hThread = CreateThread( 
            NULL,				// no security attribute 
            0,					// default stack size 
            (LPTHREAD_START_ROUTINE) SocketServer,
            (LPVOID) 0,			// thread parameter 
            0,					// not suspended 
            &dwThreadId);		// returns thread ID
	
	if (hThread)
		fprintf(str, "Create thread OK!\n");

	fclose(str);
}

void module_deinit()
{
	CloseHandle(hThread);

}