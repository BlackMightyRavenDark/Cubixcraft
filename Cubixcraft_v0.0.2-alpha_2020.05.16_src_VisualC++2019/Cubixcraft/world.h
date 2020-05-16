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

    //������ ���� � ������ �� ��� X
    short sizeX;
    //������ ���� � ������ �� ��� Z
    short sizeY;
    //������ ���� � ������ �� ��� Y.
    //��� �� - depth. �� ���� ��� ����� �������.
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
    //�������� ����
    std::wstring name;

    //���������� ��� ���������� � ��������
    std::wstring savingDirectory;

    //������, ���������� ID ���� ������
    char* blockIds;

    //������ ���������� �� ���������� ������
    std::vector<CBlock*> blocks;

    //������������ ���������� ������ � ����.
    //��� ����� ���������� ������� ��������.
    int blocksLength;

    void DestroyBlocks();
};
