#pragma once
#include <Windows.h>
#include <malloc.h>
#include <vector>
#include <iostream>
#include <GL/gl.h>

class CBlock;
class CCollisionBox;
class CWorldRenderer;

class CWorld {
public:
    float cubeSize;
    float blockWidth;
    float blockHeight;

    //размер мира в блоках по осям X,Y,Z
    int sizeX;
    int sizeY; //она же - depth.
    int sizeZ;

    //события, происходящие в мире
    void (*onBlockPlaced)(int x, int y, int z, char newBlockId);
    void (*onBlockDestroyed)(int x, int y, int z, char wreckedBlockId);
    void (*onBlockChanged)(int x, int y, int z, char newBlockId, char oldBlockId);

    CWorld(int xSize, int ySize, int zSize, std::wstring name);
    ~CWorld();

    void GenerateMap();
    int GetArrayId(int x, int y, int z);
    bool SetBlock(int x, int y, int z, char blockId);
    bool SetBlockWithoutNotify(int x, int y, int z, char blockId);
    bool ReplaceBlock(int x, int y, int z, char newBlockId);
    CBlock* GetBlock(int x, int y, int z);
    CBlock* GetBlock(int arrayIndex);
    char GetBlockId(int x, int y, int z);
    CBlock* CreateBlock(char id);
    void DestroyBlock(int x, int y, int z);
    bool IsBlockInWorld(int x, int y, int z);
    std::vector<CBlock*> GetBlocksInBox(CCollisionBox* box);
    bool IsBlockSolid(int x, int y, int z);
    CWorldRenderer* GetRenderer();
    std::wstring GetName();
    void SetSavingDirectory(std::wstring dir);
    void Save();
    bool Load();

private:
    //название мира
    std::wstring name;

    //директория для сохранения и загрузки
    std::wstring savingDirectory;

    //массив, содержащий ID всех блоков
    char* blockIds;

    //Максимальное количество блоков в мире.
    //Это число определяет размер массива.
    int blocksLength;

    CWorldRenderer* renderer;

    void DestroyBlocks();
};
