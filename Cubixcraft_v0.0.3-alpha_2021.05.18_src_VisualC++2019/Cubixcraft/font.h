#include <iostream>
#include <stdarg.h>
#include <GL/gl.h>
#include <stb_image.h>

const float fontSize = 16.0f;
const float symbolWidthCoef = 1.4f;

GLuint BuildTexturedFont(std::string textureFileName);
void KillFont();
void glTextStart();
void glTextEnd();
void glTextOut(const char* sText, float xPos, float yPos, int color);
void DrawTextBkg(float xPos, float yPos, float xWidth, int color);
float GetStringWidth(std::string textString);
std::string FormatString(char* sText, ...);
