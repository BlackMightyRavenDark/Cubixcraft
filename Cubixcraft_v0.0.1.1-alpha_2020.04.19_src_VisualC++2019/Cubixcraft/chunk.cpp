#include "chunk.h"
#include "tesselator.h"
#include "world.h"
#include "worldrenderer.h"

int chunkSize = 16;

Chunk::Chunk(WorldRenderer* worldRenderer, int x0, int y0, int z0, int x1, int y1, int z1) {
    this->x0 = x0;
    this->y0 = y0;
    this->z0 = z0;
    this->x1 = x1;
    this->y1 = y1;
    this->z1 = z1;
    renderer = worldRenderer;
    glList = glGenLists(1);
    blocksRendered = 0;

    //��� �������� ����� ���������� ���������� ���� ���� � TRUE,
    //����� ���� ������������� ��������������� ��� ����������.
    needUpdate = true;
}

Chunk::~Chunk() {
    if (glList) {
        glDeleteLists(glList, 1);
    }
}

/*
 * Rebuild()
 * ����������� ���� ����� ��� ��������, ���������� �� ��� ������.
 * �������� ������ ������������� ��� ������.
 */
int Chunk::Rebuild() {
    glNewList(glList, GL_COMPILE);
    blocksRendered = 0;
    Tesselator* tesselator = new Tesselator();

    //�������� �� ���� ������ ������ �����
    for (int x = x0; x < x1; x++) {
        for (int y = y0; y < y1; y++) {
            for (int z = z0; z < z1; z++) {
                int id = renderer->worldObj->GetBlockId(x, y, z);

                //���� ID ����� �� 0 - �������� ���
                if (id) {
                    int res = 0;
                    for (int i = 0; i < 6; i++) {
                        res += renderer->RenderBlockSide(tesselator, (float)x, (float)y, (float)z, i);
                    }

                    //���� ����������� ����-�� ���� ����� �����,
                    //�� ����������� ���������� ������������� ������ �� �������
                    if (res) {
                        blocksRendered++;
                    }
                }
            }
        }
    }
    tesselator->FlushTextureQuads();
    delete tesselator;
    glEndList();
    return blocksRendered;
}

/*
 * Render()
 * ����������� ����, �������������� ������������� ��� ����, ���� ��� ����������.
 */
int Chunk::Render() {
    if (needUpdate) {
        Rebuild();
        needUpdate = false;
    }
    glCallList(glList);
    return blocksRendered;
}
