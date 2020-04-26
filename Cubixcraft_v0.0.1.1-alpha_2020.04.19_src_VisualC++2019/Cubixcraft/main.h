#include <Windows.h>
#include <cmath>
#include <ctime>
#include <codecvt>
#include <GL/gl.h>
#include <GL/glu.h>

class CFrustum;

const int VK_A = 0x41;
const int VK_D = 0x44;
const int VK_R = 0x52;
const int VK_S = 0x53;
const int VK_W = 0x57;

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
