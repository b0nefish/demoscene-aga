#ifndef __DEMO_H__
#define __DEMO_H__

#include "std/types.h"

bool LoadConfig();
bool LoadDemo();
void SetupResources();
void BeginDemo();
void KillDemo();
void DemoUpdateTime(int oldFrameNumber, int newFrameNumber);

#endif
