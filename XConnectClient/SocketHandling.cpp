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

#include <windows.h>
///#include <winsock.h>

#include <stdio.h>
#include <string.h>
#include <conio.h>

///////////////////////////////////////////////////////////////////////////////

#define PROTOPORT 5193						/* default protocol port number */

struct  hostent  *ptrh;						/* pointer to a host table entry       */
struct  protoent *ptrp;						/* pointer to a protocol table entry   */
struct  sockaddr_in sad;					/* structure to hold an IP address     */
int     sd;									/* socket descriptor                   */
int     port;								/* protocol port number                */

///////////////////////////////////////////////////////////////////////////////

void fdsError(char* error) {
	FILE* str = fopen("XCONNECTCLIENT.out", "a+");
	if (str) {
		fprintf(str, error);
		fclose(str);
	}
}

BOOL OpenConnection(char* host)
{
	char buf[100];

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	memset((char *)&sad,0,sizeof(sad));		/* clear sockaddr structure */
	sad.sin_family = AF_INET;				/* set family to Internet     */

	port = PROTOPORT;						/* use default port number      */
	sad.sin_port = htons((u_short)port);

    ptrh = gethostbyname(host);
    if (((char *)ptrh) == NULL ) {
		sprintf(buf,"invalid host: %s\n", host);
		fdsError(buf);
		return FALSE;
    }
    memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

    if (((int)(ptrp = getprotobyname("udp"))) == 0) {
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

    /*if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        sprintf(buf,"connect failed\n");
		fdsError(buf);
        return FALSE;
    }*/

	return TRUE;
}

BOOL CloseConnection()
{
	shutdown(sd, 0x00);
	closesocket(sd);
	WSACleanup();

	return TRUE;
}

BOOL CallServer(BYTE* data, int size)
{

	/*FILE* str = fopen("XCONNECTCLIENT.out", "a+");
	if(str)
	{
		
		fprintf(str, "Send data to server.  Size: %d\n", size);
		fclose(str);
	}*/

	sockaddr from;
	
	int fromlen = sizeof(sockaddr);

	int error = 0;

	char cSize[4];
	
	memcpy(cSize, &size, 4);

	error = sendto(sd, cSize, 4, 0, (sockaddr*)(&sad), sizeof(sad));
	if(error == SOCKET_ERROR)
		return FALSE;

	for(int i=0;i<size;i+=1000)
	{
		error = sendto(sd, (const char*)(data + i), min(1000, size - i) ,0, (sockaddr*)(&sad), sizeof(sad));

		if(error == SOCKET_ERROR)
			return FALSE;
	}
	/*str = fopen("XCONNECTCLIENT.out", "a+");
	if(str)
	{
		
		fprintf(str, "Data sent.  Size: %d\n", error);
		fclose(str);
	}*/

	for(int i=0;i<size;i+=1000)
	{
		error = recvfrom(sd, (char*)(data + i), min(1000, size - i), 0, &from, &fromlen );
		if(error == SOCKET_ERROR)
			return FALSE;
	}
	/*str = fopen("XCONNECTCLIENT.out", "a+");
	if(str)
	{
		
		fprintf(str, "Received data back.  Size: %d\n", error);
		fclose(str);
	}*/
	
	return TRUE;
}
