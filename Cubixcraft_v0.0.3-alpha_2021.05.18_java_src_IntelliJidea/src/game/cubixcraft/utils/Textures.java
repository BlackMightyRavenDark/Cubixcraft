package game.cubixcraft.utils;

import org.lwjgl.*;
import javax.imageio.*;

import org.lwjgl.util.glu.*;
import java.io.*;
import java.awt.image.*;
import java.net.URL;
import java.nio.*;

import static org.lwjgl.opengl.GL11.*;

public class Textures {

    //текстурный атлас
    public static int textureBase;

    /*
     * Если путь или имя файла начинается со слеша, то файл загружается из ресурсов.
     * Иначе - файл загружается с диска.
     * Если в начале имени файла указан слеш и мы находимся в IDE,
     * то корнем является директория <project_dir>/src.
     * Если программа уже скомпилирована и упакована в JAR-файл,
     * то корнем будет являться корень JAR-файла.
     * Если в начале имени файла слеш не указан и имя файла не содержит
     * абсолютный путь, то система будет искать файл относительно
     * директории с проектом (если мы находимся в IDE) или
     * относительно текущей директории, если мы
     * запустили уже скомпилированный JAR-файл.
     * Под "текущей директорией" подразумевается та директория,
     * в которой находился пользователь при запуске JAR-файла.
     * Ее можно установить принудительно с помощью ключа -Duser.dir=<directory>
     */

    public static int loadTexture(String fileName) {
        try {
            BufferedImage img;
            if (fileName.startsWith("/")) {
                URL url = Textures.class.getResource(fileName);
                if (url == null)
                    throw new FileNotFoundException(fileName);
                img = ImageIO.read(url);
            }
            else
            {
                img = ImageIO.read(new File(fileName));
            }

            int componentCount = img.getData().getNumBands();
            if (componentCount != 3 && componentCount != 4) {
                throw new IndexOutOfBoundsException("!!!Unsupported image format! " + fileName);
            }
            IntBuffer ib = BufferUtils.createIntBuffer(1);
            glGenTextures(ib);
            int texId = ib.get(0);

            glBindTexture(GL_TEXTURE_2D, texId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            int[] rawPixels = swapRedBlue(img);

            int w = img.getWidth();
            int h = img.getHeight();

            ByteBuffer pixels = BufferUtils.createByteBuffer(w * h * 4);
            pixels.asIntBuffer().put(rawPixels);

            int format = componentCount == 4 ? GL_RGBA : GL_RGB;
            GLU.gluBuild2DMipmaps(GL_TEXTURE_2D, format, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glBindTexture(GL_TEXTURE_2D, 0);

            System.out.println(fileName + ", ID: " + texId);
            return texId;
        }
        catch (FileNotFoundException e) {
            System.out.println("!!!File not found: " + fileName);
            e.printStackTrace();
            return 0;
        }
        catch (IOException | IllegalArgumentException e) {
            System.out.println("!!!Can not load file " + fileName);
            e.printStackTrace();
            return 0;
        }
    }

    public static int[] swapRedBlue(BufferedImage image) {
        //Для правильного отображения текстуры в OpenGL, переворачиваем красный, зелёный и синий каналы.
        //По факту, местами меняются только красный и синий.
        int w = image.getWidth();
        int h = image.getHeight();
        int[] pixels = new int[w * h];
        image.getRGB(0, 0, w, h, pixels, 0, w);
        for (int i = 0; i < pixels.length; ++i) {
            int a = pixels[i] >> 24 & 0xFF;
            int r = pixels[i] >> 16 & 0xFF;
            int g = pixels[i] >>  8 & 0xFF;
            int b = pixels[i]       & 0xFF;
            pixels[i] = a << 24 | b << 16 | g << 8 | r;
        }
        return pixels;
    }

    public static void loadGameTextures() {
        System.out.println("Loading textures...");
        textureBase = loadTexture("/textures/base.png");
    }

    public static void deleteGameTextures() {
        System.out.println("Unloading textures...");
        glDeleteTextures(textureBase);
    }
}
