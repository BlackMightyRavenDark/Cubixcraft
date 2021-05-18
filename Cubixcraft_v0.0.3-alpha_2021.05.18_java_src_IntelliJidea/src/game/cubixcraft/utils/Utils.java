package game.cubixcraft.utils;

import game.cubixcraft.CubixCraft;
import game.cubixcraft.Tessellator;
import game.cubixcraft.block.Block;
import org.lwjgl.BufferUtils;

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
     * RenderCubeSideHud()
     * Рисует указанную сторону блока в HUD.
     * Для правильного вывода, необходимо предварительно переключиться в ортогональную проекцию,
     * отключить GL_LIGHTING и включить GL_DEPTH_TEST.
     */
    public static int RenderCubeSideHud(Block block, Tessellator t, float x, float y, float z, float cubeSize, int cubeSideId) {
        Vec2i pt = block.getTextureCoordinates(cubeSideId);

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

        int sidesRendered = 0;
        switch (cubeSideId) {
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
        return sidesRendered;
    }

    public static int checkOpenGlError(String t) {
        int errorCode = glGetError();
        switch (errorCode) {
            case GL_NO_ERROR:
                //Ошибок нет. Ничего не делаем.
                break;
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
            default:
                System.out.println("!!!Unknown OpenGL error in " + t);
                break;
        }
        return errorCode;
    }

    public static ByteBuffer loadImageAsByteBuffer(String fileName) {
        try {
            BufferedImage image;
            if (fileName.startsWith("/")) {
                URL url = CubixCraft.class.getResource(fileName);
                if (url == null)
                    throw new IllegalArgumentException();
                image = ImageIO.read(url);
            } else {
                image = ImageIO.read(new File(fileName));
            }
            int componentCount = image.getData().getNumBands();
            if (componentCount != 3 && componentCount != 4)
                throw new IndexOutOfBoundsException("!!!Unsupported image format! " + fileName);

            int[] rawPixels = Textures.swapRedBlue(image);

            int w = image.getWidth();
            int h = image.getHeight();

            ByteBuffer pixels = BufferUtils.createByteBuffer(w * h * 4);
            pixels.asIntBuffer().put(rawPixels);

            return pixels;
        } catch (IOException | IllegalArgumentException e) {
            System.out.println("Can not load file: " + fileName);
            e.printStackTrace();
            return null;
        }
    }

    public static String divSize(long n) {
        final int KB = 1000;
        final int MB = 1000000;
        final int GB = 1000000000;
        final long TB = 1000000000000L;

        long b = n % KB;
        long kb = (n % MB) / KB;
        long mb = (n % GB) / MB;
        long gb = (n % TB) / GB;

        if (n >= 0 && n < KB)
            return String.format("%01d b", b);
        if (n >= KB && n < MB)
            return String.format("%01d,%03d KB", kb, b);
        if (n >= MB && n < GB)
            return String.format("%01d,%03d MB", mb, kb);
        if (n >= GB && n < TB)
            return String.format("%01d,%03d GB", gb, mb);

        return String.format("%01d %03d %03d %03d bytes", gb, mb, kb, b);
    }

}
