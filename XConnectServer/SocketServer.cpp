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

//
//#include <winsock.h> 


#include "SocketServer.h"

#include <stdio.h>
#include <string.h>

#include <windows.h>

#include "XConnectionTypes.h"


#include "XCRead.h"
#include "XCWrite.h"
#include <errno.h>

#define BUFSIZE			0x7F00
#define PROTOPORT       5193            /* default protocol port number */
#define QLEN            6               /* size of request queue        */

static BOOL ServerInitialized = FALSE;

void ProcessSocketData(char* data, int size);
void SocketInstanceThread(LPVOID lpvParam);


BOOL InitializeSocketServer()
{
	ServerInitialized = TRUE;
	return TRUE;
}

void fdsError(char* error) {
	FILE* str = fopen("FDSWideConnection.out", "a+");
	if (str) {
		fprintf(str, error);
		fclose(str);
	}
}

DWORD WINAPI SocketServer(void)
{
    struct  protoent *ptrp;  /* pointer to a protocol table entry   */
    struct  sockaddr_in sad; /* structure to hold server's address  */
    struct  sockaddr_in cad; /* structure to hold client's address  */
    int     sd, sd2;         /* socket descriptors                  */
    int     port;            /* protocol port number                */
    int     alen;            /* length of address                   */
    char    buf[500];

	HANDLE hThread = NULL;
	DWORD dwThreadId;

    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);

	memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
    sad.sin_family = AF_INET;			/* set family to Internet     */
    sad.sin_addr.s_addr = INADDR_ANY;	/* set the local IP address   */

    port = PROTOPORT;
	sad.sin_port = htons((u_short)port);


    if ( ((int)(ptrp = getprotobyname("udp"))) == 0) {
        sprintf(buf, "cannot map \"tcp\" to protocol number");
        fdsError(buf);
		return FALSE;
    }

    sd = socket(AF_INET, SOCK_DGRAM, ptrp->p_proto);
    if (sd < 0) {
        sprintf(buf, "socket creation failed\n");
        fdsError(buf);
		return FALSE;
    }

    if (bind(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        sprintf(buf,"bind failed\n");
        fdsError(buf);
		return FALSE;
    }


	char data[BUFSIZE];
	sockaddr from;
	//int fromlen = sizeof(from);

	fdsError("Starting loop\n");

	while(true)
	{
		int fromlen = sizeof(from);
		int len = recvfrom(sd, data, sizeof(data), 0, &from, &fromlen);

		if(len>0)
		{

			//fdsError("Data received\n");

			sprintf(buf, "Data received: %d\n", len);
			fdsError(buf);
			ProcessSocketData(data, len);

			fromlen = sizeof(from);
			sendto(sd, data, len, 0, &from, fromlen);


		}
		else
		{
			sprintf(buf, "Error code: %d\n", WSAGetLastError());
			//sprintf(buf, strerror(WSAGetLastError()));
			//sprintf(buf, "Len equals: %d\n", len);
			fdsError(buf);
		}
	}

  
	WSACleanup();

	return TRUE;
}




void ProcessSocketData(char* data, int size)
{


	//FILE* str = fopen("FDSWideConnection.out", "a+");

	//fprintf(str, "Process socket data\n");

	//fflush(str);

	DWORD error;

	BYTE* pView;
	BYTE* pNext;
	DWORD* pdw = NULL;
	XC_ACTION_READ_HDR* pHdrR = NULL;
	XC_ACTION_WRITE_HDR* pHdrW = NULL;

	if (!ServerInitialized) {
		if (!InitializeSocketServer()) {
			fdsError("Initialization error!\n");
			return;
		}
	}

	
	//fprintf(str, "Server is initialized\n");
	//fflush(str);

	char buf[5000];

	

	pView = (BYTE*)data;
	if (pView) {
		pNext = pView;

		pdw = (DWORD*)pView;
		while (*pdw) {	
			switch (*pdw) {
				case XC_ACTION_READ:
				case XC_ACTION_READTRUE:
					{


						pHdrR = (XC_ACTION_READ_HDR*)pdw;
						pNext += sizeof(XC_ACTION_READ_HDR);

						sprintf(buf, "Get variable %x - %d\n", pHdrR->offset, pHdrR->size);
						fdsError(buf);
						//fflush(str);

						GetVariable(pNext, pHdrR->offset, pHdrR->size);
						pNext += pHdrR->size;
						pdw = (DWORD*)pNext;
					}
					break;
				case XC_ACTION_WRITE:
				case XC_ACTION_WRITETRUE:
					{

						pHdrW = (XC_ACTION_WRITE_HDR*)pdw;
						pNext += sizeof(XC_ACTION_WRITE_HDR);

						//fprintf(str, "Set variable %x\n", pHdrW->offset);
						//fflush(str);

						WriteVariable((BYTE*)pNext, pHdrW->offset, pHdrW->size);
						pNext += pHdrW->size;
						pdw = (DWORD*)pNext;
					}
					break;
				default:
					{
						*pdw = 0;
					}
					break;
			}

			fdsError("Process Data complete\n");


		}

		/*pNext += 4;
		size = pNext-pView;*/

		/*error = SendMessage(FDSConnectionWnd,
							FDSModuleCallMessage,
							(WPARAM)(pNext - pView),
							(LPARAM)data
						   );
*/
		//if (error != XC_RETURN_SUCCESS)
			//MessageBox(NULL, "Error communication with FDSConnect", "FDSWideConnectionServer Error", MB_OK);
	}

	//fclose(str);
}
