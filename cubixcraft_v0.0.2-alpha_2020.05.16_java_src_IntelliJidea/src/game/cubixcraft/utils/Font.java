package game.cubixcraft.utils;

import game.cubixcraft.CubixCraft;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;

import static org.lwjgl.opengl.GL11.*;

public class Font {

    private int imageWidth;
    private int imageHeight;
    private int textureId;
    private float fontSize = 16.0f;
    private float widthCoef = 1.5f;
    private float[] charWidths;

    public Font(String fileName) {
        /*
         * Загружает текстурный шрифт.
         * Текстура должна быть размером 256x256 и иметь формат PNG 32 bits с альфа-каналом.
         */
        System.out.println("Loading font...");
        URL url;
        if (fileName.startsWith("/")) {
           url = Font.class.getResource(fileName);
        }
        else
        {
            try {
                url = new File(fileName).toURI().toURL();
            } catch (MalformedURLException e) {
                e.printStackTrace();
                url = null;
            }
        }
        if (url == null) {
            System.out.println("Can not find " + fileName);
            throw new RuntimeException("Can not find " + fileName);
        }
        BufferedImage img;
        try {
            img = ImageIO.read(url);
        } catch (IOException e) {
            System.out.println("Can not load " + fileName);
            e.printStackTrace();
            throw new RuntimeException("Can not load " + fileName);
        }
        imageWidth = img.getWidth();
        imageHeight = img.getHeight();
        textureId = Textures.loadTexture(fileName);
        charWidths = new float[256];
    }

    public void start() {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        glDisable(GL_DEPTH_TEST);

        glBindTexture(GL_TEXTURE_2D, textureId);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, CubixCraft.windowWidth, CubixCraft.windowHeight, 0.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

   public void end() {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
    }

    public void drawString(int xPos, int yPos, String textString, int color) {
        for (int i = 0; i < 256; i++) {
            charWidths[i] = fontSize / widthCoef;
        }
        charWidths['l'] = fontSize / 2.0f / widthCoef;
        float r = (color >> 16 & 0xFF) / 255.0f;
        float g = (color >>  8 & 0xFF) / 255.0f;
        float b = (color       & 0xFF) / 255.0f;
        glColor3f(r, g, b);
        char[] chars = textString.toCharArray();
        int glyphSize = imageWidth / 16;
        float xOffset = 0.0f;
        glEnable(GL_TEXTURE_2D);
        for (int i = 0; i < chars.length; i++) {
            float texPosX = chars[i] % 16 * glyphSize / (float)imageWidth;
            float texPosY = (chars[i] - 32) / 16 / 16.0f;
            float u1 = texPosX + glyphSize / (float)imageWidth;
            float v1 = texPosY + glyphSize / (float)imageWidth;
            glBegin(GL_QUADS);
            glTexCoord2f(texPosX, texPosY);
            glVertex3f(xPos + xOffset, yPos, 0.0f);
            glTexCoord2f(u1, texPosY);
            glVertex3f(xPos + xOffset + fontSize, yPos, 0.0f);
            glTexCoord2f(u1, v1);
            glVertex3f(xPos + xOffset + fontSize, yPos + fontSize, 0.0f);
            glTexCoord2f(texPosX, v1);
            glVertex3f(xPos + xOffset, yPos + fontSize, 0.0f);
            glEnd();
            xOffset += charWidths[chars[i]];
        }
    }

    public void drawBkg(int xPos, int yPos, int wdt, int color) {
        float r = (color >> 16 & 0xFF) / 255.0f;
        float g = (color >> 8  & 0xFF) / 255.0f;
        float b = (color       & 0xFF) / 255.0f;
        glColor4f(r, g, b, 0.2f);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glVertex2f(xPos, yPos);
        glVertex2f(xPos + wdt, yPos);
        glVertex2f(xPos + wdt, yPos + 18.0f);
        glVertex2f(xPos, yPos + 18.0f);
        glEnd();
        glDisable(GL_BLEND);
    }

    public int width(String t) {
        char[] c = t.toCharArray();
        float n = 0.0f;
        for (int i = 0; i < c.length; i++) {
            n += charWidths[c[i]];
        }
        return (int)n;
    }

    public int getTextureId() {
        return textureId;
    }

    public void destroy(){
        if (textureId > 0){
            glDeleteTextures(textureId);
            textureId = 0;
        }
    }

}
