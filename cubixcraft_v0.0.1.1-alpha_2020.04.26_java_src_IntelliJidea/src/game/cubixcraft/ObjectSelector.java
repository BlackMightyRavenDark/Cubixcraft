/*
 * Класс ObjectSelector отвечает за определение игровых объектов, на которые смотрит игрок.
 */
package game.cubixcraft;

import static game.cubixcraft.Block.*;

import org.lwjgl.BufferUtils;

import java.nio.IntBuffer;

import static org.lwjgl.opengl.GL11.*;
import static org.lwjgl.util.glu.GLU.*;

public class ObjectSelector {

    public HitResult hitResult = null;
    private Player player;
    private World world;
    private IntBuffer viewportBuffer;
    private IntBuffer selectBuffer;
    public int hitsCount = 0;

    public ObjectSelector(World world, Player player){
        this.world = world;
        this.player = player;
        viewportBuffer = BufferUtils.createIntBuffer(16);
        selectBuffer = BufferUtils.createIntBuffer(400);
    }

    /*
     * renderHitCubeSide()
     * Подсвечивает грань куба, на которую смотрит игрок.
     */
    public void renderHitCubeSide(Tesselator t) {
        float x0 = (float)hitResult.x;
        float y0 = (float)hitResult.y;
        float z0 = (float)hitResult.z;
        float x1;
        float y1;
        float z1;
        switch (hitResult.sideId) {
            case BLOCK_SIDE_FRONT:
                x1 = x0 + player.worldObj.blockWidth;
                y1 = y0 + player.worldObj.blockHeight;
                z1 = z0 + player.worldObj.blockWidth;
                t.addVertexQuad(x0, y0, z1);
                t.addVertexQuad(x1, y0, z1);
                t.addVertexQuad(x1, y1, z1);
                t.addVertexQuad(x0, y1, z1);
                break;

            case BLOCK_SIDE_BACK:
                x1 = x0 + player.worldObj.blockWidth;
                y1 = y0 + player.worldObj.blockHeight;
                t.addVertexQuad(x0, y0, z0);
                t.addVertexQuad(x1, y0, z0);
                t.addVertexQuad(x1, y1, z0);
                t.addVertexQuad(x0, y1, z0);
                break;

            case BLOCK_SIDE_LEFT:
                y1 = y0 + player.worldObj.blockHeight;
                z1 = z0 + player.worldObj.blockWidth;
                t.addVertexQuad(x0, y0, z0);
                t.addVertexQuad(x0, y0, z1);
                t.addVertexQuad(x0, y1, z1);
                t.addVertexQuad(x0, y1, z0);
                break;

            case BLOCK_SIDE_RIGHT:
                x1 = x0 + player.worldObj.blockWidth;
                y1 = y0 + player.worldObj.blockHeight;
                z1 = z0 + player.worldObj.blockWidth;
                t.addVertexQuad(x1, y0, z0);
                t.addVertexQuad(x1, y0, z1);
                t.addVertexQuad(x1, y1, z1);
                t.addVertexQuad(x1, y1, z0);
                break;

            case BLOCK_SIDE_BOTTOM:
                x1 = x0 + player.worldObj.blockWidth;
                z1 = z0 + player.worldObj.blockWidth;
                t.addVertexQuad(x0, y0, z0);
                t.addVertexQuad(x1, y0, z0);
                t.addVertexQuad(x1, y0, z1);
                t.addVertexQuad(x0, y0, z1);
                break;

            case BLOCK_SIDE_TOP:
                x1 = x0 + player.worldObj.blockWidth;
                y1 = y0 + player.worldObj.blockHeight;
                z1 = z0 + player.worldObj.blockWidth;
                t.addVertexQuad(x0, y1, z0);
                t.addVertexQuad(x1, y1, z0);
                t.addVertexQuad(x1, y1, z1);
                t.addVertexQuad(x0, y1, z1);
                break;
        }
    }

    /*
     * renderGhostCubeSide()
     * Рендерит невидимую сторону (грань) блока, если она не заблокирована другим блоком.
     * Вызывается в renderGhostCubeSides().
     */
    private void renderGhostCubeSide(Tesselator t, int x, int y, int z, int side) {
        float x0 = (float)x;
        float y0 = (float)y;
        float z0 = (float)z;
        float x1;
        float y1;
        float z1;
        switch (side) {
            case BLOCK_SIDE_FRONT:
                if (!player.worldObj.isBlockSolid(x, y, z + 1)) {
                    x1 = x0 + player.worldObj.blockWidth;
                    y1 = y0 + player.worldObj.blockHeight;
                    z1 = z0 + player.worldObj.blockWidth;
                    t.addVertexQuad(x0, y0, z1);
                    t.addVertexQuad(x1, y0, z1);
                    t.addVertexQuad(x1, y1, z1);
                    t.addVertexQuad(x0, y1, z1);
                }
                break;

            case BLOCK_SIDE_BACK:
                if (!player.worldObj.isBlockSolid(x, y, z - 1)) {
                    x1 = x0 + player.worldObj.blockWidth;
                    y1 = y0 + player.worldObj.blockHeight;
                    t.addVertexQuad(x0, y0, z0);
                    t.addVertexQuad(x1, y0, z0);
                    t.addVertexQuad(x1, y1, z0);
                    t.addVertexQuad(x0, y1, z0);
                }
                break;

            case BLOCK_SIDE_LEFT:
                if (!player.worldObj.isBlockSolid(x - 1, y, z)) {
                    y1 = y0 + player.worldObj.blockHeight;
                    z1 = z0 + player.worldObj.blockHeight;
                    t.addVertexQuad(x0, y0, z0);
                    t.addVertexQuad(x0, y0, z1);
                    t.addVertexQuad(x0, y1, z1);
                    t.addVertexQuad(x0, y1, z0);
                }
                break;

            case BLOCK_SIDE_RIGHT:
                if (!player.worldObj.isBlockSolid(x + 1, y, z)) {
                    x1 = x0 + player.worldObj.blockWidth;
                    y1 = y0 + player.worldObj.blockHeight;
                    z1 = z0 + player.worldObj.blockWidth;
                    t.addVertexQuad(x1, y0, z0);
                    t.addVertexQuad(x1, y0, z1);
                    t.addVertexQuad(x1, y1, z1);
                    t.addVertexQuad(x1, y1, z0);
                }
                break;

            case BLOCK_SIDE_TOP:
                if (!player.worldObj.isBlockSolid(x, y + 1, z)) {
                    x1 = x0 + player.worldObj.blockWidth;
                    y1 = y0 + player.worldObj.blockHeight;
                    z1 = z0 + player.worldObj.blockWidth;
                    t.addVertexQuad(x0, y1, z0);
                    t.addVertexQuad(x0, y1, z1);
                    t.addVertexQuad(x1, y1, z1);
                    t.addVertexQuad(x1, y1, z0);
                }
                break;

            case BLOCK_SIDE_BOTTOM:
                if (!player.worldObj.isBlockSolid(x, y - 1, z)) {
                    x1 = x0 + player.worldObj.blockWidth;
                    z1 = z0 + player.worldObj.blockWidth;
                    t.addVertexQuad(x0, y0, z0);
                    t.addVertexQuad(x0, y0, z1);
                    t.addVertexQuad(x1, y0, z1);
                    t.addVertexQuad(x1, y0, z0);
                }
                break;
        }
    }

    /*
     * renderGhostCubeSides()
     * Рисует невидимые грани кубов для возможности взаимодействия игрока с ними.
     * Этот метод необходимо вызывать перед рендерингом сцены, предварительно переключившись
     * в режим GL_SELECT. Вызывать напрямую не нужно. Он вызывается в pickObject().
     */
    private void renderGhostCubeSides(float r) {
        /*
         * Создаём временную область столкновения.
         * Для этого раздвигаем область игрока во все стороны.
         * Это нужно для определения области вокруг игрока радиусом R,
         * в пределах которой будет производиться поиск ближайших к игроку кубов.
         */
        CollisionBox box = player.collisionBox.grow(r, r, r);
        int x0 = (int)box.x0;
        int x1 = (int)box.x1;
        int y0 = (int)box.y0;
        int y1 = (int)box.y1;
        int z0 = (int)box.z0;
        int z1 = (int)box.z1;
        Frustum frustumObj = Frustum.getFrustum();
        Tesselator t = new Tesselator();
        glInitNames();

        //перебираем все блоки в площади box
        for (int x = x0; x <= x1; x++) {
            for (int y = y0; y <= y1; y++) {
                for (int z = z0; z <= z1; z++) {
                    /*
                     * Если в координатах мира [x,y,z] есть блок и он попадает во frustum,
                     * добавляем его ID в буфер выбора и рисуем невидимые грани.
                     * А так же добавляем в буфер ID каждой грани куба (блока).
                     * Если грань заблокирована другим блоком,
                     * она не будет нарисована и добавлена в буфер.
                     */
                    if (player.worldObj.getBlockId(x, y, z) > 0 &&
                            frustumObj.cubeInFrustum((float)x, (float)y, (float)z,
                                    player.worldObj.cubeSize)) {
                        /*
                         * Получаем номер (ID) ячейки с блоком в массиве.
                         * Это нужно для получения подробной информации о блоке,
                         * если игрок его выберет.
                         */
                        int id = player.worldObj.getArrayId(x, y, z);

                        /*
                         * glPushName() добавляет этот ID в буфер выбора.
                         * Иначе, выбрать блок будет нельзя.
                         */
                        glPushName(id);
                        for (int i = 0; i < 6; i++) {
                            /*
                             * Добавляем в буфер выбора ID каждой грани куба и рисуем ее.
                             * Рисовать нужно между вызовами glPushName() и glPopName(),
                             * иначе в буфер выбора ничего не добавляется.
                             */
                            glPushName(i);
                            t.clearQuads();

                            /*
                             * Метод RenderGhostCubeSide() проверяет, заблокирована ли
                             * текущая грань куба. Если нет - она будет нарисована и
                             * добавлена в буфер выбора. Если да, то предыдущий вызов
                             * glPushName() игнорируется, а в буфер выбора ничего не добавляется.
                             */
                            renderGhostCubeSide(t, x, y, z, i);
                            t.flushQuads();
                            glPopName();
                        }
                        //каждый вызов glPushName() должен завершаться вызовом glPopName()
                        glPopName();
                    }
                }
            }
        }
    }

    /*
     * pickObject()
     * Главный метод, который определяет, на какой куб (блок) смотрит игрок.
     * Этот метод необходимо вызывать перед рендерингом сцены.
     * Возвращаемые значения:
     * !=0:
     * Игрок смотрит на блок. Информация о блоке передана в идентификатор класса HitResult.
     * 0:
     * Игрок смотрит в пустоту или находится слишком далеко от ближайшего блока.
     */
    public int pickObject(float radius) {
        selectBuffer.clear();
        glSelectBuffer(selectBuffer);
        viewportBuffer.clear();
        glGetInteger(GL_VIEWPORT, viewportBuffer);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glRenderMode(GL_SELECT);
        gluPickMatrix((float) CubixCraft.windowWidth / 2.0f, (float) CubixCraft.windowHeight / 2.0f,
                2.0f, 2.0f, viewportBuffer);
        gluPerspective(75.0f, (float) CubixCraft.windowWidth / (float) CubixCraft.windowHeight,
                0.05f, 200.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(player.rotationRoll,  0.0f, 0.0f, 1.0f);
        glRotatef(player.rotationPitch, 1.0f, 0.0f, 0.0f);
        glRotatef(player.rotationYaw,   0.0f, 1.0f, 0.0f);
        glTranslatef(-player.positionX - player.getCameraOffsetZ(),
                -player.positionY - player.height,
                -player.positionZ - player.getCameraOffsetZ());

        renderGhostCubeSides(radius);

        //Получаем количество записей в буфере выбора.
        //Количество записей равно количеству выбранных объектов.
        hitsCount = glRenderMode(GL_RENDER);

        /*
         * Теперь нужно проанализировать получившийся буфер выбора.
         * Буфер выбора это массив типа IntBuffer.
         * Массив содержит список объектов, на которые смотрел игрок в момент вызова RenderGhostCubeSides().
         * В нашем случае, все "объекты" являются сторонами (гранями) одного
         * или нескольких блоков. Вся игра состоит из блоков и ничего другого выбрать нельзя.
         * Когда игрок смотрит на блок, то в буфер выбора записывается
         * информация не только о той стороне, которую игрок видит,
         * но и обо всех сторонах блока, через которые проходит луч.
         * То есть, луч проходит через все блоки насквозь.
         * Если за блоком находится еще один блок и луч в него попадает,
         * то информация о его сторонах тоже будет записана в буфер выбора.
         * То есть, если в пустоте висит одинокий блок и игрок на него смотрит,
         * то в буфер попадут, как минимум две его грани - та, которую
         * игрок видит и следующая в зависимости от угла разворота игрока.
         * Если игрок смотрит в самый угол блока, то в буфер могут попасть
         * три и более граней. Это означает, что необходимо перебрать массив
         * и найти ближайшую к игроку грань.
         * Для описания каждого выбранного игроком объекта, в массиве отводится
         * по несколько элементов, которые формируются следующим образом:
         * [0]: Количество имен в стеке (подробнее далее)
         * [1]: Минимальное расстояние до камеры
         * [2]: Максимальное расстояние до камеры
         * [3]: ID блока, которому принадлежит текущая грань. По этому ID можно
         *      обратиться к главному массиву в классе World и получить полную
         *      информацию о блоке.
         * [4]: ID самой грани. Это целочисленная константа от 0 до 5, обозначающая
         *      условную грань блока. С ее помощью можно определить, на какую
         *      именно сторону (грань) блока смотрит игрок.
         *      Список всех шести констант можно посмотреть в файле "Block.java".
         *
         * Имена стека это числа, которые ранее передавались как аргумент функции glPushName().
         * В данном случае, они расположены в ячейках [3] и [4]. Их количество может
         * быть разным и оно хранится в первой (нулевой) ячейке массива.
         * Но так как этих "имён" у нас всегда два, то проверять нулевую ячейку нет смысла.
         * Ее можно просто пропускать.
         * Начиная с ячейки массива с индексом [5], всё повторяется заново, но уже
         * для следующей грани. Затем [10], [15], [20] и т.д.
         * Таким образом перебираем все элементы массива и ищем ближайшую к игроку грань.
         */
        if (hitsCount > 0) {
            int closest = selectBuffer.get(1);
            int cubeId  = selectBuffer.get(3);
            int sideId  = selectBuffer.get(4);
            for (int i = 0; i < hitsCount; i++) {
                int id = i * 5;
                int dist = selectBuffer.get(id + 1);
                if (dist < closest) {
                    closest = dist;
                    cubeId = selectBuffer.get(id + 3);
                    sideId = selectBuffer.get(id + 4);
                }
            }

            hitResult = new HitResult(cubeId, sideId);
            hitResult.x = world.blocks[cubeId].positionX;
            hitResult.y = world.blocks[cubeId].positionY;
            hitResult.z = world.blocks[cubeId].positionZ;

            //Определяем координаты позиции рядом с блоком,
            //куда игрок может поставить новый блок.
            switch (hitResult.sideId) {
                case BLOCK_SIDE_FRONT:
                    hitResult.xNear = hitResult.x;
                    hitResult.yNear = hitResult.y;
                    hitResult.zNear = hitResult.z + 1;
                    break;
                case BLOCK_SIDE_BACK:
                    hitResult.xNear = hitResult.x;
                    hitResult.yNear = hitResult.y;
                    hitResult.zNear = hitResult.z - 1;
                    break;
                case BLOCK_SIDE_LEFT:
                    hitResult.xNear = hitResult.x - 1;
                    hitResult.yNear = hitResult.y;
                    hitResult.zNear = hitResult.z;
                    break;
                case BLOCK_SIDE_RIGHT:
                    hitResult.xNear = hitResult.x + 1;
                    hitResult.yNear = hitResult.y;
                    hitResult.zNear = hitResult.z;
                    break;
                case BLOCK_SIDE_TOP:
                    hitResult.xNear = hitResult.x;
                    hitResult.yNear = hitResult.y + 1;
                    hitResult.zNear = hitResult.z;
                    break;
                case BLOCK_SIDE_BOTTOM:
                    hitResult.xNear = hitResult.x;
                    hitResult.yNear = hitResult.y - 1;
                    hitResult.zNear = hitResult.z;
                    break;
            }
        } else {
            hitResult = null;
        }

        return hitsCount;
    }

    public String cubeSideIdToString(int sideId){
        switch (sideId){
            case BLOCK_SIDE_FRONT:
                return "Front";
            case BLOCK_SIDE_BACK:
                return "Back";
            case BLOCK_SIDE_LEFT:
                return "Left";
            case BLOCK_SIDE_RIGHT:
                return "Right";
            case BLOCK_SIDE_TOP:
                return "Top";
            case BLOCK_SIDE_BOTTOM:
                return "Bottom";
            default:
                return "Unknown";
        }
    }
}
