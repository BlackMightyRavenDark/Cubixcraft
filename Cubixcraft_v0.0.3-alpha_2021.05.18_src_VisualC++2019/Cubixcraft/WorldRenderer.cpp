#include "WorldRenderer.h"
#include "block.h"
#include "chunk.h" 
#include "frustum.h"
#include "main.h"
#include "tessellator.h"
#include "textures.h"
#include "world.h"

CWorldRenderer::CWorldRenderer(CWorld* world) 
{
    this->world = world;

    maxRebuildsPerFrame = 8;

    //присваиваем дефолтные значения всем глобальным переменным,
    //чтобы IDE и компилятор не выдавали предупреждений.
    xChunks = yChunks = zChunks = 0;
    totalChunksRendered = totalBlocksRendered = totalQuadsRendered = totalChunkUpdates = 0;
    chunksCount = totalBlocksPossible = totalQuadsPossible = 0;
}

CWorldRenderer::~CWorldRenderer() 
{
    for (size_t i = 0; i < chunks.size(); i++) 
    {
        delete chunks[i];
    }
}

/*
 * GetArrayId()
 * Преобразовывает указанные координаты чанка в формат одномерного массива.
 * Возвращает ID ячейки массива, соответствующей переданным координатам.
 */
int CWorldRenderer::GetArrayId(int x, int y, int z) 
{
    return (xChunks * zChunks * y) + (zChunks * x) + z;
}

/*
 * CreateChunks()
 * Разбивает мир на чанки для ускорения рендеринга.
 */
void CWorldRenderer::CreateChunks()
{
    xChunks = world->sizeX / chunkSize;
    yChunks = world->sizeY / chunkSize;
    zChunks = world->sizeZ / chunkSize;

    chunksCount = xChunks * yChunks * zChunks;
    chunks.resize(chunksCount);

    //создаём массив чанков таким же образом, как и массив блоков в классе мира
    for (int x = 0; x < xChunks; x++)
    {
        for (int y = 0; y < yChunks; y++)
        {
            for (int z = 0; z < zChunks; z++)
            {
                //координаты первого блока в чанке
                int x0 = x * chunkSize;
                int y0 = y * chunkSize;
                int z0 = z * chunkSize;
                //координаты последнего блока в чанке
                int x1 = (x + 1) * chunkSize;
                int y1 = (y + 1) * chunkSize;
                int z1 = (z + 1) * chunkSize;

                int id = GetArrayId(x, y, z);
                chunks[id] = new CChunk(world, x0, y0, z0, x1, y1, z1);
            }
        }
    }

    totalBlocksPossible = chunkSize * chunkSize * chunkSize * chunksCount;
    totalQuadsPossible = totalBlocksPossible * 6;
}

/*
 * Render()
 * Рендерит все объекты мира, которые попадают в поле видимости игрока (frustum).
 * Возвращает количество отрендеренных чанков.
 * Чанк считается отрендеренным, если в нем присутствует
 * хотя-бы один блок, не являющийся блоком воздуха.
 */
int CWorldRenderer::Render() 
{
    totalChunksRendered = totalBlocksRendered = totalQuadsRendered = 0; 
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureBase);
    
    int expiredCount = 0;
    for (int i = 0; i < chunksCount; i++)
    {
        if (frustumObj->CubeInFrustum((float)chunks[i]->x0, (float)chunks[i]->y0, (float)chunks[i]->z0, 
                                      world->cubeSize * chunkSize)) 
        {
            //если чанк просрочен и количество обновлённых в текущей итерации чанков меньше максимально
            //допустимого значения, то обновляем его.
            if (chunks[i]->GetIsExpired() && expiredCount < maxRebuildsPerFrame)
            {
                chunks[i]->Rebuild();
                expiredCount++;
            }

            int blocksRenderedInChunk = chunks[i]->Render();

            if (blocksRenderedInChunk)
            {
                totalBlocksRendered += blocksRenderedInChunk;
                totalQuadsRendered += chunks[i]->GetQuadsRenderedCount();
                totalChunksRendered++;
            }
        }
    }

    glDisable(GL_TEXTURE_2D);
    
    return totalChunksRendered;
}

/*
 * SetAllChunksExpired()
 * Очищает листы всех чанков и
 * устанавливает флаг необходимости их перекомпиляции.
 */
void CWorldRenderer::SetAllChunksExpired()
{
    for (size_t i = 0; i < chunks.size(); i++) 
    {
        chunks[i]->Expire();
    }
    totalChunksRendered = totalBlocksRendered = totalQuadsRendered = totalChunkUpdates = 0;
}

/*
 * BlockChanged()
 * Уведомляет мир об изменении состояния блока в указанных координатах.
 */
void CWorldRenderer::BlockChanged(int x, int y, int z) 
{
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
    if (x0 < 0) 
    {
        x0 = 0;
    }
    if (y0 < 0) 
    {
        y0 = 0;
    }
    if (z0 < 0) 
    {
        z0 = 0;
    }
    if (x1 >= xChunks) 
    {
        x1 = xChunks - 1;
    }
    if (y1 >= yChunks) 
    {
        y1 = yChunks - 1;
    }
    if (z1 >= zChunks) 
    {
        z1 = zChunks - 1;
    }
 
    for (int xx = x0; xx <= x1; xx++) 
    {
        for (int yy = y0; yy <= y1; yy++) 
        {
            for (int zz = z0; zz <= z1; zz++)
            {
                int id = GetArrayId(xx, yy, zz);
                chunks[id]->Expire();
            }
        }
    }

    /*
     * Получаем ID изменившегося блока.
     * Если в остальном коде нет ошибок 
     * и нигде ничего не перепутано, то это ID
     * нового блока, на который был заменён старый блок.
     */
    char blockId = world->GetBlockId(x, y, z);

    //Если новый блок не является блоком воздуха,
    //то есть если блок был именно поставлен, а не разрушен, то...
    if (blockId != BLOCK_AIR_ID)
    {
        //если мы ставим его на блок травы, то меняем траву на землю
        if (world->GetBlockId(x, y - 1, z) == BLOCK_GRASS_ID)
        {
            world->ReplaceBlock(x, y - 1, z, BLOCK_DIRT_ID);
        }

        //если мы пытаемся засунуть блок травы под какой-то другой блок,
        //то суваемая нами трава становится просто землёй.
        if (blockId == BLOCK_GRASS_ID && world->GetBlockId(x, y + 1, z) != BLOCK_AIR_ID)
        {
            world->ReplaceBlock(x, y, z, BLOCK_DIRT_ID);
        }
    }
}

int CWorldRenderer::GetTotalChunksRendered()
{
    return totalChunksRendered;
}

int CWorldRenderer::GetTotalBlocksRendered()
{
    return totalBlocksRendered;
}

int CWorldRenderer::GetTotalQuadsRendered()
{
    return totalQuadsRendered;
}

int CWorldRenderer::GetTotalBlocksPossible()
{
    return totalBlocksPossible;
}

int CWorldRenderer::GetTotalQuadsPossible()
{
    return totalQuadsPossible;
}
