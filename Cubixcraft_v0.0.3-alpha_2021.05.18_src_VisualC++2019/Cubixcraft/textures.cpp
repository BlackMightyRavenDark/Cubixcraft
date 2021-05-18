#include "textures.h"
#include "main.h"
#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint textureBase = 0;

GLuint LoadTexture(std::string textureFileName)
{
    int textureWidth;
    int textureHeight;
    int textureColorComponentCount;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load(textureFileName.c_str(), &textureWidth, &textureHeight, &textureColorComponentCount, 0);
    if (image)
    {
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int format = textureColorComponentCount == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(image);

        std::cout << "Texture loaded: " << textureFileName << ", ID: " << textureId << std::endl;

        return textureId;
    }
    else
    {
        std::cout << "Can't load texture: " << textureFileName << std::endl;
    }

    return 0;
}

void LoadGameTextures() 
{
    std::cout << "Loading textures..." << std::endl;
    textureBase = LoadTexture(WideStringToString(selfDir) + "\\textures\\base.png");
}

void FreeGameTextures() 
{
    std::cout << "Unloading textures..." << std::endl;
    glDeleteTextures(1, &textureBase);
}
