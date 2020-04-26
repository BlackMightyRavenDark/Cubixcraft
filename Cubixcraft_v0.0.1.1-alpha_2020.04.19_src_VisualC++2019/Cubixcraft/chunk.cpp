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

    //ѕри создании чанка необходимо установить этот флаг в TRUE,
    //чтобы лист автоматически скомпилировалс€ при рендеринге.
    needUpdate = true;
}

Chunk::~Chunk() {
    if (glList) {
        glDeleteLists(glList, 1);
    }
}

/*
 * Rebuild()
 *  омпилирует лист чанка без проверки, необходимо ли это делать.
 * ѕроверка должна производитьс€ вне метода.
 */
int Chunk::Rebuild() {
    glNewList(glList, GL_COMPILE);
    blocksRendered = 0;
    Tesselator* tesselator = new Tesselator();

    //проходим по всем блокам внутри чанка
    for (int x = x0; x < x1; x++) {
        for (int y = y0; y < y1; y++) {
            for (int z = z0; z < z1; z++) {
                int id = renderer->worldObj->GetBlockId(x, y, z);

                //если ID блока не 0 - рендерим его
                if (id) {
                    int res = 0;
                    for (int i = 0; i < 6; i++) {
                        res += renderer->RenderBlockSide(tesselator, (float)x, (float)y, (float)z, i);
                    }

                    //если отрендерена хот€-бы одна грань блока,
                    //то увеличиваем количество отрендеренных блоков на единицу
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
 * ќтрендерить чанк, предварительно скомпилировав его лист, если это необходимо.
 */
int Chunk::Render() {
    if (needUpdate) {
        Rebuild();
        needUpdate = false;
    }
    glCallList(glList);
    return blocksRendered;
}
