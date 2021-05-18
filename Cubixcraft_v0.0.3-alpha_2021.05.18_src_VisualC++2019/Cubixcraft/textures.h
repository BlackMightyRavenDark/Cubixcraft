#include <iostream>
#include <GL/gl.h>

//текстурный атлас
extern GLuint textureBase;

GLuint LoadTexture(std::string textureFileName);
void LoadGameTextures(); 
void FreeGameTextures();
