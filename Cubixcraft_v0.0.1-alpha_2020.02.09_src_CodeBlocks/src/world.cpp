#include "world.h"
#include "main.h"
#include "font.h"
#include "frustum.h"
#include "textures.h"

World::World(int xSize, int ySize, int zSize, std::string name){
    this->name = name;
    cubeSize    = 1.0;
    blockWidth  = 1.0;
    blockHeight = 1.0;
    sizeX = xSize;
    sizeY = ySize;
    sizeZ = zSize; //depth
    blocksLength = sizeX * sizeY * sizeZ;
    blockIds = nullptr;
}

World::~World(){
    std::cout << "Destructing world..." << std::endl;
    Save();
    DestroyBlocks();
}

void World::DestroyBlocks(){
    if (blockIds){
        free(blockIds);
    }
    for (size_t i = 0; i < blocks.size(); i++){
        delete blocks[i];
    }
    blocks.resize(0);
}

void World::GenerateMap(){
    std::cout << "Generating map " << sizeX << "x" << sizeZ << "x" << sizeY;
    DestroyBlocks();
    blocksLength = sizeX * sizeY * sizeZ;
    std::cout << ", " << blocksLength << " blocks total" << std::endl;
    blockIds = (char*)malloc(sizeof(char) * blocksLength);
    blocks.resize(blocksLength);
    for (int y = 0; y < sizeZ; y++){
        for (int x = 0; x < sizeX; x++){
            for (int z = 0; z < sizeY; z++){
                SetBlock(x, y, z, y < 16 ? textureGrass : 0);
            }
        }
    }
}

int World::GetArrayId(int x, int y, int z){
    return (x * sizeY) + (sizeX * sizeY * y) + z;
}

Block* World::SetBlock(int x, int y, int z, char blockId){
    if (!IsBlockInWorld(x, y, z)){
        std::cout << "The block [" << x << "," << y << "," << z << "] is outside of the world" << std::endl;
        return nullptr;
    }
    int id = GetArrayId(x, y, z);
    blockIds[id] = blockId > 0 ? blockId - 1 : blockId;
    if (!blocks[id]){
        blocks[id] = new Block(this, blockId > 0 ? blockId - 1 : blockId, blockId, x,y,z);
    } else {
        blocks[id]->SetPosition(x, y, z);
        blocks[id]->SetId(blockIds[id]);
        blocks[id]->textureId = (GLuint)blockId;
    }
    return blocks[id];
}

Block* World::GetBlock(int x, int y, int z) {
    if (!IsBlockInWorld(x, y, z)){
        return 0;
    }
    int id = GetArrayId(x, y, z);
    return blocks[id];
}

char World::GetBlockId(int x, int y, int z) {
    if (!IsBlockInWorld(x, y, z)){
        return 0;
    }
    int id = GetArrayId(x, y, z);
    return blockIds[id];
}

bool World::IsBlockInWorld(int x, int y, int z){
    return x >= 0 && y >= 0 && z >= 0 && x < sizeX && y < sizeZ && z < sizeY;
}

std::vector<Block*> World::GetBlocksInBox(CollisionBox* box){
    std::vector<Block*> a;
    int x0 = (int)box->x0;
    int y0 = (int)box->y0;
    int z0 = (int)box->z0;
    int x1 = (int)box->x1;
    int y1 = (int)box->y1;
    int z1 = (int)box->z1;
    for (int x = x0; x <= x1; x++){
        for (int y = y0; y <= y1; y++){
            for (int z = z0; z <= z1; z++){
                Block* b = GetBlock(x, y, z);
                if (b && b->GetId()){
                    a.resize(a.size() + 1);
                    a[a.size() - 1] = b;
                }
            }
        }
    }
    return a;
}

bool World::IsBlockSolid(int x, int y, int z){
    return GetBlockId(x,y,z);
}

std::string World::GetName(){
    return name;
}



void World::Save(){
    std::cout << "Saving world..." << std::endl;
    std::string fn = "saves/" + name + ".dat";

    DWORD attr = GetFileAttributes((char*)"saves");
    if ((attr == INVALID_FILE_ATTRIBUTES) || ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0)){
        CreateDirectory((char*)"saves", NULL);
    }

    HANDLE fh = CreateFile(fn.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (fh){
        DWORD dw;
        WriteFile(fh, &sizeX, sizeof(short), &dw, nullptr);
        WriteFile(fh, &sizeY, sizeof(short), &dw, nullptr);
        WriteFile(fh, &sizeZ, sizeof(short), &dw, nullptr);
        WriteFile(fh, blockIds, blocksLength, &dw, nullptr);
        CloseHandle(fh);
    }
}

bool World::Load(){
    std::cout << "Loading world: " << name << "..." << std::endl;
    std::string s = "saves/" + name + ".dat";

    HANDLE fh = CreateFile(s.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (fh && fh != INVALID_HANDLE_VALUE){
        DWORD dw;
        short x;
        bool res = ReadFile(fh, &x, sizeof(short), &dw, nullptr);
        if (!res){
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        short y;
        res = ReadFile(fh, &y, sizeof(short), &dw, nullptr);
        if (!res){
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        short z;
        res = ReadFile(fh, &z, sizeof(short), &dw, nullptr);
        if (!res){
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        blocksLength = x * y * z;
        DestroyBlocks();
        blockIds = (char*)malloc(sizeof(char) * blocksLength);
        res = ReadFile(fh, blockIds, blocksLength, &dw, nullptr);
        CloseHandle(fh);
        if (res){
            sizeX = x;
            sizeY = y;
            sizeZ = z;
            blocks.resize(blocksLength);
            int i = 0;
            for (int yy = 0; yy < sizeZ; yy++){
                for (int xx = 0; xx < sizeX; xx++){
                    for (int zz = 0; zz < sizeY; zz++){
                        blocks[i] = new Block(this, blockIds[i], textureGrass, xx, yy, zz);
                        i++;
                    }
                }
            }
        } else {
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        return res;
    } else {
        std::cout << "Load failed!" << std::endl;
        return false;
    }
    return true;
}
