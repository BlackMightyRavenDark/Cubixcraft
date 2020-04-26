#include <Windows.h>
#include <malloc.h>
#include <vector>
#include <iostream>
#include <codecvt>
#include <GL/gl.h>

class Block;
class CollisionBox;

class World {
public:
    float cubeSize;
    float blockWidth;
    float blockHeight;

    short sizeX;
    short sizeY;
    short sizeZ;

    char* blockIds;

    std::vector<Block*> blocks;
    int blocksLength;

    World(int xSize, int ySize, int zSize, std::wstring name);
    ~World();

    void GenerateMap();
    int GetArrayId(int x, int y, int z);
    Block* SetBlock(int x, int y, int z, char blockId);
    Block* GetBlock(int x, int y, int z);
    char GetBlockId(int x, int y, int z);
    bool IsBlockInWorld(int x, int y, int z);
    std::vector<Block*> GetBlocksInBox(CollisionBox* box);
    bool IsBlockSolid(int x, int y, int z);
    std::wstring GetName();
    void Save();
    bool Load();

private:
    std::wstring name;
    
    void DestroyBlocks();
};
