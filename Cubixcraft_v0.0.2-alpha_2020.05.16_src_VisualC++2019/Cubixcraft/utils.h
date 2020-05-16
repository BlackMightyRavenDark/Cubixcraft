#include <iostream>
#include <GL/gl.h>
#include <codecvt>

class CBlock;
class CTessellator;

enum class MOUSE_WHEEL_DIRECTION { MW_UP = 0, MW_DOWN = 1 };

int RenderCubeSide2D(CBlock* b, CTessellator* t, float x, float y, float z, float cubeSize, int cubeSide);
int CheckOpenGlError(std::string t);
std::string WideStringToString(std::wstring wideString);
std::wstring ExtractDirectoryName(std::wstring s);
bool KeyPressed(int vKey);
