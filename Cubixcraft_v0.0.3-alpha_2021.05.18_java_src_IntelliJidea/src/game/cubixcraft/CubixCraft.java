package game.cubixcraft;

import game.cubixcraft.block.Block;
import game.cubixcraft.utils.*;
import game.cubixcraft.world.*;

import org.lwjgl.LWJGLException;
import org.lwjgl.input.Keyboard;
import org.lwjgl.input.Mouse;
import org.lwjgl.opengl.Display;
import org.lwjgl.opengl.DisplayMode;

import java.nio.ByteBuffer;

import static org.lwjgl.opengl.GL11.*;
import static game.cubixcraft.utils.Utils.*;

public class CubixCraft {
    private static int windowWidth = 640;
    private static int windowHeight = 480;

    private final Player player;
    private final World world;
    private final ObjectSelector objectSelector;
    private final Font font;
    private final GameTimer gameTimer;

    private int sceneFps = 0;
    private int chunkUpdates = 0;
    private float blockInHandRotatingAngle = 0.0f;

    private static CubixCraft cubixcraftObj;

    private final String title = "Cubixcraft 0.0.3-alpha | Java edition";

    private boolean running;

    public CubixCraft() {
        try {
            Display.setTitle(title);
            Display.setResizable(true);
            Display.setDisplayMode(new DisplayMode(windowWidth, windowHeight));
            ByteBuffer[] byteBuffer = new ByteBuffer[]{ loadImageAsByteBuffer("/textures/tree.png") };
            if (byteBuffer[0] != null) {
                Display.setIcon(byteBuffer);
            } else {
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

        cubixcraftObj = this;
        
        Textures.loadGameTextures();

        font = new Font("/textures/font.png");

        world = new World(256, 256, 256, "world1");
        if (!world.load()) {
            world.generateMap();
        }

        player = new Player(world, 0.0f, world.sizeY + 2.0f,0.0f,
                135.0f,0.0f,0.0f);
        objectSelector = new ObjectSelector(player);
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

            //выполняется при изменении размеров окна
            if (Display.wasResized()) {
                windowWidth = Display.getWidth();
                windowHeight = Display.getHeight();
                resizeViewport(windowWidth, windowHeight);
            }

            gameTimer.advanceTime();

            //Продвигаем жизненный цикл игрока.
            //В текущей версии обрабатывается только управление.
            player.tick();

            renderScene();

            //Проверяем, не было ли ошибок при рендеринге.
            //Если они были, то в консоль будет выведено сообщение.
            checkOpenGlError("Render scene");

            //определяем FPS (frames per second)
            //и количество изменившихся чанков в секунду.
            frames++;
            if (System.currentTimeMillis() >= lastTime + 1000L) {
                sceneFps = frames;
                chunkUpdates = Chunk.totalChunkUpdates;

                //сбрасываем счётчики
                lastTime = System.currentTimeMillis();
                frames = 0;
                Chunk.totalChunkUpdates = 0;
            }

            Display.update();
        }

        System.out.println("Destroying everything...");
        destroyGame();
    }

    private void onMouseWheel(int wheelDirection) {
        int blockId;
        //при вращении колеса мыши, меняем блок в руках игрока
        switch (wheelDirection) {
            case MOUSE_WHEEL_UP:
                blockId = player.getBlockIdInHand() - 1;
                if (blockId < 1) {
                    blockId = 2;
                }
                player.setBlockIdInHand((byte)blockId);
                break;

            case MOUSE_WHEEL_DOWN:
                blockId = player.getBlockIdInHand() + 1;
                if (blockId > 2) {
                    blockId = 1;
                }
                player.setBlockIdInHand((byte)blockId);
                break;
        }
    }

    private void handleUserInput() {
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

            if (Keyboard.isKeyDown(Keyboard.KEY_G)) {
                System.out.println("Refreshing chunks...");
                world.getRenderer().setAllChunksExpired();
            }

            if (Keyboard.isKeyDown(Keyboard.KEY_R)) {
                player.reset();
                System.out.println("Player is respawned.");
            }
        }

        //обработка нажатия кнопок и колеса мыши
        while (Mouse.next()) {
            int wheel = Mouse.getDWheel();
            if (wheel < 0) {
                onMouseWheel(MOUSE_WHEEL_UP);
            } else if (wheel > 0) {
                onMouseWheel(MOUSE_WHEEL_DOWN);
            }

            if (Mouse.getEventButtonState()) {
                switch (Mouse.getEventButton()) {
                    case MOUSE_BUTTON_LEFT:
                        //нажата левая кнопка мыши
                        if (Mouse.isGrabbed()) {
                            /*
                             * Если игрок смотрит на блок, то как минимум одна сторона
                             * этого блока всегда свободна. С этой стороны игрок может
                             * поставить новый блок. Но сначала необходимо проверить,
                             * не стоит ли на этом месте сам игрок, чтобы не дать
                             * поставить блок на самого себя.
                             */
                            if (objectSelector.getHitsCount() > 0) {
                                if (world.isBlockInWorld(objectSelector.hitResult.xNear,
                                        objectSelector.hitResult.yNear, objectSelector.hitResult.zNear)) {
                                    CollisionBox box = new CollisionBox(objectSelector.hitResult.xNear,
                                            objectSelector.hitResult.yNear, objectSelector.hitResult.zNear,
                                            objectSelector.hitResult.xNear + world.blockWidth,
                                            objectSelector.hitResult.yNear + world.blockHeight,
                                            objectSelector.hitResult.zNear + world.blockWidth);
                                    if (!player.collisionBox.intersects(box)) {
                                        world.setBlock(objectSelector.hitResult.xNear,
                                                objectSelector.hitResult.yNear, objectSelector.hitResult.zNear,
                                                player.getBlockIdInHand());
                                    } else {
                                        System.out.println("Position [" + objectSelector.hitResult.xNear + "," +
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
                        break;

                    case MOUSE_BUTTON_RIGHT:
                        //нажата правая кнопка мыши
                        if (objectSelector.getHitsCount() > 0 && Mouse.isGrabbed()) {
                            world.destroyBlock(objectSelector.hitResult.x,
                                    objectSelector.hitResult.y, objectSelector.hitResult.z);
                        }
                        break;
                }
            }
        }
    }

    /*
     * renderScene()
     * Отрисовка всего и вся.
     */
    private void renderScene() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //определяем, куда смотрит игрок
        int hits = objectSelector.pickObject(3.0f);

        //устанавливаем игровую камеру
        player.setupCamera();

        //рендерим мир
        world.getRenderer().render();

        //если игрок смотрит на блок - подсвечиваем его
        if (hits > 0) {
            renderHitResult();
        }

        //рисуем игровой интерфейс
        drawHud();

        //выводим текст на экран
        drawHudText();
    }

    /*
     * renderHitResult()
     * Подсветка блока, на который смотрит игрок.
     */
    private void renderHitResult() {
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //плавная пульсация альфа-канала для подсветки стороны блока
        double alpha = (Math.sin(System.currentTimeMillis() / 200.0) * 0.6 + 0.65) * 0.5;

        //здесь можно задать цвет RGB для подсветки
        glColor4d(1.0, 1.0, 1.0, alpha);

        Tessellator t = new Tessellator();
        objectSelector.renderHitCubeSide(t);
        t.flushQuads();

        /*
         * Если позиция, на которую игрок может поставить новый блок
         * не занята самим игроком, то рисуем на этом месте блок-призрак.
         * Для того чтобы нарисовать блок с нужной текстурой,
         * придётся создать дополнительный его экземпляр.
         */
        Block block = world.createBlock(player.getBlockIdInHand());
        block.setPosition(objectSelector.hitResult.xNear, objectSelector.hitResult.yNear,  objectSelector.hitResult.zNear);
        if (!block.collisionBox.intersects(player.collisionBox)) {
            //Делаем так, чтобы блок-призрак пульсировал медленнее
            //и немного менее прозрачно, чем подсветка стороны блока.
            alpha = (Math.sin(System.currentTimeMillis() / 300.0) * 0.6 + 0.65) * 0.6;
            glColor4d(1.0, 1.0, 1.0, alpha);

            glEnable(GL_TEXTURE_2D);
            block.render(t);
            t.flushTextureQuads();
            glDisable(GL_TEXTURE_2D);
        }

        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
    }

    /*
     * drawHud()
     * Рисует игровой интерфейс.
     */
    private void drawHud() {
        //Переключаемся в режим плоского экрана (ортогональная проекция)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //Последние два параметра определяют глубину (толщину) плоскости.
        //На самом деле, по факту, плоскость не является плоской.
        glOrtho(0.0, windowWidth, 0.0, windowHeight, -100.0, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //Рисуем в HUD блок, который держит игрок.
        //Для этого необходимо, чтобы главный текстурный атлас
        //заранее был прикреплён вызовом функции glBindTexture().

        float blockInHandPreviewSize = 50.0f;
        float blockPreviewPositionX = windowWidth - blockInHandPreviewSize;
        float blockPreviewPositionY = windowHeight - 100.0f;

        //делаем так, чтобы блок вращался
        blockInHandRotatingAngle += gameTimer.getDeltaTime() * 100.0f;
        while (blockInHandRotatingAngle >= 360.0f) {
            blockInHandRotatingAngle -= 360.0f;
        }

        //собственно, вращение блока
        glTranslatef(blockPreviewPositionX, blockPreviewPositionY, blockInHandPreviewSize / 2.0f);
        glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(blockInHandRotatingAngle, 0.0f, 1.0f, 0.0f);
        glTranslatef(-blockPreviewPositionX , -blockPreviewPositionY, -(blockInHandPreviewSize / 2.0f));

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);

        Tessellator t = new Tessellator();

        //Для того чтобы получить координаты текстуры блока, необходимо создать его экземпляр
        Block block = world.createBlock(player.getBlockIdInHand());
        glBindTexture(GL_TEXTURE_2D, Textures.textureBase);
        for (int i = 0; i < 6; i++) {
            RenderCubeSideHud(block, t, blockPreviewPositionX - blockInHandPreviewSize / 2.0f,
                    blockPreviewPositionY - blockInHandPreviewSize / 2.0f, 0.0f, blockInHandPreviewSize, i);
        }
        t.flushTextureQuads();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);

        glLoadIdentity();

        //рисуем перекрестие в центре экрана
        float screenCenterX = windowWidth / 2.0f;
        float screenCenterY = windowHeight / 2.0f;

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2f(screenCenterX - 10.0f, screenCenterY);
        glVertex2f(screenCenterX + 10.0f, screenCenterY);
        glVertex2f(screenCenterX, screenCenterY - 10.0f);
        glVertex2f(screenCenterX, screenCenterY + 10.0f);
        glEnd();

        glEnable(GL_DEPTH_TEST);
    }

    /*
     * drawHudText()
     * Выводит текст поверх игрового экрана.
     * Для правильного вывода, необходимо отключить GL_LIGHTING и GL_DEPTH_TEST.
     */
    private void drawHudText() {
        int screenWidth = windowWidth;
        int screenHeight = windowHeight;

        font.start();

        String t = "Position: [" + player.getPositionAsString() + "]";
        font.drawBkg(0.0f, 0.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, 0.0f,0.0f, 0xFFFFFF);

        t = "Rotation: [" + player.getRotationAsString() + "]";
        font.drawBkg(0.0f, 18.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, 0.0f, 18.0f, 0xFFFFFF);

        t = "FPS: " + sceneFps;
        font.drawBkg(screenWidth - font.getStringWidth(t), 0.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, screenWidth - font.getStringWidth(t), 0.0f, 0x00FF00);

        t = "Delta time: " + gameTimer.getDeltaTime();
        font.drawBkg(screenWidth - font.getStringWidth(t), 18.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, screenWidth - font.getStringWidth(t), 18.0f, 0xFFFFFF);

        t = "Chunk updates: " + chunkUpdates;
        font.drawBkg(screenWidth - font.getStringWidth(t), 36.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, screenWidth - font.getStringWidth(t), 36.0f, 0xFFFFFF);

        t = "World: " + world.getName() + " [" + world.sizeX + "," + world.sizeY + "," + world.sizeZ + "]";
        font.drawBkg(0.0f, 36.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, 0.0f, 36.0f, 0xFFFFFF);

        WorldRenderer wr = world.getRenderer();

        t = "Chunks rendered: " + wr.getTotalChunksRendered() + " / " + wr.chunksCount;
        font.drawBkg(0.0f, 54.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, 0.0f, 54.0f, 0xFFFFFF);

        t = "Blocks rendered: " + wr.getTotalBlocksRendered() + " / " + wr.getTotalBlocksPossible();
        font.drawBkg(0.0f, 72.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, 0.0f, 72.0f, 0xFFFFFF);

        t = "Quads rendered: " + wr.getTotalQuadsRendered() + " / " + wr.getTotalQuadsPossible();
        font.drawBkg(0.0f, 90.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, 0.0f, 90.0f,0xFFFFFF);

        t = "Vertexes rendered: " + wr.getTotalQuadsRendered() * 4 + " / " + wr.getTotalQuadsPossible() * 4;
        font.drawBkg(0.0f, 108.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, 0.0f, 108.0f, 0xFFFFFF);

        if (objectSelector.getHitsCount() > 0) {
            Block block = world.getBlock(objectSelector.hitResult.cubeId);
            t = "Looking at [" + objectSelector.hitResult.x + "," + objectSelector.hitResult.y + "," +
                    objectSelector.hitResult.z + "] " + block.getName() + ", " +
                    objectSelector.cubeSideIdToString(objectSelector.hitResult.sideId) + " side";
        } else {
            t = "Looking at: none";
        }
        font.drawBkg(0.0f, 126.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, 0.0f, 126.0f, 0xFFFFFF);

        t = "Press R to respawn a player";
        font.drawBkg(0.0f, screenHeight - 36.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, 0.0f, screenHeight - 36.0f, 0xFFFFFF);

        t = "Press G to refresh chunks";
        font.drawBkg(0.0f, screenHeight - 18.0f, font.getStringWidth(t), 0x0);
        font.drawString(t, 0.0f, screenHeight - 18.0f, 0xFFFFFF);

        font.end();
    }

    /*
     * destroyGame()
     * Вызывается после завершения игрового цикла.
     * Сохраняет игровой мир и освобождает память от объектов, которые можно (и нужно) удалить вручную.
     */
    private void destroyGame() {
        world.save();
        Textures.deleteGameTextures();
        font.destroy();

        for (Chunk chunk : world.getRenderer().chunks) {
            chunk.destroyList();
        }

        Keyboard.destroy();
        Mouse.destroy();
        Display.destroy();
    }

    private void resizeViewport(int newWidth, int newHeight) {
        glViewport(0,0, newWidth, newHeight);
        System.out.println("Viewport changed to " + newWidth + "x" + newHeight);
    }

    private void grabMouse(boolean flag) {
        if (flag) {
            if (!Mouse.isGrabbed()) {
                Mouse.setGrabbed(true);
                System.out.println("Mouse is grabbed.");
            }
        } else {
            if (Mouse.isGrabbed()) {
                Mouse.setGrabbed(false);
                Mouse.setCursorPosition(windowWidth / 2, windowHeight / 2);
                System.out.println("Mouse is released.");
            }
        }
    }

    public GameTimer getGameTimer() {
        return gameTimer;
    }

    public World getWorld() {
        return world;
    }

    public static int getWindowWidth() {
        return windowWidth;
    }

    public static int getWindowHeight() {
        return windowHeight;
    }

    public static CubixCraft getCubixcraft() {
        return cubixcraftObj;
    }

    public static void main(String[] args) {
        System.out.println("Starting Cubixcraft third alpha... | Version 0.0.3-alpha, Java edition | " +
                "2021.05.18 is the date of the 367 days anniversary of code refactoring!");
        new CubixCraft();
    }

}
