#ifndef _PIPESERVER__H
#define _PIPESERVER__H


#define FDS_ACTION_READ				1
#define FDS_ACTION_WRITE			2
#define FDS_ACTION_READTRUE			3
#define FDS_ACTION_WRITETRUE		4
#define FDS_ACTION_READTOKEN		5
#define FDS_ACTION_WRITETOKEN		6
#define FDS_ACTION_SEARCH			7

#define FDS_RETURN_FAILURE			0
#define FDS_RETURN_SUCCESS			1

typedef struct tagFDS_ACTION_READ_HDR {
	DWORD action;
	DWORD offset;
	DWORD size;
	void* data;
} FDS_ACTION_READ_HDR;

typedef struct tagFDS_ACTION_WRITE_HDR {
	DWORD action;
	DWORD offset;
	DWORD size;
} FDS_ACTION_WRITE_HDR;

typedef struct tagFDS_ACTION_READTOKEN_HDR {
	DWORD action;
	DWORD token;
	void* data;
} FDS_ACTION_READTOKEN_HDR;

typedef struct tagFDS_ACTION_WRITETOKEN_HDR {
	DWORD action;
	DWORD token;
} FDS_ACTION_WRITETOKEN_HDR;

#ifdef __cplusplus
extern "C" {
#endif

void MyErrExit(LPTSTR mess);
void PipeServing();
void InstanceThread(LPVOID lpvParam); 
void ProcessData(BYTE* data, LPDWORD size);

#ifdef __cplusplus
};
#endif

#endif //  _PIPESERVER__H