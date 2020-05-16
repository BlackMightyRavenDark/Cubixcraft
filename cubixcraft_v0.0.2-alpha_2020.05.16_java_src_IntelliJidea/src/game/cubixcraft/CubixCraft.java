package game.cubixcraft;

import static game.cubixcraft.utils.Utils.*;

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

public class CubixCraft {
    public static int windowWidth = 640;
    public static int windowHeight = 480;
    public Player player;
    private GameTimer gameTimer;
    private World world;
    private WorldRenderer worldRenderer;
    private ObjectSelector objectSelector;
    private Font font;
    private int sceneFps = 0;
    private int chunkUpdates = 0;
    private String title = "Cubixcraft 0.0.2-alpha | Java edition";
    boolean running;
    private static CubixCraft cubixcraftObj;

    private float blockInHandRotatingAngle = 0.0f;

    public static CubixCraft getCubixcraft() {
        return cubixcraftObj;
    }

    public CubixCraft() {
        try {
            Display.setDisplayMode(new DisplayMode(windowWidth, windowHeight));
            Display.setTitle(title);
            Display.setResizable(true);
            ByteBuffer[] byteBuffer = new ByteBuffer[]{ loadImageAsByteBuffer("/textures/tree.png") };
            if (byteBuffer[0] != null) {
                Display.setIcon(byteBuffer);
            }
            else {
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

        world = new World(256, 256, 64, "world1");
        if (!world.load()) {
            world.generateMap();
        }

        worldRenderer = new WorldRenderer(world);
        player = new Player(world, 0.0f, world.sizeZ + 2.0f,0.0f,
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

            //Выполняется при изменении размеров окна
            if (Display.wasResized()) {
                resizeGlWindow(Display.getWidth(), Display.getHeight());
            }

            gameTimer.advanceTime();

            //Продвигаем жизненный цикл игрока.
            //В текущей версии обрабатывается только управление.
            player.tick();

            renderScene();

            //Проверяем, не было ли ошибок при рендеринге.
            //Если они были, то в консоль будет выведено сообщение.
            CheckOpenGlError("render scene");

            //определяем FPS (frames per second)
            //и количество изменившихся чанков за секунду.
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

        destroyGame();
    }

    private void onMouseWheel(int wheelDirection) {
        //при вращении колеса мыши, меняем блок в руках игрока
        switch (wheelDirection) {

            case MOUSE_WHEEL_UP:
                player.blockIdInHand -= 1;
                if (player.blockIdInHand < 1) {
                    player.blockIdInHand = 2;
                }
                break;

            case MOUSE_WHEEL_DOWN:
                player.blockIdInHand += 1;
                if (player.blockIdInHand > 2) {
                    player.blockIdInHand = 1;
                }
                break;
        }
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

            if (Keyboard.isKeyDown(Keyboard.KEY_G)) {
                System.out.println("Refreshing chunks...");
                worldRenderer.setAllChunksExpired();
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
            }
            else
            if (wheel > 0) {
                onMouseWheel(MOUSE_WHEEL_DOWN);
            }

            //нажата левая кнопка
            if (Mouse.getEventButton() == MOUSE_BUTTON_LEFT && Mouse.getEventButtonState()) {
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
                                        world.createBlock(player.blockIdInHand));
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
                if (Mouse.getEventButton() == MOUSE_BUTTON_RIGHT && Mouse.getEventButtonState()) {
                    if (objectSelector.getHitsCount() > 0 && Mouse.isGrabbed()) {
                        world.destroyBlock(objectSelector.hitResult.x,
                                objectSelector.hitResult.y, objectSelector.hitResult.z);
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
        if (hits > 0) {
            renderHitResult();
        }

        //рисуем игровой интерфейс
        drawHud();

        //выводим текст на экран
        drawGlText();
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
        double a = (Math.sin(System.currentTimeMillis() / 200.0) * 0.6 + 0.65) * 0.5;

        //Здесь можно задать цвет RGB для подсветки
        glColor4d(1.0, 1.0, 1.0, a);

        Tessellator t = new Tessellator();
        objectSelector.renderHitCubeSide(t);
        t.flushQuads();

        /*
         * Если позиция, на которую игрок может поставить новый блок
         * не занята самим игроком, то рисуем на этом месте блок-призрак.
         * Для того чтобы нарисовать блок с нужной текстурой,
         * придётся создать дополнительный его экземпляр.
         */
        Block b = world.createBlock(player.blockIdInHand);
        b.setPosition(objectSelector.hitResult.xNear, objectSelector.hitResult.yNear,  objectSelector.hitResult.zNear);
        if (!b.collisionBox.intersects(player.collisionBox)) {
            //Делаем так, чтобы блок-призрак пульсировал медленнее
            //и немного менее прозрачно, чем подсветка стороны блока.
            a = (Math.sin(System.currentTimeMillis() / 300.0) * 0.6 + 0.65) * 0.6;
            glColor4d(1.0, 1.0, 1.0, a);

            glEnable(GL_TEXTURE_2D);
            b.render(t);
            t.flushTextureQuads();
            glDisable(GL_TEXTURE_2D);
        }

        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
    }

    /*
     * drawGlText()
     * Выводит текст поверх игрового экрана.
     * Для правильного вывода, необходимо отключить GL_LIGHTING и GL_DEPTH_TEST.
     */
    private void drawGlText() {
        int screenWidth = windowWidth;
        int screenHeight = windowHeight;

        font.start();

        String t = "Position: [" + player.getPositionAsString() + "]";
        font.drawBkg(0, 0, font.width(t), 0x0);
        font.drawString(0,0, t, 0xFFFFFF);
        t = "Rotation: [" + player.getRotationAsString() + "]";
        font.drawBkg(0, 18, font.width(t), 0x0);
        font.drawString(0, 18, t, 0xFFFFFF);
        t = "FPS: " + sceneFps;
        font.drawBkg(screenWidth - font.width(t) - 4, 0, font.width(t) + 4, 0x0);
        font.drawString(screenWidth - font.width(t) - 4, 0, t, 0x00FF00);
        t = "Delta time: " + gameTimer.getDeltaTime();
        font.drawBkg(screenWidth - font.width(t) - 4, 18, font.width(t) + 4, 0x0);
        font.drawString(screenWidth - font.width(t) - 4, 18, t, 0xFFFFFF);
        t = "Chunk updates: " + chunkUpdates;
        font.drawBkg(screenWidth - font.width(t) - 4, 36, font.width(t) + 4, 0x0);
        font.drawString(screenWidth - font.width(t) - 4, 36, t, 0xFFFFFF);

        t = "World: " + world.getName() + " [" + world.sizeX + "," + world.sizeZ + "," + world.sizeY + "]";
        font.drawBkg(0, 36, font.width(t), 0x0);
        font.drawString(0, 36, t, 0xFFFFFF);

        t = "Chunks rendered: " + worldRenderer.getTotalChunksRendered() + " / " + worldRenderer.chunksCount;
        font.drawBkg(0, 54, font.width(t) + 4, 0x0);
        font.drawString(0, 54, t, 0xFFFFFF);

        t = "Blocks rendered: " + worldRenderer.getTotalBlocksRendered() + " / " + worldRenderer.getTotalBlocksPossible();
        font.drawBkg(0, 72, font.width(t) + 4, 0x0);
        font.drawString(0, 72, t, 0xFFFFFF);

        t = "Quads rendered: " + worldRenderer.getTotalQuadsRendered() + " / " + worldRenderer.getTotalQuadsPossible();
        font.drawBkg(0, 90, font.width(t) + 4, 0x0);
        font.drawString(0, 90, t, 0xFFFFFF);

        t = "Vertexes rendered: " + worldRenderer.getTotalQuadsRendered() * 4 + " / " +
                worldRenderer.getTotalQuadsPossible() * 4;
        font.drawBkg(0, 108, font.width(t) + 4, 0x0);
        font.drawString(0, 108, t, 0xFFFFFF);

        if (objectSelector.getHitsCount() > 0) {
            t = "Looks at [" + objectSelector.hitResult.x + "," + objectSelector.hitResult.y + "," +
                    objectSelector.hitResult.z + "] " + world.getBlock(objectSelector.hitResult.cubeId).getName() + ", " +
                    objectSelector.cubeSideIdToString(objectSelector.hitResult.sideId) + " side";
        } else {
            t = "Looks at: none";
        }
        font.drawBkg(0, 126, font.width(t) + 4, 0x0);
        font.drawString(0, 126, t, 0xFFFFFF);

        t = "Press R to respawn a player";
        font.drawBkg(0, screenHeight - 36, font.width(t) + 4, 0x0);
        font.drawString(0, screenHeight - 36, t, 0xFFFFFF);

        t = "Press G to refresh chunks";
        font.drawBkg(0, screenHeight - 18, font.width(t) + 4, 0x0);
        font.drawString(0, screenHeight - 18, t, 0xFFFFFF);

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
        glOrtho(0.0, windowWidth, 0.0, windowHeight, -100.0, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        //Рисуем в HUD блок, который держит игрок
        float blockInHandPreviewSize = 50.0f;
        float blockPreviewPositionX = windowWidth - blockInHandPreviewSize;
        float blockPreviewPositionY = windowHeight - 100.0f;

        //Вращение блока в HUD
        blockInHandRotatingAngle += gameTimer.getDeltaTime() * 100.0f;
        while (blockInHandRotatingAngle >= 360.0f) {
            blockInHandRotatingAngle -= 360.0f;
        }

        glTranslatef(blockPreviewPositionX, blockPreviewPositionY, blockInHandPreviewSize / 2.0f);
        glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(blockInHandRotatingAngle, 0.0f, 1.0f, 0.0f);
        glTranslatef(-blockPreviewPositionX , -blockPreviewPositionY, -(blockInHandPreviewSize / 2.0f));

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);

        Tessellator t = new Tessellator();

        //Для того чтобы получить координаты текстуры блока, необходимо создать его экземпляр
        Block b = world.createBlock(player.blockIdInHand);
        glBindTexture(GL_TEXTURE_2D, Textures.textureBase);
        for (int i = 0; i < 6; i++) {
            RenderCubeSide2D(b, t, blockPreviewPositionX - blockInHandPreviewSize / 2.0f,
                    blockPreviewPositionY - (blockInHandPreviewSize / 2.0f), 0.0f, 50.0f, i);
        }
        t.flushTextureQuads();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);

        glLoadIdentity();

        float screenCenterX = windowWidth / 2.0f;
        float screenCenterY = windowHeight / 2.0f;

        //рисуем перекрестие в центре экрана
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2f(screenCenterX - 10.0f, screenCenterY);
        glVertex2f(screenCenterX + 10.0f, screenCenterY);
        glVertex2f(screenCenterX, screenCenterY - 10.0f);
        glVertex2f(screenCenterX, screenCenterY + 10.0f);
        glEnd();

        glEnable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    private void destroyGame() {
        System.out.println("Destroying everything...");
        world.save();
        Textures.deleteGameTextures();
        font.destroy();
        for (int i = 0; i < worldRenderer.chunks.length; i++) {
            worldRenderer.chunks[i].destroyList();
        }
        Keyboard.destroy();
        Mouse.destroy();
        Display.destroy();
    }

    private void resizeGlWindow(int newWidth, int newHeight) {
        windowWidth = newWidth;
        windowHeight = newHeight;
        glViewport(0,0, newWidth, newHeight);
        System.out.println("Viewport changed to " + newWidth + "x" + newHeight);
    }

    private void grabMouse(boolean flag) {
        if (flag) {
            if (!Mouse.isGrabbed()) {
                Mouse.setGrabbed(true);
                System.out.println("Mouse is grabbed");
            }
        } else {
            if (Mouse.isGrabbed()) {
                Mouse.setGrabbed(false);
                Mouse.setCursorPosition(windowWidth / 2, windowHeight / 2);
                System.out.println("Mouse is released");
            }
        }
    }

    public GameTimer getGameTimer() {
        return gameTimer;
    }

    public WorldRenderer getWorldRenderer() {
        return worldRenderer;
    }

    public static void main(String[] args) {
        System.out.println("Starting Cubixcraft | 0.0.2-alpha Java edition | " +
                "16.05.2020 is the date when dirt came to the Earth!");
        new CubixCraft();
	}

}
