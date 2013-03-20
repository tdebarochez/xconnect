#pragma once

#include <windows.h>
#include <map>
#include <string>
#include <cstdlib>
#include "XPLMPlugin.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "XPLMScenery.h"
#include "XPLMNavigation.h"

using namespace std;

extern BYTE* XConnectMemBlock;
extern bool Throttle1Disconnect;
extern bool Throttle2Disconnect;
extern bool PitchDisconnect;
extern bool RollDisconnect;
extern XPLMDataRef test;




typedef std::map<string, XPLMDataRef> StringMap;

extern StringMap VarMap;

template <typename T>
void XCCopyMemory(unsigned char* target, T const* source)
{
  memcpy(static_cast<void*>(target), static_cast<void const*>(source), sizeof(T));
}