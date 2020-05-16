#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SOIL.h>

extern GLuint textureBase;

GLuint LoadTexture(std::string fn);
void LoadGameTextures(); 
void KillGameTextures();
