#include "textures.h"

GLuint textureGrass = 0;

GLuint LoadTexture(std::string fn){
    GLuint tex = SOIL_load_OGL_texture((char*)fn.c_str(),
        SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
    if (tex){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        std::cout << "Texture loaded: " << fn << ", ID: " << tex << std::endl;
        glDisable(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Error loading " << fn << std::endl;
    }
    return tex;
}

void LoadGameTextures(){
    std::cout << "Loading textures..." << std::endl;
    textureGrass = LoadTexture("textures/grass.png");
}

void KillGameTextures(){
    std::cout << "Unloading textures..." << std::endl;
    glDeleteTextures(1, &textureGrass);
}
