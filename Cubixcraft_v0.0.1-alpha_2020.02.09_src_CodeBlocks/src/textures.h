#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include "SOIL/soil.h"

extern GLuint textureGrass;

GLuint LoadTexture(std::string fn);
void LoadGameTextures();
void KillGameTextures();
