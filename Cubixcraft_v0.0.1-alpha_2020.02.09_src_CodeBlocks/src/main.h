#pragma once
#include <windows.h>
#include <cmath>
#include <ctime>
#include <GL/gl.h>
#include <GL/glu.h>
#include "frustum.h"

#define VK_A 0x41
#define VK_D 0x44
#define VK_R 0x52
#define VK_S 0x53
#define VK_W 0x57

extern HWND hMainWindow;
extern HWND hPanelRender;

extern int glScreenWidth;
extern int glScreenHeight;

extern POINT oldMousePos;
extern bool mouseGrabbed;

extern double deltaTime;
extern int sceneFps;

extern CFrustum* frustumObj;
extern bool bNeedToRecalculateFrustum;
