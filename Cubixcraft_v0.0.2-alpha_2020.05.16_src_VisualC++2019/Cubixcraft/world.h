#pragma once;
#include <Windows.h>
#include <malloc.h>
#include <vector>
#include <iostream>
#include <codecvt>
#include <GL/gl.h>

class CBlock;
class CCollisionBox;

class CWorld {
public:
    float cubeSize;
    float blockWidth;
    float blockHeight;

    //размер мира в блоках по оси X
    short sizeX;
    //размер мира в блоках по оси Z
    short sizeY;
    //размер мира в блоках по оси Y.
    //ќна же - depth. ѕо этой оси игрок прыгает.
    short sizeZ;

    void (*onBlockPlaced)(CBlock* newBlock);
    void (*onBlockDestroyed)(CBlock* block);
    void (*onBlockChanged)(CBlock* oldBlock);

    CWorld(short xSize, short ySize, short zSize, std::wstring name);
    ~CWorld();

    void GenerateMap();
    int GetArrayId(int x, int y, int z);
    bool SetBlock(int x, int y, int z, CBlock* block);
    bool SetBlockWithoutNotify(int x, int y, int z, CBlock* block);
    bool ReplaceBlock(int x, int y, int z, CBlock* block);
    CBlock* GetBlock(int x, int y, int z);
    CBlock* GetBlock(int arrayIndex);
    char GetBlockId(int x, int y, int z);
    CBlock* CreateBlock(char id);
    void DestroyBlock(int x, int y, int z);
    bool IsBlockInWorld(int x, int y, int z);
    std::vector<CBlock*> GetBlocksInBox(CCollisionBox* box);
    bool IsBlockSolid(int x, int y, int z);
    std::wstring GetName();
    void SetSavingDirectory(std::wstring dir);
    void Save();
    bool Load();

private:
    //название мира
    std::wstring name;

    //директори€ дл€ сохранени€ и загрузки
    std::wstring savingDirectory;

    //массив, содержащий ID всех блоков
    char* blockIds;

    //массив указателей на экземпл€ры блоков
    std::vector<CBlock*> blocks;

    //ћаксимальное количество блоков в мире.
    //Ёто число определ€ет размеры массивов.
    int blocksLength;

    void DestroyBlocks();
};
