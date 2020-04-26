#include "font.h"
#include "main.h"

GLuint fontTex = 0;
GLuint hFontLists = 0;

/*
 * BuildTexturedFont()
 * Загружает текстурный шрифт.
 * Текстура должна быть размером 256x256 и иметь формат PNG 32 bits с альфа-каналом.
 */
GLuint BuildTexturedFont(char* textureFileName) {
    fontTex = SOIL_load_OGL_texture(textureFileName,
        SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (fontTex) {
        float w = 1.0f / 16.0f;
        hFontLists = glGenLists(256);
        glBindTexture(GL_TEXTURE_2D, fontTex);

        //создаём отдельные листы для каждой буквы
        for (int i = 0; i < 256; i++) {
            float cx = (i % 16) / 16.0f;
            float cy = (i / 16) / 16.0f;
            glNewList(hFontLists + i, GL_COMPILE);
            glBegin(GL_QUADS);
            glTexCoord2f(cx,     1.0f - cy - w);
            glVertex2i(0, 0);
            glTexCoord2f(cx + w, 1.0f - cy - w);
            glVertex2i(16, 0);
            glTexCoord2f(cx + w, 1.0f - cy);
            glVertex2i(16, 16);
            glTexCoord2f(cx,     1.0f - cy);
            glVertex2i(0, 16);
            glEnd();
            glTranslatef(symbolWidth, 0.0f, 0.0f);
            glEndList();
        }
        std::cout << "Loaded font: " << textureFileName << std::endl;
    }
    return fontTex;
}

/*
 * KillFont()
 * Выгружает текстуру шрифта и освобождает память.
 */
void KillFont() {
    if (hFontLists) {
        glDeleteLists(hFontLists, 256);
        hFontLists = 0;
    }
    if (fontTex) {
        glDeleteTextures(1, &fontTex);
        fontTex = 0;
    }
}

/*
 * glTextStart()
 * Устанавливает необходимые параметры для вывода текста и
 * переключает OpenGL в 2D-режим рисования.
 */
void glTextStart() {
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, fontTex);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0.0, (double)glScreenWidth, 0.0, (double)glScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

/*
 * glTextEnd()
 * Завершить вывод текста и переключиться обратно в 3D-режим.
 */
void glTextEnd() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
}

/*
 * glTextOut()
 * Выводит текст на экран.
 */
void glTextOut(int xPos, int yPos, const char* sText, int color) {
    glLoadIdentity();
    float r = (color >> 16 & 0xFF) / 255.0f;
    float g = (color >> 8  & 0xFF) / 255.0f;
    float b = (color       & 0xFF) / 255.0f;
    glColor3f(r, g, b);
    glTranslated(xPos, (double)glScreenHeight - 20.0 - yPos, 0.0);
    glListBase(hFontLists - 32);
    glEnable(GL_TEXTURE_2D);
    glCallLists(strlen(sText), GL_BYTE, sText);
    glDisable(GL_TEXTURE_2D);
}

/*
 * DrawTextBkg()
 * Рисует полупрозрачный прямоугольник.
 */
void DrawTextBkg(int xPos, int yPos, int xWidth, int color) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float r = (color >> 16 & 0xFF) / 255.0f;
    float g = (color >>  8 & 0xFF) / 255.0f;
    float b = (color       & 0xFF) / 255.0f;
    glColor4f(r, g, b, 0.2f);
    glLoadIdentity();
    glBegin(GL_QUADS);
    glVertex3f((GLfloat)xPos,            (GLfloat)(glScreenHeight - yPos - 20.0f), 0.0f);
    glVertex3f((GLfloat)(xPos + xWidth), (GLfloat)(glScreenHeight - yPos - 20.0f), 0.0f);
    glVertex3f((GLfloat)(xPos + xWidth), (GLfloat)(glScreenHeight - yPos),         0.0f);
    glVertex3f((GLfloat)xPos,            (GLfloat)(glScreenHeight - yPos),         0.0f);
    glEnd();
    glDisable(GL_BLEND);
}

std::string FormatStr(char* sText, ...) {
    char text[256];
    va_list vl;
    va_start(vl, sText);
    vsprintf_s(text, sText, vl);
    va_end(vl);
    return text;
}
