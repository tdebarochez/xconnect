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



#include <stdio.h>
#include "SocketServer.h"
#include "XCSession.h"
#include "Common.h"

DWORD dwThreadId;
HANDLE hThread;

void module_init(HINSTANCE dllInstance)
{
	FILE* str = fopen("XCOnnectServer.out", "w");
	fprintf(str, "XConnectServer init OK!\n");


	XConnectMemBlock = new BYTE[0x10000];

	for (int i = 0; i < 0x10000; i++) {
		XConnectMemBlock[i] = 0;
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