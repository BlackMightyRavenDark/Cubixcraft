#include "worldrenderer.h"
#include "block.h"
#include "chunk.h"
#include "frustum.h"
#include "main.h"
#include "tesselator.h"
#include "textures.h"
#include "world.h"

WorldRenderer::WorldRenderer(World* world) {
    worldObj = world;

    //разбиваем мир на чанки дл€ ускорени€ рендеринга
    xChunks = worldObj->sizeX / chunkSize;
    yChunks = worldObj->sizeZ / chunkSize;
    zChunks = worldObj->sizeY / chunkSize;

    chunksCount = xChunks * yChunks * zChunks;
    chunks.resize(chunksCount);

    //создаЄм массив чанков таким же образом, как и массив блоков в классе мира
    for (int x = 0; x < xChunks; x++) {
        for (int y = 0; y < yChunks; y++) {
            for (int z = 0; z < zChunks; z++) {
                int x0 = x * chunkSize;
                int y0 = y * chunkSize;
                int z0 = z * chunkSize;
                int x1 = (x + 1) * chunkSize;
                int y1 = (y + 1) * chunkSize;
                int z1 = (z + 1) * chunkSize;
                if (x1 > worldObj->sizeX) {
                    x1 = worldObj->sizeX;
                }
                if (y1 > worldObj->sizeZ) {
                    y1 = worldObj->sizeZ;
                }
                if (z1 > worldObj->sizeY) {
                    z1 = worldObj->sizeY;
                }
                int id = GetArrayId(x, y, z);
                chunks[id] = new Chunk(this, x0, y0, z0, x1, y1, z1);
            }
        }
    }
    chunksRendered = totalBlocksRendered = 0;
    totalBlocksPossible = (int)(powf((float)chunkSize, 3.0f) * chunksCount);
}

WorldRenderer::~WorldRenderer() {
    for (size_t i = 0; i < chunks.size(); i++) {
        delete chunks[i];
    }
}

/*
 * GetArrayId()
 * ѕреобразовывает указанные координаты чанка в формат одномерного массива.
 * ¬озвращает ID €чейки массива, соответствующей переданным координатам.
 */
int WorldRenderer::GetArrayId(int x, int y, int z) {
    return (x * zChunks) + (xChunks * zChunks * y) + z;
}

/*
 * Render()
 * –ендерит все объекты мира, которые попадают в поле видимости игрока (frustum).
 */
int WorldRenderer::Render() {
    chunksRendered = totalBlocksRendered = 0;
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureGrass);
    for (int i = 0; i < chunksCount; i++) {
        if (frustumObj->CubeInFrustum((float)chunks[i]->x0, (float)chunks[i]->y0, (float)chunks[i]->z0, 
                                      worldObj->cubeSize * chunkSize)) {
            chunks[i]->Render();
            totalBlocksRendered += chunks[i]->blocksRendered;
            if (chunks[i]->blocksRendered) {
                chunksRendered++;
            }
        }
    }
    glDisable(GL_TEXTURE_2D);
    return chunksRendered;
}

/*
 * RebuildAllChunks()
 * ”станавливает флаг необходимости перекомпил€ции листов всех чанков.
 */
void WorldRenderer::RebuildAllChunks() {
    for (size_t i = 0; i < chunks.size(); i++) {
        chunks[i]->needUpdate = true;
    }
}

/*
 * BlockChanged()
 * ”ведомл€ет мир об изменении состо€ни€ блока в указанных координатах.
 */
void WorldRenderer::BlockChanged(int x, int y, int z) {
    /*
     * ≈сли изменилс€ один из крайних блоков в чанке,
     * то необходимо так же обновить и соседний чанк.
     * јлгоритм хреновый и нуждаетс€ в тотальной переработке.
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
                int id = GetArrayId(xx, yy, zz);
                chunks[id]->needUpdate = true;
            }
        }
    }
}

/*
 * RenderBlockSide()
 * –ендерит сторону (грань) блока, если она не заблокирована другим блоком.
 */
int WorldRenderer::RenderBlockSide(Tesselator* t, float x, float y, float z, int side) {
    int sidesRendered = 0;
    switch (side) {
    case BLOCK_SIDE_BACK:
        if (!worldObj->IsBlockSolid((int)x, (int)y, (int)z - 1)) {
            t->AddTextureVertexQuad(0.0f, 0.0f, x, y, z);
            t->AddTextureVertexQuad(1.0f, 0.0f, x + worldObj->blockWidth, y, z);
            t->AddTextureVertexQuad(1.0f, 1.0f, x + worldObj->blockWidth, y + worldObj->blockHeight, z);
            t->AddTextureVertexQuad(0.0f, 1.0f, x, y + worldObj->blockHeight, z);
            sidesRendered++;
        }
        break;
    case BLOCK_SIDE_FRONT:
        if (!worldObj->IsBlockSolid((int)x, (int)y, (int)z + 1)) {
            t->AddTextureVertexQuad(0.0f, 0.0f, x, y, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(1.0f, 0.0f, x + worldObj->blockWidth, y, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(1.0f, 1.0f, x + worldObj->blockWidth, y + worldObj->blockHeight, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(0.0f, 1.0f, x, y + worldObj->blockHeight, z + worldObj->blockWidth);
            sidesRendered++;
        }
        break;
    case BLOCK_SIDE_LEFT:
        if (!worldObj->IsBlockSolid((int)x - 1, (int)y, (int)z)) {
            t->AddTextureVertexQuad(0.0f, 0.0f, x, y, z);
            t->AddTextureVertexQuad(1.0f, 0.0f, x, y, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(1.0f, 1.0f, x, y + worldObj->blockHeight, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(0.0f, 1.0f, x, y + worldObj->blockHeight, z);
            sidesRendered++;
        }
        break;
    case BLOCK_SIDE_RIGHT:
        if (!worldObj->IsBlockSolid((int)x + 1, (int)y, (int)z)) {
            t->AddTextureVertexQuad(0.0f, 0.0f, x + worldObj->blockWidth, y, z);
            t->AddTextureVertexQuad(1.0f, 0.0f, x + worldObj->blockWidth, y, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(1.0f, 1.0f, x + worldObj->blockWidth, y + worldObj->blockHeight, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(0.0f, 1.0f, x + worldObj->blockWidth, y + worldObj->blockHeight, z);
            sidesRendered++;
        }
        break;
    case BLOCK_SIDE_BOTTOM:
        if (!worldObj->IsBlockSolid((int)x, (int)y - 1, (int)z)) {
            t->AddTextureVertexQuad(0.0f, 0.0f, x, y, z);
            t->AddTextureVertexQuad(1.0f, 0.0f, x + worldObj->blockWidth, y, z);
            t->AddTextureVertexQuad(1.0f, 1.0f, x + worldObj->blockWidth, y, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(0.0f, 1.0f, x, y, z + worldObj->blockWidth);
            sidesRendered++;
        }
        break;
    case BLOCK_SIDE_TOP:
        if (!worldObj->IsBlockSolid((int)x, (int)y + 1, (int)z)) {
            t->AddTextureVertexQuad(0.0f, 0.0f, x, y + worldObj->blockHeight, z);
            t->AddTextureVertexQuad(1.0f, 0.0f, x + worldObj->blockWidth, y + worldObj->blockHeight, z);
            t->AddTextureVertexQuad(1.0f, 1.0f, x + worldObj->blockWidth, y + worldObj->blockHeight, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(0.0f, 1.0f, x, y + worldObj->blockHeight, z + worldObj->blockWidth);
            sidesRendered++;
        }
        break;
    }
    return sidesRendered++;
}
