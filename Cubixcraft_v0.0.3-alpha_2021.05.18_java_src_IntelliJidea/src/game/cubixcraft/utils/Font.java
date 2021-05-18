package game.cubixcraft.utils;

import game.cubixcraft.CubixCraft;

import java.nio.charset.Charset;

import static org.lwjgl.opengl.GL11.*;

public class Font {
    private int textureId;
    private final float fontSize = 16.0f;
    private final float widthCoef = 1.4f;
    private final float[] charWidths;

    public Font(String fileName) {
        /*
         * Загружает текстурный шрифт.
         * Текстура должна быть в формате PNG 32 bits с альфа-каналом
         * и содержать символы таблицы ASCII, 16 штук по ширине и 16 по высоте.
         */
        System.out.println("Loading font...");

        textureId = Textures.loadTexture(fileName);
        charWidths = new float[256];
        for (int i = 0; i < 256; i++) {
            charWidths[i] = fontSize / widthCoef;
        }
        charWidths['l'] = fontSize / 2.0f / widthCoef;
    }

    public void start() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, CubixCraft.getWindowWidth(), CubixCraft.getWindowHeight(), 0.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        glDisable(GL_DEPTH_TEST);

        glBindTexture(GL_TEXTURE_2D, textureId);
    }

   public void end() {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
    }

    public void drawString(String textString, float xPos, float yPos, int color) {
        float r = (color >> 16 & 0xFF) / 255.0f;
        float g = (color >>  8 & 0xFF) / 255.0f;
        float b = (color       & 0xFF) / 255.0f;
        glColor3f(r, g, b);

        glLoadIdentity();

        byte[] chars = textString.getBytes(Charset.forName("cp1251"));
        final int glyphCount = 16;
        final float glyphSize = 1.0f / (float)glyphCount;
        float xOffset = 0.0f;

        glEnable(GL_TEXTURE_2D);

        glBegin(GL_QUADS);
        for (int i = 0; i < chars.length; i++) {
            char charId = (char)(chars[i] - 32); // or int charId = (chars[i] < 0 ? chars[i] + 256 : chars[i]) - 32;
            float texPosX = (charId % glyphCount) * glyphSize;
            float texPosY = (float)(charId / glyphCount) / (float)glyphCount;
            float u1 = texPosX + glyphSize;
            float v1 = texPosY + glyphSize;

            glTexCoord2f(texPosX, texPosY);
            glVertex2f(xPos + xOffset, yPos);
            glTexCoord2f(u1, texPosY);
            glVertex2f(xPos + xOffset + fontSize, yPos);
            glTexCoord2f(u1, v1);
            glVertex2f(xPos + xOffset + fontSize, yPos + fontSize);
            glTexCoord2f(texPosX, v1);
            glVertex2f(xPos + xOffset, yPos + fontSize);

            xOffset += charWidths[chars[i]];
        }
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    public void drawBkg(float xPos, float yPos, float width, int color) {
        float r = (color >> 16 & 0xFF) / 255.0f;
        float g = (color >> 8  & 0xFF) / 255.0f;
        float b = (color       & 0xFF) / 255.0f;
        glColor4f(r, g, b, 0.2f);

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_QUADS);
        glVertex2f(xPos, yPos);
        glVertex2f(xPos + width, yPos);
        glVertex2f(xPos + width, yPos + fontSize + 2.0f);
        glVertex2f(xPos, yPos + fontSize + 2.0f);
        glEnd();

        glDisable(GL_BLEND);
    }

    public float getStringWidth(String textString) {
        char[] chars = textString.toCharArray();
        float n = 0.0f;
        for (int i = 0; i < chars.length; i++) {
            n += charWidths[chars[i]];
        }
        return n;
    }

    public int getTextureId() {
        return textureId;
    }

    public void destroy() {
        if (textureId > 0) {
            glDeleteTextures(textureId);
            textureId = 0;
        }
    }

}
