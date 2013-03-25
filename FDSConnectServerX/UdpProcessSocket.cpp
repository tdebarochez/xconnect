#include <Utility.h>
#include <string>
#include "UdpProcessSocket.h"

#include "FDSConnectionTypes.h"
#include "FDSRead.h"
#include "FDSWrite.h"

UdpProcessSocket::UdpProcessSocket(ISocketHandler& h) 
: UdpSocket(h)
{
}

UdpProcessSocket::~UdpProcessSocket(void)
{
}


void UdpProcessSocket::OnRawData(const char *p,size_t l,struct sockaddr *sa_from,socklen_t sa_len)
{

	struct sockaddr_in sa;
	memcpy(&sa,sa_from,sa_len);
	ipaddr_t a;
	memcpy(&a,&sa.sin_addr,4);
	std::string ip;
	Utility::l2ip(a,ip);


	char* data = new char[l];
	memcpy(data, p, l);

	ProcessSocketData(data, l);

	//UdpSocket s(this->Handler());
	

	
	
}

void UdpProcessSocket::ProcessSocketData(char* data, int size)
{


	FILE* str = fopen("FDSWideConnection.out", "a+");

	fprintf(str, "Process socket data\n");

	fflush(str);

	DWORD error;

	BYTE* pView;
	BYTE* pNext;
	DWORD* pdw = NULL;
	FDS_ACTION_READ_HDR* pHdrR = NULL;
	FDS_ACTION_WRITE_HDR* pHdrW = NULL;

	

	
	//fprintf(str, "Server is initialized\n");
	//fflush(str);

	

	pView = (BYTE*)data;
	if (pView) {
		pNext = pView;

		pdw = (DWORD*)pView;
		//while (*pdw) {	
			switch (*pdw) {
				case FDS_ACTION_READ:
				case FDS_ACTION_READTRUE:
					{


						pHdrR = (FDS_ACTION_READ_HDR*)pdw;
						pNext += sizeof(FDS_ACTION_READ_HDR);

						//fprintf(str, "Get variable %x - %d\n", pHdrR->offset, pHdrR->size);
						//fflush(str);

						//GetVariable(pNext, pHdrR->offset, pHdrR->size);
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

						//WriteVariable((BYTE*)pNext, pHdrW->offset, pHdrW->size);
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
		}

		pNext += 4;
		size = pNext-pView;

		/*error = SendMessage(FDSConnectionWnd,
							FDSModuleCallMessage,
							(WPARAM)(pNext - pView),
							(LPARAM)data
						   );*/

		//if (error != FDS_RETURN_SUCCESS)
			//MessageBox(NULL, "Error communication with FDSConnect", "FDSWideConnectionServer Error", MB_OK);
	//}

	//fclose(str);
}