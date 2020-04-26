#include "font.h"
#include "main.h"

GLuint fontTex = 0;
GLuint hFontLists = 0;

GLuint BuildTexturedFont(char* textureFileName){
    glEnable(GL_TEXTURE_2D);
    fontTex = SOIL_load_OGL_texture(textureFileName,
        SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (fontTex){
        float w = 1.0 / 16.0;
        hFontLists = glGenLists(256);
        glBindTexture(GL_TEXTURE_2D, fontTex);
        for (int i = 0; i < 256; i++){
            float cx = (i % 16) / 16.0;
            float cy = (i / 16) / 16.0;
            glNewList(hFontLists + i, GL_COMPILE);
            glBegin(GL_QUADS);
            glTexCoord2f(cx,     1.0 - cy - w);
            glVertex2i(0, 0);
            glTexCoord2f(cx + w, 1.0 - cy - w);
            glVertex2i(16, 0);
            glTexCoord2f(cx + w, 1.0 - cy);
            glVertex2i(16, 16);
            glTexCoord2f(cx,     1.0 - cy);
            glVertex2i(0, 16);
            glEnd();
            glTranslatef(symbolWidth, 0.0, 0.0);
            glEndList();
        }
        std::cout << "Loaded font: " << textureFileName << std::endl;
    }
    glDisable(GL_TEXTURE_2D);
    return fontTex;
}

void KillFont(){
    if (hFontLists){
        glDeleteLists(hFontLists, 256);
        hFontLists = 0;
    }
    if (fontTex){
        glDeleteTextures(1, &fontTex);
        fontTex = 0;
    }
}

void glTextStart(){
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0);
    glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTex);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0.0, (double)glScreenWidth, 0.0, (double)glScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();


}

void glTextEnd(){
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
}

void glTextOut(int xPos, int yPos, const char* sText, int color){
    glLoadIdentity();
    glColor3f((color >> 16 & 0xFF) / 255.0, (color >> 8 & 0xFF) / 255.0, (color & 0xFF) / 255.0);
    glTranslated(xPos, (double)glScreenHeight - 20.0 - yPos, 0.0);
    glListBase(hFontLists - 32);
    glEnable(GL_TEXTURE_2D);
    glCallLists(strlen(sText), GL_BYTE, sText);
    glDisable(GL_TEXTURE_2D);
}

void DrawTextBkg(int xPos, int yPos, int len, int color){
    float r = (color >> 16 & 0xFF) / 255.0;
    float g = (color >> 8 & 0xFF) / 255.0;
    float b = (color & 0xFF) / 255.0;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, 0.2);
    glLoadIdentity();
    glBegin(GL_QUADS);
    glVertex3f(xPos, glScreenHeight - yPos - 20.0, 0.0);
    glVertex3f(xPos + len, glScreenHeight - yPos - 20.0, 0.0);
    glVertex3f(xPos + len, glScreenHeight - yPos, 0.0);
    glVertex3f(xPos, glScreenHeight - yPos, 0.0);
    glEnd();
    glDisable(GL_BLEND);
}

std::string FormatStr(char* sText, ...){
    char text[256];
    va_list vl;
    va_start(vl, sText);
    vsprintf(text, sText, vl);
    va_end(vl);
    return text;
}
