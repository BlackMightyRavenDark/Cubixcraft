#include <iostream>
#include <codecvt>
#include <GL/gl.h>

class CBlock;
class CTessellator;

enum class MOUSE_WHEEL_DIRECTION { MW_UP = 0, MW_DOWN = 1 };

int RenderCubeSideHud(CBlock* block, CTessellator* t, float x, float y, float z, float cubeSize, int cubeSideId);
int CheckOpenGlError(std::string t);
std::string WideStringToString(std::wstring wideString);
std::wstring ExtractDirectoryName(std::wstring wideString);
bool KeyPressed(int vKey);
