package game.cubixcraft;

import org.lwjgl.*;
import javax.imageio.*;

import org.lwjgl.util.glu.*;
import java.io.*;
import java.awt.image.*;
import java.net.URL;
import java.nio.*;

import static org.lwjgl.opengl.GL11.*;

public class Textures {

    public static int textureGrass;

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
     * относительно директории с JAR-файлом, если мы
     * запустили уже скомпилированный JAR-файл.
     */

    public static int loadTexture(String fileName){
        try {
            BufferedImage img;
            if (fileName.startsWith("/")) {
                URL url = Textures.class.getResource(fileName);
                img = ImageIO.read(url);
            }
            else
            {
                img = ImageIO.read(new File(fileName));
            }
            IntBuffer ib = BufferUtils.createIntBuffer(1);
            glGenTextures(ib);
            glBindTexture(GL_TEXTURE_2D, ib.get(0));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            int w = img.getWidth();
            int h = img.getHeight();
            ByteBuffer pixels = BufferUtils.createByteBuffer(w * h * 4);
            int[] rawPixels = new int[w * h];
            img.getRGB(0,0, w, h, rawPixels, 0, w);

            //Для правильного отображения текстуры в OpenGL, переворачиваем красный, зелёный и синий каналы.
            //По факту, местами меняются только красный и синий.
            for (int i = 0; i < rawPixels.length; ++i) {
                int a = rawPixels[i] >> 24 & 0xFF;
                int r = rawPixels[i] >> 16 & 0xFF;
                int g = rawPixels[i] >> 8  & 0xFF;
                int b = rawPixels[i]       & 0xFF;
                rawPixels[i] = a << 24 | b << 16 | g << 8 | r;
            }
            pixels.asIntBuffer().put(rawPixels);
            GLU.gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w,h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            System.out.println(fileName + ", ID: " + ib.get(0));
            return ib.get(0);
        }
        catch (IOException e) {
            System.out.println(fileName);
            e.printStackTrace();
            return 0;
        }
    }

    public static void loadGameTextures(){
        System.out.println("Loading textures...");
        textureGrass = loadTexture("/textures/grass.png");
    }

    public static void deleteGameTextures(){
        System.out.println("Unloading textures...");
        glDeleteTextures(textureGrass);
    }
}
