#include "worldrenderer.h"
#include "main.h"
#include "tesselator.h"
#include "textures.h"

WorldRenderer::WorldRenderer(World* world){
    worldObj = world;

    xChunks = worldObj->sizeX / chunkSize;
    yChunks = worldObj->sizeZ / chunkSize;
    zChunks = worldObj->sizeY / chunkSize;

    chunksCount = xChunks * yChunks * zChunks;
    chunks.resize(chunksCount);
    for (int x = 0; x < xChunks; x++){
        for (int y = 0; y < yChunks; y++){
            for (int z = 0; z < zChunks; z++){
                int x0 = x * chunkSize;
                int y0 = y * chunkSize;
                int z0 = z * chunkSize;
                int x1 = (x + 1) * chunkSize;
                int y1 = (y + 1) * chunkSize;
                int z1 = (z + 1) * chunkSize;
                if (x1 > worldObj->sizeX){
                    x1 = worldObj->sizeX;
                }
                if (y1 > worldObj->sizeZ){
                    y1 = worldObj->sizeZ;
                }
                if (z1 > worldObj->sizeY){
                    z1 = worldObj->sizeY;
                }
                int id = GetArrayId(x, y, z);
                chunks[id] = new Chunk(this, x0, y0, z0, x1, y1, z1);
            }
        }
    }
    totalBlocksRendered = 0;
    totalBlocksPossible = pow(chunkSize, 3.0) * chunksCount;
    tesselator = new Tesselator();
}

WorldRenderer::~WorldRenderer(){
    delete tesselator;
    for (int i = 0; i < chunksCount; i++){
        delete chunks[i];
    }
}

int WorldRenderer::GetArrayId(int x, int y, int z){
    return (x * zChunks) + (xChunks * zChunks * y) + z;
}

int WorldRenderer::Render(){
    chunksRendered = 0;
    totalBlocksRendered = 0;
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureGrass);
    for (int i = 0; i < chunksCount; i++){
        if (frustumObj->CubeInFrustum(chunks[i]->x0, chunks[i]->y0, chunks[i]->z0, worldObj->cubeSize * chunkSize)){
            chunks[i]->Render();
            totalBlocksRendered += chunks[i]->blocksRendered;
            if (chunks[i]->blocksRendered){
                chunksRendered++;
            }
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    return chunksRendered;
}

void WorldRenderer::RebuildAllChunks(){
    for (size_t i = 0; i < chunks.size(); i++){
        chunks[i]->needUpdate = true;
    }
}

void WorldRenderer::BlockChanged(int x, int y, int z){
    int x0 = (x - 1) / chunkSize;
    int y0 = (y - 1) / chunkSize;
    int z0 = (z - 1) / chunkSize;
    int x1 = (x + 1) / chunkSize;
    int y1 = (y + 1) / chunkSize;
    int z1 = (z + 1) / chunkSize;
    if (x0 < 0) {
        x0 = 0;
    }
    if (y0 < 0){
        y0 = 0;
    }
    if (z0 < 0){
        z0 = 0;
    }
    if (x1 >= xChunks){
        x1 = xChunks - 1;
    }
    if (y1 >= yChunks){
        y1 = yChunks - 1;
    }
    if (z1 >= zChunks){
        z1 = zChunks - 1;
    }
    for (int xx = x0; xx <= x1; xx++){
        for (int yy = y0; yy <= y1; yy++){
            for (int zz = z0; zz <= z1; zz++){
                int id = GetArrayId(xx, yy, zz);
                chunks[id]->needUpdate = true;
            }
        }
    }
}

int WorldRenderer::RenderBlockSide(Tesselator* t, float x, float y, float z, int side){
    int sidesRendered = 0;
    switch (side){
    case BLOCK_SIDE_BACK:
        if (!worldObj->IsBlockSolid(x, y, z - 1)){
            t->AddTextureVertexQuad(0.0, 0.0, x, y, z);
            t->AddTextureVertexQuad(1.0, 0.0, x + worldObj->blockWidth, y, z);
            t->AddTextureVertexQuad(1.0, 1.0, x + worldObj->blockWidth, y + worldObj->blockHeight, z);
            t->AddTextureVertexQuad(0.0, 1.0, x, y + worldObj->blockHeight, z);
            sidesRendered++;
        }
        break;
    case BLOCK_SIDE_FRONT:
        if (!worldObj->IsBlockSolid(x, y, z + 1)){
            t->AddTextureVertexQuad(0.0, 0.0, x, y, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(1.0, 0.0, x + worldObj->blockWidth, y, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(1.0, 1.0, x + worldObj->blockWidth, y + worldObj->blockHeight, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(0.0, 1.0, x, y + worldObj->blockHeight, z + worldObj->blockWidth);
            sidesRendered++;
        }
        break;
    case BLOCK_SIDE_LEFT:
        if (!worldObj->IsBlockSolid(x - 1, y, z)){
            t->AddTextureVertexQuad(0.0, 0.0, x, y, z);
            t->AddTextureVertexQuad(1.0, 0.0, x, y, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(1.0, 1.0, x, y + worldObj->blockHeight, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(0.0, 1.0, x, y + worldObj->blockHeight, z);
            sidesRendered++;
        }
        break;
    case BLOCK_SIDE_RIGHT:
        if (!worldObj->IsBlockSolid(x + 1, y, z)){
            t->AddTextureVertexQuad(0.0, 0.0, x + worldObj->blockWidth, y, z);
            t->AddTextureVertexQuad(1.0, 0.0, x + worldObj->blockWidth, y, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(1.0, 1.0, x + worldObj->blockWidth, y + worldObj->blockHeight, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(0.0, 1.0, x + worldObj->blockWidth, y + worldObj->blockHeight, z);
            sidesRendered++;
        }
        break;
    case BLOCK_SIDE_BOTTOM:
        if (!worldObj->IsBlockSolid(x, y - 1, z)){
            t->AddTextureVertexQuad(0.0, 0.0, x, y, z);
            t->AddTextureVertexQuad(1.0, 0.0, x + worldObj->blockWidth, y, z);
            t->AddTextureVertexQuad(1.0, 1.0, x + worldObj->blockWidth, y, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(0.0, 1.0, x, y, z + worldObj->blockWidth);
            sidesRendered++;
        }
        break;
    case BLOCK_SIDE_TOP:
        if (!worldObj->IsBlockSolid(x, y + 1, z)){
            t->AddTextureVertexQuad(0.0, 0.0, x, y + worldObj->blockHeight, z);
            t->AddTextureVertexQuad(1.0, 0.0, x + worldObj->blockWidth, y + worldObj->blockHeight, z);
            t->AddTextureVertexQuad(1.0, 1.0, x + worldObj->blockWidth, y + worldObj->blockHeight, z + worldObj->blockWidth);
            t->AddTextureVertexQuad(0.0, 1.0, x, y + worldObj->blockHeight, z + worldObj->blockWidth);
            sidesRendered++;
        }
        break;
    }
    return sidesRendered++;
}

void WorldRenderer::RenderBlockSideWithoutTexture(Tesselator* t, float x, float y, float z, int side){
    switch (side){
    case BLOCK_SIDE_BACK:
        if (!worldObj->IsBlockSolid(x, y, z - 1)){
            float x1 = x + worldObj->blockWidth;
            float y1 = y + worldObj->blockHeight;
            t->AddVertexQuad(x,  y,  z);
            t->AddVertexQuad(x1, y,  z);
            t->AddVertexQuad(x1, y1, z);
            t->AddVertexQuad(x,  y1, z);
        }
        break;
    case BLOCK_SIDE_FRONT:
        if (!worldObj->IsBlockSolid(x, y, z + 1)){
            float x1 = x + worldObj->blockWidth;
            float y1 = y + worldObj->blockHeight;
            float z1 = z + worldObj->blockWidth;
            t->AddVertexQuad(x,  y, z1);
            t->AddVertexQuad(x1, y, z1);
            t->AddVertexQuad(x1, y1, z1);
            t->AddVertexQuad(x, y1, z1);
        }
        break;
    case BLOCK_SIDE_LEFT:
        if (!worldObj->IsBlockSolid(x - 1, y, z)){
            float y1 = y + worldObj->blockHeight;
            float z1 = z + worldObj->blockWidth;
            t->AddVertexQuad(x, y, z);
            t->AddVertexQuad(x, y, z1);
            t->AddVertexQuad(x, y1, z1);
            t->AddVertexQuad(x, y1, z);
        }
        break;
    case BLOCK_SIDE_RIGHT:
        if (!worldObj->IsBlockSolid(x + 1, y, z)){
            float x1 = x + worldObj->blockWidth;
            float y1 = y + worldObj->blockHeight;
            float z1 = z + worldObj->blockWidth;
            t->AddVertexQuad(x1, y, z);
            t->AddVertexQuad(x1, y, z1);
            t->AddVertexQuad(x1, y1, z1);
            t->AddVertexQuad(x1, y1, z);
        }
        break;
    case BLOCK_SIDE_BOTTOM:
        if (!worldObj->IsBlockSolid(x, y - 1, z)){
            float x1 = x + worldObj->blockWidth;
            float z1 = z + worldObj->blockWidth;
            t->AddVertexQuad(x, y, z);
            t->AddVertexQuad(x1, y, z);
            t->AddVertexQuad(x1, y, z1);
            t->AddVertexQuad(x, y, z1);
        }
        break;
    case BLOCK_SIDE_TOP:
        if (!worldObj->IsBlockSolid(x, y + 1, z)){
            float x1 = x + worldObj->blockWidth;
            float y1 = y + worldObj->blockHeight;
            float z1 = z + worldObj->blockWidth;
            t->AddVertexQuad(x, y1, z);
            t->AddVertexQuad(x1, y1, z);
            t->AddVertexQuad(x1, y1, z1);
            t->AddVertexQuad(x, y1, z1);
        }
        break;
    }
}
