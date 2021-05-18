#include "font.h"
#include "main.h"
#include "textures.h"

GLuint fontTex = 0;
GLuint glyphLists = 0;

/*
 * BuildTexturedFont()
 * Загружает текстурный шрифт.
 * Текстура должна быть в формате PNG 32 bits с альфа-каналом
 * и содержать символы таблицы ASCII, 16 штук по ширине и 16 по высоте.
 */
GLuint BuildTexturedFont(std::string textureFileName) 
{
    fontTex = LoadTexture(textureFileName);
    if (fontTex)
    {
        const int symbolsInRow = 16;
        float glyphSize = 1.0f / (float)symbolsInRow;
        const float shiftX = fontSize / symbolWidthCoef;

        //создаём отдельные листы для каждой буквы
        glyphLists = glGenLists(256);

        for (int i = 0; i < 256; i++)
        {
            float glyphPosX = (i % symbolsInRow) / (float)symbolsInRow;
            float glyphPosY = (i / symbolsInRow) / (float)symbolsInRow;
            float u1 = glyphPosX + glyphSize;
            float v1 = 1.0f - glyphPosY;

            glNewList(glyphLists + i, GL_COMPILE);

            glBegin(GL_QUADS);

            glTexCoord2f(glyphPosX, v1 - glyphSize);
            glVertex2f(0.0f, 0.0f);
            
            glTexCoord2f(u1, v1 - glyphSize);
            glVertex2f(fontSize, 0.0f);

            glTexCoord2f(u1, v1);
            glVertex2f(fontSize, fontSize);

            glTexCoord2f(glyphPosX, v1);
            glVertex2f(0.0f, fontSize);

            glEnd();

            glTranslatef(shiftX, 0.0f, 0.0f);

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
void KillFont() 
{
    if (glyphLists) 
    {
        glDeleteLists(glyphLists, 256);
        glyphLists = 0;
    }
    if (fontTex) 
    {
        glDeleteTextures(1, &fontTex);
        fontTex = 0;
    }
}

/*
 * glTextStart()
 * Устанавливает необходимые параметры для вывода текста
 * и переключает OpenGL в 2D-режим рисования (ортогональная проекция).
 */
void glTextStart() 
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, (double)viewportWidth, 0.0, (double)viewportHeight, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, fontTex);
}

/*
 * glTextEnd()
 * Завершает вывод текста.
 */
void glTextEnd() 
{
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
}

/*
 * glTextOut()
 * Выводит текст на экран.
 */
void glTextOut(const char* sText, float xPos, float yPos, int color)
{
    float r = (color >> 16 & 0xFF) / 255.0f;
    float g = (color >> 8  & 0xFF) / 255.0f;
    float b = (color       & 0xFF) / 255.0f;
    glColor3f(r, g, b);

    glLoadIdentity();
    glTranslatef(xPos, (float)viewportHeight - fontSize - yPos - 4.0f, 0.0f);

    glListBase(glyphLists - 32);
    glEnable(GL_TEXTURE_2D);
    glCallLists(strlen(sText), GL_UNSIGNED_BYTE, sText);
    glDisable(GL_TEXTURE_2D);
}

/*
 * DrawTextBkg()
 * Рисует полупрозрачный прямоугольник.
 */
void DrawTextBkg(float xPos, float yPos, float xWidth, int color)
{
    float r = (color >> 16 & 0xFF) / 255.0f;
    float g = (color >>  8 & 0xFF) / 255.0f;
    float b = (color       & 0xFF) / 255.0f;
    glColor4f(r, g, b, 0.2f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glLoadIdentity();
    
    glBegin(GL_QUADS);
    glVertex2f(xPos,          (float)viewportHeight - yPos - 20.0f);
    glVertex2f(xPos + xWidth, (float)viewportHeight - yPos - 20.0f);
    glVertex2f(xPos + xWidth, (float)viewportHeight - yPos        );
    glVertex2f(xPos,          (float)viewportHeight - yPos        );
    glEnd();

    glDisable(GL_BLEND);
}

float GetStringWidth(std::string textString)
{
    return (textString.length() * fontSize) / symbolWidthCoef;
}

std::string FormatString(char* sText, ...)
{
    char text[256];
    va_list vl;
    va_start(vl, sText);
    vsprintf_s(text, sText, vl);
    va_end(vl);
    return text;
}
