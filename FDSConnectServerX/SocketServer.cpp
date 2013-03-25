//
//#include <winsock.h> 


#include "SocketServer.h"

#include <stdio.h>
#include <string.h>

#include <windows.h>

#include "FDSConnectionTypes.h"


#include "FDSRead.h"
#include "FDSWrite.h"
#include <errno.h>

#define BUFSIZE			0x7F00
#define PROTOPORT       5193            /* default protocol port number */
#define QLEN            6               /* size of request queue        */

static BOOL ServerInitialized = FALSE;

void ProcessSocketData(char* data, int size);
void SocketInstanceThread(LPVOID lpvParam);
//void ProcessReceivedData(char*, int size);


BOOL InitializeSocketServer()
{
	/*FDSConnectionWnd = FindWindow("FDSConnection", NULL);
	if (!FDSConnectionWnd) {
		MessageBox(NULL, "Cannot find FDSConnection!", "FDSWideConnectionServer Error", MB_OK);
		return FALSE;
	}
	*/
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
		int len = c(sd, data, sizeof(data), 0, &from, &fromlen);

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

    /*if (listen(sd, QLEN) < 0) {
        sprintf(buf,"listen failed\n");
        fdsError(buf);
		return FALSE;
    }*/


	/*SocketHandler h;
	UdpProcessSocket s(h);
	port_t port = PROTOPORT;
	if(s.Bind(port, 10) == -1)
	{
		sprintf(buf,"bind failed\n");
        fdsError(buf);
		return FALSE;
	}

	h.Add(&s);
	
	h.Select(1,0);
	while(h.GetCount())
	{
		h.Select(1, 0);
	}*/

	//hThread = CreateThread( 
 //           NULL,
 //           0,
 //           (LPTHREAD_START_ROUTINE) SocketInstanceThread, 
 //           (LPVOID) h,
 //           0,
 //           &dwThreadId);

 //        if (hThread == NULL) {
	//		sprintf(buf,"creating thread failed\n");
	//		fdsError(buf);
	//		return FALSE;
	//	}

 //   while (1) {
 //       alen = sizeof(cad);
 //       if ( (sd2=accept(sd, (struct sockaddr *)&cad, &alen)) < 0) {
 //           sprintf(buf, "accept failed\n");
	//		fdsError(buf);
	//		return FALSE;
 //       }

 //      //Create a thread for this client. 
 //        hThread = CreateThread( 
 //           NULL,
 //           0,
 //           (LPTHREAD_START_ROUTINE) SocketInstanceThread, 
 //           (LPVOID) sd2,
 //           0,
 //           &dwThreadId);

 //        if (hThread == NULL) {
	//		sprintf(buf,"creating thread failed\n");
	//		fdsError(buf);
	//		return FALSE;
	//	}
	//}

	WSACleanup();

	return TRUE;
}

/*void SocketInstanceThread(LPVOID lpvParam)
{

	

	//fdsError("Start loop\n");

	char* data = new char[BUFSIZE];
	
	
	char* procPos = data;

	memset(data, 0, BUFSIZE);

	DWORD* pdw = NULL;
	FDS_ACTION_READ_HDR* pHdrR = NULL;
	FDS_ACTION_WRITE_HDR* pHdrW = NULL;

	int size=0;
	int sd2 = (int)lpvParam;
	int processed = 0;

	int ret = recv(sd2, data, sizeof(DWORD), MSG_WAITALL);
	while ((ret > 0)  && (ret != SOCKET_ERROR)) {

		FILE* str = fopen("FDSWideConnection.out", "a+");
		fprintf(str, "Data received: %d\n", ret);
		fclose(str);

		size = ret;


	
		pdw = (DWORD*)procPos;
		if(*pdw == FDS_ACTION_READ )
		{
			recv(sd2, data + sizeof(DWORD), sizeof(FDS_ACTION_READ_HDR) - sizeof(DWORD), MSG_WAITALL);
			pHdrR = (FDS_ACTION_READ_HDR*)pdw;
			ret = recv(sd2, data + sizeof(FDS_ACTION_READ_HDR), pHdrR->size, MSG_WAITALL);

			if(ret>0 && ret!=SOCKET_ERROR)
			{
				size = sizeof(FDS_ACTION_READ_HDR) + pHdrR->size;
				ProcessSocketData(procPos, sizeof(FDS_ACTION_READ_HDR) + pHdrR->size);
			}

		}
		else if(*pdw == FDS_ACTION_WRITE)
		{

			recv(sd2, data + sizeof(DWORD), sizeof(FDS_ACTION_WRITE_HDR) - sizeof(DWORD), MSG_WAITALL);
			pHdrW = (FDS_ACTION_WRITE_HDR*)pdw;
			ret = recv(sd2, data + sizeof(FDS_ACTION_WRITE_HDR), pHdrW->size, MSG_WAITALL);

			if(ret>0 && ret!=SOCKET_ERROR)
			{
				size = sizeof(FDS_ACTION_WRITE_HDR) + pHdrW->size;
				ProcessSocketData(procPos, sizeof(FDS_ACTION_WRITE_HDR) + pHdrW->size);
			}

		}
					
			
		

		
		//fdsError("Send Data to client\n");
		send(sd2, data, size, 0);

		//fdsError("Wait for next receive\n");

		ret = recv(sd2, data, sizeof(DWORD), 0);
	}

	shutdown(sd2, 0x00);
	closesocket(sd2);
}
*/


void ProcessSocketData(char* data, int size)
{


	//FILE* str = fopen("FDSWideConnection.out", "a+");

	//fprintf(str, "Process socket data\n");

	//fflush(str);

	DWORD error;

	BYTE* pView;
	BYTE* pNext;
	DWORD* pdw = NULL;
	FDS_ACTION_READ_HDR* pHdrR = NULL;
	FDS_ACTION_WRITE_HDR* pHdrW = NULL;

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
				case FDS_ACTION_READ:
				case FDS_ACTION_READTRUE:
					{


						pHdrR = (FDS_ACTION_READ_HDR*)pdw;
						pNext += sizeof(FDS_ACTION_READ_HDR);

						sprintf(buf, "Get variable %x - %d\n", pHdrR->offset, pHdrR->size);
						fdsError(buf);
						//fflush(str);

						GetVariable(pNext, pHdrR->offset, pHdrR->size);
						pNext += pHdrR->size;
						pdw = (DWORD*)pNext;
					}
					break;
				case FDS_ACTION_WRITE:
				case FDS_ACTION_WRITETRUE:
					{

						pHdrW = (FDS_ACTION_WRITE_HDR*)pdw;
						pNext += sizeof(FDS_ACTION_WRITE_HDR);

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
		//if (error != FDS_RETURN_SUCCESS)
			//MessageBox(NULL, "Error communication with FDSConnect", "FDSWideConnectionServer Error", MB_OK);
	}

	//fclose(str);
}
