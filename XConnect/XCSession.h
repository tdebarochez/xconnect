#pragma once

#include <windows.h>
#include <stdio.h>
#include "main.h"


#define XC_CALL_CONNECTION			"FSASMLIB:IPC"
#define XC_ACTION_READ				1
#define XC_ACTION_WRITE			2
#define XC_ACTION_READTRUE			3
#define XC_ACTION_WRITETRUE		4
#define XC_ACTION_READTOKEN		5
#define XC_ACTION_WRITETOKEN		6
#define XC_ACTION_SEARCH			7

#define XC_RETURN_FAILURE			0
#define XC_RETURN_SUCCESS			1


void module_init(HINSTANCE dllInstance);
void module_deinit();



typedef struct tagXC_ACTION_READ_HDR {
	DWORD action;
	DWORD offset;
	DWORD size;
	void* data;
} XC_ACTION_READ_HDR;

typedef struct tagXC_ACTION_WRITE_HDR {
	DWORD action;
	DWORD offset;
	DWORD size;
} XC_ACTION_WRITE_HDR;

typedef struct tagXC_ACTION_READTOKEN_HDR {
	DWORD action;
	DWORD token;
	void* data;
} XC_ACTION_READTOKEN_HDR;

typedef struct tagXC_ACTION_WRITETOKEN_HDR {
	DWORD action;
	DWORD token;
} XC_ACTION_WRITETOKEN_HDR;

typedef struct tagXC_ACTION_SEARCH_HDR {
	DWORD action;
	DWORD offset_from;
	DWORD offset_to;
	DWORD size;
} XC_ACTION_SEARCH_HDR;

enum OperationType {
	WaitingT,
	WritingT,
	ReadingT,
	SearchingT
};