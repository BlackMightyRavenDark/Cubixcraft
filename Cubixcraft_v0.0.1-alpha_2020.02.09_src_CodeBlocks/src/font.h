#include <iostream>
#include <cstring>
#include <stdarg.h>
#include <GL/gl.h>
#include "SOIL/SOIL.h"

const float symbolWidth = 11.0;

GLuint BuildTexturedFont(char* textureFileName);
void KillFont();
void glTextStart();
void glTextEnd();
void glTextOut(int xPos, int yPos, const char* sText, int color);
void DrawTextBkg(int xPos, int yPos, int len, int color);
std::string FormatStr(char* sText, ...);
