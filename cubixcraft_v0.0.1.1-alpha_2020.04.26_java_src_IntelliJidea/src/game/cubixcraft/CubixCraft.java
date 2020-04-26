package game.cubixcraft;

import org.lwjgl.LWJGLException;
import org.lwjgl.input.Keyboard;
import org.lwjgl.input.Mouse;
import org.lwjgl.opengl.Display;
import org.lwjgl.opengl.DisplayMode;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.nio.ByteBuffer;

import static org.lwjgl.opengl.GL11.*;

public class CubixCraft {
    public static int windowWidth = 640;
    public static int windowHeight = 480;
    public Player player;
    public static GameTimer gameTimer;
    public World world;
    public WorldRenderer worldRenderer;
    public ObjectSelector objectSelector;
    private Font font;
    public int sceneFps = 0;
    private String title = "Cubixcraft 0.0.1.1-alpha | Java edition";
    boolean running;

    public CubixCraft(){
        try {
            Display.setDisplayMode(new DisplayMode(windowWidth, windowHeight));
            Display.setTitle(title);
            Display.setResizable(true);
            ByteBuffer[] byteBuffer = new ByteBuffer[]{loadImageAsByteBuffer("/textures/tree.png")};
            if (byteBuffer[0] != null){
                Display.setIcon(byteBuffer);
            }
            else
            {
                System.out.println("Can not set window icon!");
            }
            Display.create();
            Keyboard.create();
            Mouse.create();
        } catch (LWJGLException ex) {
            System.out.println("Fatal LWJGL error!");
            ex.printStackTrace();
            System.exit(1);
        }

        font = new Font("/textures/font.png");

        Textures.loadGameTextures();

        world = new World(256, 256, 64, "world1");
        if (!world.load()) {
            world.generateMap();
        }
        worldRenderer = new WorldRenderer(world);
        player = new Player(world, 0.0f, world.sizeZ,0.0f,
                135.0f,0.0f,0.0f);
        objectSelector = new ObjectSelector(world, player);
        gameTimer = new GameTimer(30.0f);

        glClearColor(0.5f, 0.8f, 1.0f, 1.0f);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        long lastTime = System.currentTimeMillis();
        int frames = 0;
        running = true;

        while (running) {
            if (Display.isCloseRequested()) {
                running = false;
                break;
            }

            //освободить мышь, если окно потеряло фокус
            if (!Display.isActive() && Mouse.isGrabbed()) {
                Mouse.setGrabbed(false);
                Mouse.setCursorPosition(windowWidth / 2, windowHeight / 2);
                System.out.println("Window focus was lost! Mouse is released.");
            }

            //обработка клавиатуры и мыши
            handleUserInput();

            //Выполняется при изменении размеров окна
            if (Display.wasResized()) {
                resizeGlWindow(Display.getWidth(), Display.getHeight());
            }

            gameTimer.advanceTime();

            player.tick();

            renderScene();

            frames++;
            if (System.currentTimeMillis() >= lastTime + 1000L) {
                sceneFps = frames;
                lastTime = System.currentTimeMillis();
                frames = 0;
            }

            Display.update();
        }

        destroy();
    }

    private void handleUserInput(){
        //обработка нажатия клавиш на клавиатуре
        while (Keyboard.next()) {
            if (Keyboard.isKeyDown(Keyboard.KEY_ESCAPE) || Keyboard.isKeyDown(Keyboard.KEY_RETURN)) {
                if (Mouse.isGrabbed()) {
                    grabMouse(false);
                } else {
                    running = false;
                    return;
                }
            }

            if (Keyboard.isKeyDown(Keyboard.KEY_R)){
                player.reset();
                System.out.println("Player is respawned");
            }
        }

        //обработка нажатия кнопок мыши
        while (Mouse.next()){
            //нажата левая кнопка
            if (Mouse.getEventButton() == 0 && Mouse.getEventButtonState()){
                if (Mouse.isGrabbed()){
                    /*
                     * Если игрок смотрит на блок, то как минимум одна сторона
                     * этого блока всегда свободна. С этой стороны игрок может
                     * поставить новый блок. Но сначала необходимо проверить,
                     * не стоит ли на этом месте сам игрок, чтобы не дать
                     * поставить блок на самого себя.
                     */
                    if (objectSelector.hitsCount > 0) {
                        if (world.isBlockInWorld(objectSelector.hitResult.xNear,
                                objectSelector.hitResult.yNear, objectSelector.hitResult.zNear)){
                            CollisionBox box = new CollisionBox(objectSelector.hitResult.xNear,
                                    objectSelector.hitResult.yNear, objectSelector.hitResult.zNear,
                                    objectSelector.hitResult.xNear + world.blockWidth,
                                    objectSelector.hitResult.yNear + world.blockHeight,
                                    objectSelector.hitResult.zNear + world.blockWidth);
                            if (!player.collisionBox.intersects(box)) {
                                if (world.setBlock(objectSelector.hitResult.xNear,
                                        objectSelector.hitResult.yNear, objectSelector.hitResult.zNear,
                                        (byte)1)) {
                                    //если всё сошлось и блок поставлен, посылаем уведомление
                                    worldRenderer.blockChanged(objectSelector.hitResult.xNear,
                                            objectSelector.hitResult.yNear, objectSelector.hitResult.zNear);
                                    System.out.println("Placed a block at [" + objectSelector.hitResult.xNear + "," +
                                            objectSelector.hitResult.yNear + "," + objectSelector.hitResult.zNear + "]");
                                }
                            } else {
                                System.out.println("Position: [" + objectSelector.hitResult.xNear + "," +
                                        objectSelector.hitResult.yNear + "," +
                                        objectSelector.hitResult.zNear + "] is blocked by a player.");
                            }
                        } else {
                            System.out.println("Block [" + objectSelector.hitResult.xNear + "," +
                                    objectSelector.hitResult.yNear + "," +
                                    objectSelector.hitResult.zNear + "] is outside of the world!");
                        }
                    }
                } else {
                    grabMouse(true);
                }
            } else {
                //нажата правая кнопка
                if (Mouse.getEventButton() == 1 && Mouse.getEventButtonState()){
                    if (objectSelector.hitsCount > 0 && Mouse.isGrabbed()){
                        if (world.setBlock(objectSelector.hitResult.x,
                                objectSelector.hitResult.y, objectSelector.hitResult.z, (byte)0)) {
                            worldRenderer.blockChanged(objectSelector.hitResult.x,
                                    objectSelector.hitResult.y, objectSelector.hitResult.z);
                            System.out.println("Wrecked a block at [" + objectSelector.hitResult.x + "," +
                                    objectSelector.hitResult.y + "," + objectSelector.hitResult.z + "]");
                        }
                    }
                }
            }
        }
    }

    /*
     * renderScene()
     * Отрисовка всего и вся.
     */
    private void renderScene(){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //определяем, куда смотрит игрок
        int hits = objectSelector.pickObject(3.0f);

        //устанавливаем игровую камеру
        player.setupCamera();

        //рендерим мир
        worldRenderer.render();

        //если игрок смотрит на блок - подсвечиваем его
        if (hits > 0){
            renderHitResult();
        }

        //рисуем игровой интерфейс
        drawHud();

        //выводим текст на экран
        drawGlText();
    }

    private void renderHitResult(){
        Tesselator t = new Tesselator();
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        //плавная пульсация альфа-канала
        double a = (Math.sin(System.currentTimeMillis() / 200.0) * 0.6 + 0.75) * 0.2;
        glColor4d(1.0, 1.0, 1.0, a);
        objectSelector.renderHitCubeSide(t);
        t.flushQuads();
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
    }

    /*
     * drawGlText()
     * Выводит текст поверх игрового экрана.
     * Для правильного вывода, необходимо отключить GL_LIGHTING и GL_DEPTH_TEST.
     */
    private void drawGlText(){
        int screenWidth = windowWidth;

        font.start();

        String t = "Position: [" + player.printPosition() + "]";
        font.drawBkg(0, 0, font.width(t), 0x0);
        font.drawString(0,0, t, 0xFFFFFF);
        t = "Rotation: [" + player.printRotation() + "]";
        font.drawBkg(0, 18, font.width(t), 0x0);
        font.drawString(0, 18, t, 0xFFFFFF);
        t = "FPS: " + sceneFps;
        font.drawBkg(screenWidth - font.width(t) - 4, 0, font.width(t) + 4, 0x0);
        font.drawString(screenWidth - font.width(t) - 4, 0, t, 0x00FF00);
        t = "Delta time: " + gameTimer.deltaTime;
        font.drawBkg(screenWidth - font.width(t) - 4, 18, font.width(t) + 4, 0x0);
        font.drawString(screenWidth - font.width(t) - 4, 18, t, 0xFFFFFF);

        t = "World: " + world.getName() + " [" + world.sizeX + "," + world.sizeZ + "," + world.sizeY + "]";
        font.drawBkg(0, 36, font.width(t), 0x0);
        font.drawString(0, 36, t, 0xFFFFFF);

        t = "Chunks rendered: " + worldRenderer.chunksRendered + " / " + worldRenderer.chunksCount;
        font.drawBkg(0, 54, font.width(t) + 4, 0x0);
        font.drawString(0, 54, t, 0xFFFFFF);

        t = "Blocks rendered: " + worldRenderer.totalBlocksRendered + " / " + worldRenderer.totalBlocksPossible;
        font.drawBkg(0, 72, font.width(t) + 4, 0x0);
        font.drawString(0, 72, t, 0xFFFFFF);

        if (objectSelector.hitsCount > 0){
            t = "Looks at [" + objectSelector.hitResult.x + "," + objectSelector.hitResult.y + "," +
                    objectSelector.hitResult.z + "] " + objectSelector.cubeSideIdToString(objectSelector.hitResult.sideId);
        } else {
            t = "Looks at: none";
        }
        font.drawBkg(0, 90, font.width(t) + 4, 0x0);
        font.drawString(0, 90, t, 0xFFFFFF);

        font.end();
    }

    /*
     * drawHud()
     * Рисует игровой интерфейс.
     */
    private void drawHud() {
        //Переключаемся в режим плоского экрана
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        //Последние два параметра определяют глубину (толщину) плоскости.
        //На самом деле, по факту, плоскость не является плоской.
        glOrtho(0.0, windowWidth, 0.0, windowHeight, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);

        int xCenter = windowWidth / 2;
        int yCenter = windowHeight / 2;

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2f(xCenter - 10.0f, yCenter);
        glVertex2f(xCenter + 10.0f, yCenter);
        glVertex2f(xCenter, yCenter - 10.0f);
        glVertex2f(xCenter, yCenter + 10.0f);
        glEnd();

        glEnable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    private void destroy() {
        System.out.println("Destroying...");
        world.save();
        font.destroy();
        Textures.deleteGameTextures();
        Keyboard.destroy();
        Mouse.destroy();
        Display.destroy();
    }

    private void resizeGlWindow(int newWidth, int newHeight){
        windowWidth = newWidth;
        windowHeight = newHeight;
        glViewport(0,0, newWidth, newHeight);
        System.out.println("Viewport changed to " + newWidth + "x" + newHeight);
    }

    private void grabMouse(boolean flag){
        if (flag) {
            if (!Mouse.isGrabbed()) {
                Mouse.setGrabbed(true);
                System.out.println("Mouse is grabbed");
            }
        } else {
            if (Mouse.isGrabbed()){
                Mouse.setGrabbed(false);
                Mouse.setCursorPosition(windowWidth / 2, windowHeight / 2);
                System.out.println("Mouse is released");
            }
        }
    }

    public ByteBuffer loadImageAsByteBuffer(String fn){
        try {
            BufferedImage image;
            if (fn.startsWith("/")) {
                URL url = CubixCraft.class.getResource(fn);
                if (url == null){
                    System.out.println("Can not load file: " + fn);
                    return null;
                }
                image = ImageIO.read(url);
            }
            else
            {
                image = ImageIO.read(new File(fn));
            }
            byte[] buffer = new byte[image.getWidth() * image.getHeight() * 4];
            int counter = 0;
            for (int i = 0; i < image.getHeight(); i++) {
                for (int j = 0; j < image.getWidth(); j++) {
                    int colorSpace = image.getRGB(j, i);
                    buffer[counter] =     (byte) ((colorSpace << 8) >> 24);
                    buffer[counter + 1] = (byte) ((colorSpace << 16) >> 24);
                    buffer[counter + 2] = (byte) colorSpace;
                    buffer[counter + 3] = (byte) (colorSpace >> 24);
                    counter += 4;
                }
            }
            return ByteBuffer.wrap(buffer);
        } catch (IOException e) {
            System.out.println("Can not load file: " + fn);
            e.printStackTrace();
            return null;
        }
    }

    public static void main(String[] args) {
        System.out.println("Starting Cubixcraft | Version 0.0.1.1-alpha Java edition | " +
                "26.04.2020 is the date the cubes were respawned! AGAIN!!! IN JAVA!!!!!");
        new CubixCraft();
    }
}
