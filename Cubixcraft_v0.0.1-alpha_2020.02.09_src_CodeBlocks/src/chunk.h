#pragma once
#include "worldrenderer.h"
#include <GL/gl.h>

extern int chunkSize;

class WorldRenderer;

class Chunk {
public:
    int x0;
    int y0;
    int z0;
    int x1;
    int y1;
    int z1;

    int blocksRendered;

    bool needUpdate;

    GLuint glList;

    Chunk(WorldRenderer* worldRenderer, int x0, int y0, int z0, int x1, int y1, int z1);
    ~Chunk();
    int Rebuild();
    int Render();

private:
    WorldRenderer* renderer;
};
