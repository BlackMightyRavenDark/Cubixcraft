#include <iostream>
#include <GL/gl.h>

//���������� �����
extern GLuint textureBase;

GLuint LoadTexture(std::string textureFileName);
void LoadGameTextures(); 
void FreeGameTextures();
