package game.cubixcraft;

import static game.cubixcraft.Block.*;
import static game.cubixcraft.Chunk.*;
import static game.cubixcraft.Textures.*;

import static org.lwjgl.opengl.GL11.*;

public class WorldRenderer {

    public World worldObj;

    public int xChunks;
    public int yChunks;
    public int zChunks;

    public Chunk[] chunks;

    public int chunksCount;
    public int chunksRendered;
    public int totalBlocksRendered;
    public int totalBlocksPossible;
    
    public WorldRenderer(World world){
        worldObj = world;

        //разбиваем мир на чанки для ускорения рендеринга
        xChunks = worldObj.sizeX / chunkSize;
        yChunks = worldObj.sizeZ / chunkSize;
        zChunks = worldObj.sizeY / chunkSize;

        chunksCount = xChunks * yChunks * zChunks;
        chunks = new Chunk[chunksCount];

        //создаём массив чанков таким же образом, как и массив блоков в классе мира
        for (int x = 0; x < xChunks; x++) {
            for (int y = 0; y < yChunks; y++) {
                for (int z = 0; z < zChunks; z++) {
                    int x0 = x * chunkSize;
                    int y0 = y * chunkSize;
                    int z0 = z * chunkSize;
                    int x1 = (x + 1) * chunkSize;
                    int y1 = (y + 1) * chunkSize;
                    int z1 = (z + 1) * chunkSize;
                    int id = getArrayId(x, y, z);
                    chunks[id] = new Chunk(this, x0, y0, z0, x1, y1, z1);
                }
            }
        }
        chunksRendered = totalBlocksRendered = 0;
        totalBlocksPossible = (int)(Math.pow(chunkSize, 3) * chunksCount);
    }

    /*
     * render()
     * Рендерит все объекты мира, которые попадают в поле видимости игрока (frustum).
     */
    public int render(){
        chunksRendered = totalBlocksRendered = 0;
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureGrass);
        Frustum frustumObj = Frustum.getFrustum();
        for (int i = 0; i < chunksCount; i++) {
            if (frustumObj.cubeInFrustum(
                    (float)chunks[i].x0, (float)chunks[i].y0, (float)chunks[i].z0,
                    worldObj.cubeSize * chunkSize)) {
                chunks[i].render();
                totalBlocksRendered += chunks[i].blocksRendered;
                if (chunks[i].blocksRendered > 0) {
                    chunksRendered++;
                }
            }
        }
        glDisable(GL_TEXTURE_2D);
        return chunksRendered;
    }

    /*
     * rebuildAllChunks()
     * Устанавливает флаг необходимости перекомпиляции листов всех чанков.
     */
    public void rebuildAllChunks(){
        for (int i = 0; i < chunks.length; i++){
            chunks[i].needUpdate = true;
        }
    }

    /*
     * getArrayId()
     * Преобразовывает указанные координаты чанка в формат одномерного массива.
     * Возвращает ID ячейки массива, соответствующей переданным координатам.
     */
    public int getArrayId(int x, int y, int z){
        return (x * zChunks) + (xChunks * zChunks * y) + z;
    }

    /*
     * blockChanged()
     * Уведомляет мир об изменении состояния блока в указанных координатах.
     */
    public void blockChanged(int x, int y, int z){
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
                    chunks[id].needUpdate = true;
                }
            }
        }
    }

    /*
     * renderBlockSide()
     * Рендерит сторону (грань) блока, если она не заблокирована другим блоком.
     */
    public int renderBlockSide(Tesselator t, float x, float y, float z, int side){
        int sidesRendered = 0;
        switch (side) {
            case BLOCK_SIDE_BACK:
                if (!worldObj.isBlockSolid((int)x, (int)y, (int)z - 1)) {
                    t.addTextureVertexQuad(0.0f, 0.0f, x, y, z);
                    t.addTextureVertexQuad(1.0f, 0.0f, x + worldObj.blockWidth, y, z);
                    t.addTextureVertexQuad(1.0f, 1.0f, x + worldObj.blockWidth, y + worldObj.blockHeight, z);
                    t.addTextureVertexQuad(0.0f, 1.0f, x, y + worldObj.blockHeight, z);
                    sidesRendered++;
                }
                break;
            case BLOCK_SIDE_FRONT:
                if (!worldObj.isBlockSolid((int)x, (int)y, (int)z + 1)) {
                    t.addTextureVertexQuad(0.0f, 0.0f, x, y, z + worldObj.blockWidth);
                    t.addTextureVertexQuad(1.0f, 0.0f, x + worldObj.blockWidth, y, z + worldObj.blockWidth);
                    t.addTextureVertexQuad(1.0f, 1.0f, x + worldObj.blockWidth, y + worldObj.blockHeight, z + worldObj.blockWidth);
                    t.addTextureVertexQuad(0.0f, 1.0f, x, y + worldObj.blockHeight, z + worldObj.blockWidth);
                    sidesRendered++;
                }
                break;
            case BLOCK_SIDE_LEFT:
                if (!worldObj.isBlockSolid((int)x - 1, (int)y, (int)z)) {
                    t.addTextureVertexQuad(0.0f, 0.0f, x, y, z);
                    t.addTextureVertexQuad(1.0f, 0.0f, x, y, z + worldObj.blockWidth);
                    t.addTextureVertexQuad(1.0f, 1.0f, x, y + worldObj.blockHeight, z + worldObj.blockWidth);
                    t.addTextureVertexQuad(0.0f, 1.0f, x, y + worldObj.blockHeight, z);
                    sidesRendered++;
                }
                break;
            case BLOCK_SIDE_RIGHT:
                if (!worldObj.isBlockSolid((int)x + 1, (int)y, (int)z)) {
                    t.addTextureVertexQuad(0.0f, 0.0f, x + worldObj.blockWidth, y, z);
                    t.addTextureVertexQuad(1.0f, 0.0f, x + worldObj.blockWidth, y, z + worldObj.blockWidth);
                    t.addTextureVertexQuad(1.0f, 1.0f, x + worldObj.blockWidth, y + worldObj.blockHeight, z + worldObj.blockWidth);
                    t.addTextureVertexQuad(0.0f, 1.0f, x + worldObj.blockWidth, y + worldObj.blockHeight, z);
                    sidesRendered++;
                }
                break;
            case BLOCK_SIDE_BOTTOM:
                if (!worldObj.isBlockSolid((int)x, (int)y - 1, (int)z)) {
                    t.addTextureVertexQuad(0.0f, 0.0f, x, y, z);
                    t.addTextureVertexQuad(1.0f, 0.0f, x + worldObj.blockWidth, y, z);
                    t.addTextureVertexQuad(1.0f, 1.0f, x + worldObj.blockWidth, y, z + worldObj.blockWidth);
                    t.addTextureVertexQuad(0.0f, 1.0f, x, y, z + worldObj.blockWidth);
                    sidesRendered++;
                }
                break;
            case BLOCK_SIDE_TOP:
                if (!worldObj.isBlockSolid((int)x, (int)y + 1, (int)z)) {
                    t.addTextureVertexQuad(0.0f, 0.0f, x, y + worldObj.blockHeight, z);
                    t.addTextureVertexQuad(1.0f, 0.0f, x + worldObj.blockWidth, y + worldObj.blockHeight, z);
                    t.addTextureVertexQuad(1.0f, 1.0f, x + worldObj.blockWidth, y + worldObj.blockHeight, z + worldObj.blockWidth);
                    t.addTextureVertexQuad(0.0f, 1.0f, x, y + worldObj.blockHeight, z + worldObj.blockWidth);
                    sidesRendered++;
                }
                break;
        }
        return sidesRendered;
    }
}
