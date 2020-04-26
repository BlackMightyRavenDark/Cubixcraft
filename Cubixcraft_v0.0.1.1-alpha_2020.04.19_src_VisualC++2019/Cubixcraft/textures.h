#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SOIL.h>

extern GLuint textureGrass;

GLuint LoadTexture(std::string fn);
void LoadGameTextures(); 
void KillGameTextures();
