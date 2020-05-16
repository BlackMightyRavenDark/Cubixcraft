#include "chunk.h"
#include "block.h"
#include "tessellator.h"
#include "world.h"
#include "worldrenderer.h"

int chunkSize = 16;
int totalChunkUpdates = 0;

CChunk::CChunk(CWorldRenderer* worldRenderer, int x0, int y0, int z0, int x1, int y1, int z1) 
{
    this->x0 = x0;
    this->y0 = y0;
    this->z0 = z0;
    this->x1 = x1;
    this->y1 = y1;
    this->z1 = z1;
    renderer = worldRenderer;
    glList = glGenLists(1);

    blocksRenderedCount = quadsRenderedCount = 0;

    //ѕри создании чанка необходимо установить этот флаг в TRUE,
    //чтобы лист автоматически скомпилировалс€ при рендеринге.
    isExpired = true;
}

CChunk::~CChunk() 
{
    if (glList) 
    {
        glDeleteLists(glList, 1);
    }
}

/*
 * Rebuild()
 *  омпилирует лист чанка без проверки, необходимо ли это делать.
 * ѕроверка должна производитьс€ вне метода.
 * ¬озвращает количество отрендеренных блоков в чанке.
 */
int CChunk::Rebuild()
{
    totalChunkUpdates++;
    glNewList(glList, GL_COMPILE);
    blocksRenderedCount = quadsRenderedCount = 0;

    CTessellator* tessellator = new CTessellator();

    //проходим по всем блокам внутри чанка
    for (int x = x0; x < x1; x++)
    {
        for (int y = y0; y < y1; y++)
        {
            for (int z = z0; z < z1; z++)
            {
                CBlock* b = renderer->worldObj->GetBlock(x, y, z);

                //если блок не €вл€етс€ воздухом, тогда рендерим его
                if (b->GetId() != BLOCK_AIR_ID)
                {
                    int sidesRendered = b->Render(tessellator);

                    //≈сли отрендерена хот€-бы одна грань блока,
                    //то увеличиваем количество отрендеренных блоков на единицу
                    //и суммируем количество отрендеренных граней в чанке.
                    if (sidesRendered)
                    {
                        blocksRenderedCount++;
                        quadsRenderedCount += sidesRendered;
                    }
                }
            }
        }
    }
    tessellator->FlushTextureQuads();
    delete tessellator;
    glEndList();
    isExpired = false;
    return blocksRenderedCount;
}

/*
 * Render()
 * –ендерит чанк и возвращает количество отрендеренных блоков.
 */
int CChunk::Render()
{
    glCallList(glList);
    return blocksRenderedCount;

}

/*
 * ExpireThisChunk()
 * ”дал€ет лист чанка и создаЄт его заново.
 * Ќовый лист создаЄтс€ пустым.
 */
int CChunk::ExpireThisChunk()
{
    if (glList)
    {
        glDeleteLists(glList, 1);
    }
    glList = glGenLists(1);
    isExpired = true;
    return glList;
}

bool CChunk::GetIsExpired()
{
    return isExpired;
}

int CChunk::GetQuadsRenderedCount()
{
    return quadsRenderedCount;
}
