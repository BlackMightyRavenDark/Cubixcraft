package game.cubixcraft.utils;

import game.cubixcraft.CubixCraft;
import game.cubixcraft.Tessellator;
import game.cubixcraft.block.Block;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.nio.ByteBuffer;

import static game.cubixcraft.block.Block.*;

import static org.lwjgl.opengl.GL11.*;

public class Utils {
    
    public static final int MOUSE_BUTTON_LEFT = 0;
    public static final int MOUSE_BUTTON_RIGHT = 1;
    public static final int MOUSE_WHEEL_UP = 0;
    public static final int MOUSE_WHEEL_DOWN = 1;

    /*
     * RenderCubeSide2D()
     * Рисует указанную сторону блока в HUD.
     * Для правильного вывода, необходимо отключить GL_LIGHTING и включить GL_DEPTH_TEST.
     */
    public static int RenderCubeSide2D(Block b, Tessellator t, float x, float y, float z, float cubeSize, int cubeSide) {
        int sidesRendered = 0;
        Vec2i pt = b.getTextureCoordinates(cubeSide);

        /*
         * u0, v0 - координаты x,y левой верхней точки текстуры текущей стороны блока в текстурном атласе.
         * u1, v1 - координаты x,y правой нижней точки текстуры текущей стороны блока в текстурном атласе.
         * Из текстурного атласа будет выделена область между этими точками
         * и наложена на текущую сторону блока в качестве ее текстуры.
         * 16 - это количество текстур в атласе по вертикали и горизонтали.
         * Текстурный атлас используется для сокращения количества переключений
         * между текстурами и, как следствие, ускорения рендеринга и повышения FPS.
         */
        float u0 = pt.x / 16.0f;
        float v0 = pt.y / 16.0f;
        float u1 = u0 + 1.0f / 16.0f;
        float v1 = v0 + 1.0f / 16.0f;

        switch (cubeSide) {
            case BLOCK_SIDE_BACK:
                t.addTextureVertexQuad(u0, v1, x, y, z);
                t.addTextureVertexQuad(u1, v1, x + cubeSize, y, z);
                t.addTextureVertexQuad(u1, v0, x + cubeSize, y + cubeSize, z);
                t.addTextureVertexQuad(u0, v0, x, y + cubeSize, z);
                sidesRendered++;
                break;

            case BLOCK_SIDE_FRONT:
                t.addTextureVertexQuad(u0, v1, x, y, z + cubeSize);
                t.addTextureVertexQuad(u1, v1, x + cubeSize, y, z + cubeSize);
                t.addTextureVertexQuad(u1, v0, x + cubeSize, y + cubeSize, z + cubeSize);
                t.addTextureVertexQuad(u0, v0, x, y + cubeSize, z + cubeSize);
                sidesRendered++;
                break;

            case BLOCK_SIDE_LEFT:
                t.addTextureVertexQuad(u0, v1, x, y, z);
                t.addTextureVertexQuad(u1, v1, x, y, z + cubeSize);
                t.addTextureVertexQuad(u1, v0, x, y + cubeSize, z + cubeSize);
                t.addTextureVertexQuad(u0, v0, x, y + cubeSize, z);
                sidesRendered++;
                break;

            case BLOCK_SIDE_RIGHT:
                t.addTextureVertexQuad(u0, v1, x + cubeSize, y, z);
                t.addTextureVertexQuad(u1, v1, x + cubeSize, y, z + cubeSize);
                t.addTextureVertexQuad(u1, v0, x + cubeSize, y + cubeSize, z + cubeSize);
                t.addTextureVertexQuad(u0, v0, x + cubeSize, y + cubeSize, z);
                sidesRendered++;
                break;

            case BLOCK_SIDE_BOTTOM:
                t.addTextureVertexQuad(u0, v1, x, y, z);
                t.addTextureVertexQuad(u1, v1, x + cubeSize, y, z);
                t.addTextureVertexQuad(u1, v0, x + cubeSize, y, z + cubeSize);
                t.addTextureVertexQuad(u0, v0, x, y, z + cubeSize);
                sidesRendered++;
                break;

            case BLOCK_SIDE_TOP:
                t.addTextureVertexQuad(u0, v0, x, y + cubeSize, z);
                t.addTextureVertexQuad(u1, v0, x + cubeSize, y + cubeSize, z);
                t.addTextureVertexQuad(u1, v1, x + cubeSize, y + cubeSize, z + cubeSize);
                t.addTextureVertexQuad(u0, v1, x, y + cubeSize, z + cubeSize);
                sidesRendered++;
                break;
        }
        return sidesRendered++;
    }

    public static int CheckOpenGlError(String t)
    {
        int errorCode = glGetError();
        switch (errorCode)
        {
            case GL_INVALID_ENUM:
                System.out.println("!!!OpenGL error in " + t + ": GL_INVALID_ENUM");
                break;
            case GL_INVALID_VALUE:
                System.out.println("!!!OpenGL error in " + t + ": GL_INVALID_VALUE");
                break;
            case GL_INVALID_OPERATION:
                System.out.println("!!!OpenGL error in " + t + ": GL_INVALID_OPERATION");
                break;
            case GL_STACK_OVERFLOW:
                System.out.println("!!!OpenGL error in " + t + ": GL_STACK_OVERFLOW");
                break;
            case GL_STACK_UNDERFLOW:
                System.out.println("!!!OpenGL error in " + t + ": GL_STACK_UNDERFLOW");
                break;
            case GL_OUT_OF_MEMORY:
                System.out.println("!!!OpenGL error in " + t + ": GL_OUT_OF_MEMORY");
                break;
        }
        return errorCode;
    }

    public static ByteBuffer loadImageAsByteBuffer(String fn) {
        try {
            BufferedImage image;
            if (fn.startsWith("/")) {
                URL url = CubixCraft.class.getResource(fn);
                if (url == null) {
                    System.out.println("Can not load file: " + fn);
                    return null;
                }
                image = ImageIO.read(url);
            } else {
                image = ImageIO.read(new File(fn));
            }
            int w = image.getWidth();
            int h = image.getHeight();
            byte[] buffer = new byte[w * h * 4];
            int counter = 0;
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    int colorSpace = image.getRGB(x, y);
                    buffer[counter] = (byte) ((colorSpace << 8) >> 24);
                    buffer[counter + 1] = (byte) ((colorSpace << 16) >> 24);
                    buffer[counter + 2] = (byte) colorSpace;
                    buffer[counter + 3] = (byte) (colorSpace >> 24);
                    counter += 4;
                }
            }
            return ByteBuffer.wrap(buffer);
        } catch (IOException | IllegalArgumentException e) {
            System.out.println("Can not load file: " + fn);
            e.printStackTrace();
            return null;
        }
    }

}
