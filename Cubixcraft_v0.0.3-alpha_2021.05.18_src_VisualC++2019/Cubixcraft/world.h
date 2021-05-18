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

    //������ ���� � ������ �� ���� X,Y,Z
    int sizeX;
    int sizeY; //��� �� - depth.
    int sizeZ;

    //�������, ������������ � ����
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
    //�������� ����
    std::wstring name;

    //���������� ��� ���������� � ��������
    std::wstring savingDirectory;

    //������, ���������� ID ���� ������
    char* blockIds;

    //������������ ���������� ������ � ����.
    //��� ����� ���������� ������ �������.
    int blocksLength;

    CWorldRenderer* renderer;

    void DestroyBlocks();
};
