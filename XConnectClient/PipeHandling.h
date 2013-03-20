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

#ifndef PIPEHANDLING__H
#define PIPEHANDLING__H

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

void MyErrExit(LPTSTR mess);
BOOL PipeCallServer(LPVOID lpvParam, int size, LPTSTR ServerName);

#endif //  PIPEHANDLING__H