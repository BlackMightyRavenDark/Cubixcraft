package game.cubixcraft.world;

import game.cubixcraft.utils.Textures;
import game.cubixcraft.utils.Frustum;
import game.cubixcraft.block.Block;

import static game.cubixcraft.block.Block.*;
import static game.cubixcraft.world.Chunk.*;

import static org.lwjgl.opengl.GL11.*;

public class WorldRenderer {

    //количество чанков по осям X,Y,Z
    public int xChunks;
    public int yChunks;
    public int zChunks;

    //массив чанков
    public Chunk[] chunks;

    //Количество чанков, из которых состоит мир.
    //Это число определяет размер массива чанков.
    public int chunksCount;

    /*
     * Ограничение на максимальное количество рисующихся за один кадр чанков.
     * Это нужно для ускорения рендеринга каждого кадра.
     * Если игрой, в какой-то момент, была запрошена отрисовка большого
     * количества чанков одновременно, то они будут отрисованы (отрендерены)
     * не все сразу, а по MAX_REBUILDS_PER_FRAME за каждый кадр.
     * Значение переменной по-умолчанию равно 8.
     * Это немного сокращает время ожидания. Однако, игрок
     * будет видеть как вокруг него загружаются чанки.
     */
    public static int MAX_REBUILDS_PER_FRAME = 8;

    /*
     * Общее количество отрендеренных чанков.
     * Чанк считается отрендеренным, если в нем присутствует
     * хотя-бы один блок, не являющийся блоком воздуха.
     */
    private int totalChunksRendered;

    //Общее количество отрендеренных блоков
    private int totalBlocksRendered;

    //максимальное количество блоков,
    //которые можно построить в мире
    private int totalBlocksPossible;

    //Общее количество отрендеренных граней
    private int totalQuadsRendered;

    /*
     * Максимальное количество граней кубов (блоков),
     * которое может находиться в мире.
     * Это число находится простым умножением
     * максимального количества возможных блоков на 6.
     */
    private int totalQuadsPossible;

    public World world;

    public WorldRenderer(World world) {
        this.world = world;
    }

    /*
     * getArrayId()
     * Преобразовывает указанные координаты чанка в формат одномерного массива.
     * Возвращает ID ячейки массива, соответствующей переданным координатам.
     */
    public int getArrayId(int x, int y, int z) {
        return (xChunks * zChunks * y) + (zChunks * x) + z;
    }

    /*
     * createChunks()
     * Разбивает мир на чанки для ускорения рендеринга.
     */
    public void createChunks() {
        xChunks = world.sizeX / chunkSize;
        yChunks = world.sizeY / chunkSize;
        zChunks = world.sizeZ / chunkSize;

        chunksCount = xChunks * yChunks * zChunks;
        chunks = new Chunk[chunksCount];

        //создаём массив чанков таким же образом, как и массив блоков в классе мира
        for (int x = 0; x < xChunks; x++) {
            for (int y = 0; y < yChunks; y++) {
                for (int z = 0; z < zChunks; z++) {
                    //координаты первого блока в чанке
                    int x0 = x * chunkSize;
                    int y0 = y * chunkSize;
                    int z0 = z * chunkSize;
                    //координаты последнего блока в чанке
                    int x1 = (x + 1) * chunkSize;
                    int y1 = (y + 1) * chunkSize;
                    int z1 = (z + 1) * chunkSize;

                    int id = getArrayId(x, y, z);
                    chunks[id] = new Chunk(world, x0, y0, z0, x1, y1, z1);
                }
            }
        }

        totalChunksRendered = totalBlocksRendered = totalQuadsRendered = 0;
        totalBlocksPossible = chunkSize * chunkSize * chunkSize * chunksCount;
        totalQuadsPossible = totalBlocksPossible * 6;
    }

    /*
     * render()
     * Рендерит все объекты мира, которые попадают в поле видимости игрока (frustum).
     * Возвращает количество отрендеренных чанков.
     * Чанк считается отрендеренным, если в нем присутствует
     * хотя-бы один блок, не являющийся блоком воздуха.
     */
    public int render() {
        totalChunksRendered = totalBlocksRendered = totalQuadsRendered = 0;
        glColor3f(1.0f, 1.0f, 1.0f);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, Textures.textureBase);

        Frustum frustumObj = Frustum.getFrustum();

        int expiredCount = 0;
        for (int i = 0; i < chunksCount; i++) {
            if (frustumObj.cubeInFrustum(
                    (float)chunks[i].x0, (float)chunks[i].y0, (float)chunks[i].z0,
                    world.cubeSize * chunkSize)) {

                //если чанк просрочен и количество обновлённых в текущей итерации чанков меньше максимально
                //допустимого значения, то обновляем его.
                if (chunks[i].getIsExpired() && expiredCount < MAX_REBUILDS_PER_FRAME) {
                    chunks[i].rebuild();
                    expiredCount++;
                }

                int blocksRenderedInChunk = chunks[i].render();

                if (blocksRenderedInChunk > 0) {
                    totalBlocksRendered += blocksRenderedInChunk;
                    totalQuadsRendered += chunks[i].getQuadsRenderedCount();
                    totalChunksRendered++;
                }
            }
        }
        glDisable(GL_TEXTURE_2D);
        return totalChunksRendered;
    }

    /*
     * setAllChunksExpired()
     * Очищает листы всех чанков и
     * устанавливает флаг необходимости их перекомпиляции.
     */
    public void setAllChunksExpired() {
        for (Chunk chunk : chunks) {
            chunk.expire();
        }
        totalChunksRendered = totalBlocksRendered = totalQuadsRendered = Chunk.totalChunkUpdates = 0;
    }

    /*
     * blockChanged()
     * Уведомляет мир об изменении состояния блока в указанных координатах.
     */
    public void blockChanged(int x, int y, int z) {
        /*
         * Если изменился один из крайних блоков в чанке,
         * то необходимо так же обновить и соседний чанк.
         * Алгоритм хреновый и нуждается в тотальной переработке.
         */
        int x0 = (x - 1) / chunkSize;
        int y0 = (y - 1) / chunkSize;
        int z0 = (z - 1) / chunkSize;
        int x1 = (x + 1) / chunkSize;
        int y1 = (y + 1) / chunkSize;
        int z1 = (z + 1) / chunkSize;
        if (x0 < 0) {
            x0 = 0;
        }
        if (y0 < 0) {
            y0 = 0;
        }
        if (z0 < 0) {
            z0 = 0;
        }
        if (x1 >= xChunks) {
            x1 = xChunks - 1;
        }
        if (y1 >= yChunks) {
            y1 = yChunks - 1;
        }
        if (z1 >= zChunks) {
            z1 = zChunks - 1;
        }

        for (int xx = x0; xx <= x1; xx++) {
            for (int yy = y0; yy <= y1; yy++) {
                for (int zz = z0; zz <= z1; zz++) {
                    int id = getArrayId(xx, yy, zz);
                    chunks[id].expire();
                }
            }
        }

        /*
         * Получаем экземпляр изменившегося блока.
         * Если в остальном коде нет ошибок и нигде
         * ничего не перепутано, то этот идентификатор
         * содержит экземпляр нового блока,
         * на который был заменён старый блок.
         */
        Block blockOld = world.getBlock(x, y, z);

        //Если новый блок не является блоком воздуха,
        //то есть, если блок был именно поставлен, а не разрушен, то...
        if (blockOld != null && blockOld.getId() != BLOCK_AIR_ID) {
            Block blockOld1 = world.getBlock(x, y - 1, z);
            //если мы ставим его на блок травы, то меняем траву на землю
            if (blockOld1 != null && blockOld1.getId() == BLOCK_GRASS_ID) {
                world.setBlockWithoutNotify(x, y - 1, z, BLOCK_DIRT_ID);
                for (IWorldListener listener : world.getListeners()) {
                    listener.blockChanged(blockOld1);
                }

                //рекурсивно вызываем текущий метод еще раз,
                //так как изменение блока привело к изменению другого блока.
                blockChanged(x, y - 1, z);
            }

            //если мы пытаемся засунуть блок травы под какой-то другой блок,
            //то суваемая нами трава становится просто землёй.
            if (world.getBlockId(x, y + 1, z) != BLOCK_AIR_ID &&
                    blockOld.getId() == BLOCK_GRASS_ID) {
                world.setBlockWithoutNotify(x, y, z, BLOCK_DIRT_ID);
                for (IWorldListener listener : world.getListeners()) {
                    listener.blockChanged(blockOld);
                }
                blockChanged(x, y, z);
            }
        }
    }

    public int getTotalChunksRendered() {
        return totalChunksRendered;
    }

    public int getTotalBlocksRendered() {
        return totalBlocksRendered;
    }

    public int getTotalQuadsRendered() {
        return totalQuadsRendered;
    }

    public int getTotalBlocksPossible() {
        return totalBlocksPossible;
    }

    public int getTotalQuadsPossible() {
        return totalQuadsPossible;
    }
}
