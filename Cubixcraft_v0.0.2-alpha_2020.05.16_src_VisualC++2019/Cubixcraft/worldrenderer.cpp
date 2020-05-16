#include "worldrenderer.h"
#include "block.h"
#include "chunk.h"
#include "frustum.h"
#include "main.h"
#include "tessellator.h"
#include "textures.h"
#include "world.h"

CWorldRenderer::CWorldRenderer(CWorld* world) 
{
    worldObj = world;

    //��������� ��� �� ����� ��� ��������� ����������
    xChunks = worldObj->sizeX / chunkSize;
    yChunks = worldObj->sizeZ / chunkSize;
    zChunks = worldObj->sizeY / chunkSize;

    chunksCount = xChunks * yChunks * zChunks;
    chunks.resize(chunksCount);

    //������ ������ ������ ����� �� �������, ��� � ������ ������ � ������ ����
    for (int x = 0; x < xChunks; x++) 
    {
        for (int y = 0; y < yChunks; y++) 
        {
            for (int z = 0; z < zChunks; z++) 
            {
                //���������� ������� ����� � �����
                int x0 = x * chunkSize;
                int y0 = y * chunkSize;
                int z0 = z * chunkSize;
                //���������� ���������� ����� � �����
                int x1 = (x + 1) * chunkSize;
                int y1 = (y + 1) * chunkSize;
                int z1 = (z + 1) * chunkSize;
                
                int id = GetArrayId(x, y, z);
                chunks[id] = new CChunk(this, x0, y0, z0, x1, y1, z1);
            }
        }
    }
    maxRebuildsPerFrame = 8;
    totalChunksRendered = totalBlocksRendered = totalQuadsRendered = totalChunkUpdates = 0;
    totalBlocksPossible = (int)(powf((float)chunkSize, 3.0f) * chunksCount);
    totalQuadsPossible = totalBlocksPossible * 6;
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
 * ��������������� ��������� ���������� ����� � ������ ����������� �������.
 * ���������� ID ������ �������, ��������������� ���������� �����������.
 */
int CWorldRenderer::GetArrayId(int x, int y, int z) 
{
    return (x * zChunks) + (xChunks * zChunks * y) + z;
}

/*
 * Render()
 * �������� ��� ������� ����, ������� �������� � ���� ��������� ������ (frustum).
 * ���������� ���������� ������������� ������.
 * ���� ��������� �������������, ���� � ��� ������������
 * ����-�� ���� ����, �� ���������� ������ �������.
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
                                      worldObj->cubeSize * chunkSize)) 
        {
            //���� ���� ��������� � ���������� ���������� ������ ������ �����������
            //����������� ��������, �� ��������� ���.
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
 * ������� ����� ���� ������ �
 * ������������� ���� ������������� �� ��������������.
 */
void CWorldRenderer::SetAllChunksExpired()
{
    for (size_t i = 0; i < chunks.size(); i++) 
    {
        chunks[i]->ExpireThisChunk();
    }
}

/*
 * BlockChanged()
 * ���������� ��� �� ��������� ��������� ����� � ��������� �����������.
 */
void CWorldRenderer::BlockChanged(int x, int y, int z) 
{
    /*
     * ���� ��������� ���� �� ������� ������ � �����,
     * �� ���������� ��� �� �������� � �������� ����.
     * �������� �������� � ��������� � ��������� �����������.
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
                chunks[id]->ExpireThisChunk();
            }
        }
    }

    /*
     * �������� ��������� ������������� �����.
     * ���� � ��������� ���� ��� ������ � �����
     * ������ �� ����������, �� ���� ���������
     * �������� ��������� ������ �����,
     * �� ������� ��� ������ ������ ����.
     */
    CBlock* block = worldObj->GetBlock(x, y, z);

    //���� ����� ���� �� �������� ������ �������,
    //�� ���� ���� ���� ��� ������ ���������, � �� ��������, ��...
    if (block->GetId() != BLOCK_AIR_ID)
    {
        //���� �� ������ ��� �� ���� �����, �� ������ ����� �� �����
        if (worldObj->GetBlockId(x, y - 1, z) == BLOCK_GRASS_ID)
        {
            worldObj->ReplaceBlock(x, y - 1, z, worldObj->CreateBlock(BLOCK_DIRT_ID));
        }

        //���� �� �������� �������� ���� ����� ��� �����-�� ������ ����,
        //�� �������� ���� ����� ���������� ������ �����.
        if (block->GetId() == BLOCK_GRASS_ID && worldObj->GetBlockId(x, y + 1, z) != BLOCK_AIR_ID)
        {
            worldObj->ReplaceBlock(x, y, z, worldObj->CreateBlock(BLOCK_DIRT_ID));
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
