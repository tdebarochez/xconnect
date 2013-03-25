#include "XPLMPlugin.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "XPLMScenery.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "FDSSession.h"


const char* version = "0.0.1";

#if IBM
#include <windows.h>

HINSTANCE hDLLInstance = 0;

#ifdef __cplusplus
extern "C"
{
#endif
     PLUGIN_API int XPluginStart(char* name, char* signature, char* description);
     PLUGIN_API int XPluginEnable(void);
     PLUGIN_API void XPluginDisable(void);
     PLUGIN_API void XPluginStop(void);
     PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void* inParam);
 #ifdef __cplusplus
}
#endif

BOOL APIENTRY DllMain( HINSTANCE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{

	hDLLInstance = hModule;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif




PLUGIN_API int XPluginStart(char* name, char* signature, char* description)
{

	strcpy(name, "XConnectServer");
	strcpy(signature, "FDSoft.XConnectServer");
	strcpy(description, "Provides the same functionality as FSUIPC");
	module_init(hDLLInstance);
	return 1;
}


PLUGIN_API int XPluginEnable(void)
{
	return 1;
}
PLUGIN_API void XPluginDisable(void)
{
	return;
}

PLUGIN_API void XPluginStop(void)
{
	module_deinit();
	return;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void* inParam)
{
    return;
}