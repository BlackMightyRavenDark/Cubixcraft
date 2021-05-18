#include "chunk.h"
#include "block.h"
#include "tessellator.h"
#include "world.h"
#include "WorldRenderer.h"

int chunkSize = 16;
int totalChunkUpdates = 0;

CChunk::CChunk(CWorld* world, int x0, int y0, int z0, int x1, int y1, int z1) 
{
    this->x0 = x0;
    this->y0 = y0;
    this->z0 = z0;
    this->x1 = x1;
    this->y1 = y1;
    this->z1 = z1;

    this->world = world;
    glList = glGenLists(1);

    blocksRenderedCount = quadsRenderedCount = 0;

    //��� �������� ����� ���������� ���������� ���� ���� � TRUE,
    //����� ���� ������������� ��������������� ��� ����������.
    expired = true;
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
 * ����������� ���� ����� ��� ��������, ���������� �� ��� ������.
 * �������� ������ ������������� ����� ����������� ������� ������.
 * ���������� ���������� ������������� ������ � �����.
 */
int CChunk::Rebuild()
{
    totalChunkUpdates++;

    glNewList(glList, GL_COMPILE);
    
    blocksRenderedCount = quadsRenderedCount = 0;

    CTessellator* tessellator = new CTessellator();

    //�������� �� ���� ������ ������ �����
    for (int x = x0; x < x1; x++)
    {
        for (int y = y0; y < y1; y++)
        {
            for (int z = z0; z < z1; z++)
            {
                char blockId = world->GetBlockId(x, y, z);

                //���� ���� �� �������� ��������, ����� �������� ���
                if (blockId != BLOCK_AIR_ID)
                {
                    CBlock* b = world->CreateBlock(blockId);
                    b->SetPosition(x, y, z);
                    int sidesRendered = b->Render(tessellator);
                    delete b;

                    //���� ����������� ����-�� ���� ����� �����,
                    //�� ����������� ���������� ������������� ������ �� �������
                    //� ��������� ���������� ������������� ������ � �����.
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
    
    expired = false;
    return blocksRenderedCount;
}

/*
 * Render()
 * �������� ���� � ���������� ���������� ������������� ������.
 */
int CChunk::Render()
{
    if (blocksRenderedCount)
        glCallList(glList);
    return blocksRenderedCount;
}

/*
 * Expire()
 * ������� ���� ����� � ������ ��� ������.
 * ����� ���� �������� ������.
 * ���������� ID ������ ������� �����.
 */
int CChunk::Expire()
{
    if (glList)
    {
        glDeleteLists(glList, 1);
    }
    glList = glGenLists(1);
    expired = true;
    blocksRenderedCount = quadsRenderedCount = 0;
    return glList;
}

bool CChunk::GetIsExpired()
{
    return expired;
}

int CChunk::GetQuadsRenderedCount()
{
    return quadsRenderedCount;
}
