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

#include "XCSession.h"
#include "XPLMPlugin.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "XPLMScenery.h"

#include "XCRead.h"
#include "XCWrite.h"
#include "Common.h"

WNDCLASS wc;
HWND hWnd;
UINT XC_CALL = 0;
static UINT XCModuleCallMessage = WM_USER + 256;
__int32 GlobalOperationType = WaitingT;




LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if((message == XC_CALL) || (message == XCModuleCallMessage)) 
	{
        HANDLE hMap = 0;
		BYTE* pView = NULL;
		BYTE* pNext = NULL;
		XC_ACTION_READ_HDR* pHdrR = NULL;
		XC_ACTION_WRITE_HDR* pHdrW = NULL;
		XC_ACTION_READTOKEN_HDR* pHdrRT = NULL;
		XC_ACTION_WRITETOKEN_HDR* pHdrWT = NULL;
		XC_ACTION_SEARCH_HDR* pHdrS = NULL;
		DWORD* pdw = NULL;
		DWORD true_offset;
		char szName[MAX_PATH];
		ATOM atom = wParam;
		LRESULT result = XC_RETURN_FAILURE;

	    FILE* str = fopen("XConnect.out", "a+");
        fprintf(str, "sizeof(XC_ACTION_READ_HDR)=%d\n", (int)sizeof(XC_ACTION_READ_HDR));
	    if (message == XC_CALL) fprintf(str, "XC_CALL");
	    if (message == XCModuleCallMessage) fprintf(str, "XCModuleCallMessage");
        fprintf(str, " W=%d L=%p\n", (int)wParam, (void*)lParam);

		if (message == XCModuleCallMessage) {
			hMap = (BYTE*)lParam;
		}
		else {
			GlobalGetAtomName(atom, (LPWSTR)szName, MAX_PATH);
			hMap = OpenFileMapping(FILE_MAP_WRITE, FALSE, (LPWSTR)szName);

            MEMORY_BASIC_INFORMATION info;
            VirtualQuery(hMap, &info, sizeof(info));
            fprintf(str, "FileMapping RegionSize=%d\n", (int)info.RegionSize);
		}

		if (hMap) {
			if (message == XCModuleCallMessage)
				pView = (BYTE*)hMap;
			else
				pView = (BYTE*)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0);
			if (pView) {
				result = XC_RETURN_SUCCESS;
				pNext = pView;

				pdw = (DWORD*)pView;
				while ((*pdw) && (result == XC_RETURN_SUCCESS))  {	
                    switch (*pdw) {
                    case XC_ACTION_READ:        fprintf(str, "  XC_ACTION_READ\n"); break;
                    case XC_ACTION_READTRUE:    fprintf(str, "  XC_ACTION_READTRUE\n"); break;
                    case XC_ACTION_WRITE:       fprintf(str, "  XC_ACTION_WRITE\n"); break;
                    case XC_ACTION_WRITETRUE:   fprintf(str, "  XC_ACTION_WRITETRUE\n"); break;
                    case XC_ACTION_READTOKEN:   fprintf(str, "  XC_ACTION_READTOKEN\n"); break;
                    case XC_ACTION_WRITETOKEN:  fprintf(str, "  XC_ACTION_WRITETOKEN\n"); break;
                    case XC_ACTION_SEARCH:      fprintf(str, "  XC_ACTION_SEARCH\n"); break;
                    }
					switch (*pdw) {
						case XC_ACTION_READ:
						case XC_ACTION_READTRUE:
							{
								GlobalOperationType = ReadingT;

								pHdrR = (XC_ACTION_READ_HDR*)pdw;

								pNext += sizeof(XC_ACTION_READ_HDR);
								if (pHdrR->size) {
									GetVariable(pNext, pHdrR->offset, pHdrR->size);

									//CopyMemory(pNext, (BYTE*)true_offset, pHdrR->size);
								}
								pNext += pHdrR->size;
								pdw = (DWORD*)pNext;
							}
							break;
						case XC_ACTION_WRITE:
						case XC_ACTION_WRITETRUE:
							{
								GlobalOperationType = WritingT;

								pHdrW = (XC_ACTION_WRITE_HDR*)pdw;

								pNext += sizeof(XC_ACTION_WRITE_HDR);
								if (pHdrW->size) {
									
									WriteVariable(pNext, pHdrW->offset, pHdrW->size);
									
									/*if (pHdrW->action == XC_ACTION_WRITETRUE) {
										true_offset = pHdrW->offset;
										if (!IsBadWritePtr((BYTE*)true_offset, pHdrW->size))
											CopyMemory((BYTE*)true_offset, pNext, pHdrW->size);
									}
									else {
										writedataptr = pNext; 
										writedatasize = pHdrW->size; 
										true_offset = GetTrueFS9Offset(pHdrW->offset);
										if (!IsBadWritePtr((BYTE*)true_offset, pHdrW->size))
											CopyMemory((BYTE*)true_offset, pNext, pHdrW->size);
										UpdateFSVar(pHdrW->offset);
									}*/

								}
								pNext += pHdrW->size;
								pdw = (DWORD*)pNext;
							}
							break;
						/*case XC_ACTION_READTOKEN:
							{
								GlobalOperationType = ReadingT;

								pHdrRT = (XC_ACTION_READTOKEN_HDR*)pdw;
								if (pHdrRT) {
									MODULE_VAR mvar = { pHdrRT->token };

									lookup_var(&mvar);

									pNext += sizeof(XC_ACTION_READTOKEN_HDR);
									CopyMemory(pNext, (BYTE*)(&mvar.var_value.n), 8);
									pNext += 8;
									pdw = (DWORD*)pNext;
								}
							}
							break;
						case XC_ACTION_WRITETOKEN:
							{
								GlobalOperationType = WritingT;

								pHdrWT = (XC_ACTION_WRITETOKEN_HDR*)pdw;
								if (pHdrWT) {
									MODULE_VAR mvar = { pHdrWT->token };

									lookup_var(&mvar);

									pNext += sizeof(XC_ACTION_WRITETOKEN_HDR);
									CopyMemory((BYTE*)(mvar.var_ptr), pNext, 8);
									pNext += 8;
									pdw = (DWORD*)pNext;
								}
							}
							break;
						case XC_ACTION_SEARCH:
							{
								FILE* str;
								char buf[200];
								DWORD os;
								BOOL eq;
								DWORD i;

								GlobalOperationType = SearchingT;

								pHdrS = (XC_ACTION_SEARCH_HDR*)pdw;

								pNext += sizeof(XC_ACTION_SEARCH_HDR);
								if (pHdrS->size) {
									if (pHdrS->offset_from < pHdrS->offset_to) {
										for (os = pHdrS->offset_from; os <= pHdrS->offset_to; os++) {
											if (!IsBadReadPtr((BYTE*)os, pHdrS->size)) {
												eq = TRUE;
												for (i = 0; i < pHdrS->size; i++) {
													if (((BYTE*)os)[i] != pNext[i]) {
														eq = FALSE;
														break;
													}
												}
												if (eq) {
													str = fopen("modules\\FDSConnection.out", "a+");
													sprintf(buf, "value found at %d\n", (long)os);
													fprintf(str, buf);
													fclose(str);
												}
											}
										}
									}
									else {
										for (os = pHdrS->offset_from; os >= pHdrS->offset_to; os--) {
											if (!IsBadReadPtr((BYTE*)os, pHdrS->size)) {
												eq = TRUE;
												for (i = 0; i < pHdrS->size; i++) {
													if (((BYTE*)os)[i] != pNext[i]) {
														eq = FALSE;
														break;
													}
												}
												if (eq) {
													str = fopen("modules\\FDSConnection.out", "a+");
													sprintf(buf, "value found at %d\n", (long)os);
													fprintf(str, buf);
													fclose(str);
												}
											}
										}
									}
								}

								pNext += pHdrS->size;
								pdw = (DWORD*)pNext;
							}*/
						default:
							{
								*pdw = 0;
								result = XC_RETURN_FAILURE;
							}
							break;
					}
				}

				pNext = pView;

				if (message == XC_CALL)
					UnmapViewOfFile((LPVOID)pView);
			}

			if (message == XC_CALL)
				CloseHandle(hMap);
		}

	    fclose(str);
		return result;

	}

	return (DefWindowProc(hWnd, message, wParam, lParam));

}


void module_init(HINSTANCE dllInstance)
{
	char szClassName[] = "FS98MAIN";

	FILE* str = fopen("XConnect.out", "w");
	fprintf(str, "XConnect Initialisation\n");

	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = TEXT("FS98MAIN");
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.hInstance = dllInstance;

	if(RegisterClass(&wc))
	{
		hWnd = CreateWindowEx(NULL, TEXT("FS98MAIN"), TEXT("FS98MAIN"), WS_POPUP, 0,0,10,10,NULL, NULL, dllInstance, NULL);
		if(hWnd)
		{
			fprintf(str, "Create window OK: %d\n", hWnd);
			XC_CALL = RegisterWindowMessage(TEXT(XC_CALL_CONNECTION));
			if(XC_CALL != 0)
			{
				fprintf(str, "Register call message OK\n");

				XConnectMemBlock = new BYTE[0x10000];

				for (int i = 0; i < 0x10000; i++) {
					XConnectMemBlock[i] = 0;
				}

			}
		}
	}

	fclose(str);
}

void module_deinit(void)
{
	FILE* str = fopen("XConnect.out", "a+");

	DestroyWindow(hWnd);
	fprintf(str, "XConnect deinitialisation\n");
	fclose(str);
}