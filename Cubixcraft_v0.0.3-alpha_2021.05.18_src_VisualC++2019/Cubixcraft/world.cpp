#include "world.h"
#include "block.h"
#include "BlockAir.h"
#include "BlockDirt.h"
#include "BlockGrass.h"
#include "chunk.h"
#include "CollisionBox.h"
#include "WorldRenderer.h"

CWorld::CWorld(int xSize, int ySize, int zSize, std::wstring name)
{
    /*
     * ��� ����� ����������� � ���� <savingDirectory>/<name>.dat
     * ��-����� �� ����� ����������� � ��� ���������� ��������� �������!
     * � ��� ��, �������� ���� ��������� �� �����.
     */
    this->name = name;

    /*
     * ��� ���������� ���������� ������ ������. ��������� ��� ��������� ������.
     * � ������� ������ ������ ���� ����� �������!
     */
    cubeSize    = 1.0f;
    blockWidth  = 1.0f;
    blockHeight = 1.0f;

    sizeX = xSize;
    sizeY = ySize;
    sizeZ = zSize; 

    blocksLength = sizeX * sizeY * sizeZ;
    blockIds = nullptr;

    renderer = new CWorldRenderer(this);

    savingDirectory = L"";
}

CWorld::~CWorld() 
{
    Save();
    std::cout << "Destructing world..." << std::endl;
    DestroyBlocks();
    delete renderer;
}

/*
 * DestroyBlocks() 
 * ������� ��� ����� � ���� � ����������� ������.
 */
void CWorld::DestroyBlocks() 
{
    if (blockIds) 
    {
        free(blockIds);
        blockIds = nullptr;
    }
}

/*
 * GenerateMap()
 * ���������� ������� ���.
 */
void CWorld::GenerateMap() 
{
    blocksLength = sizeX * sizeY * sizeZ;
    std::cout << "Generating map " << sizeX << "x" << sizeY << "x" << sizeZ << ", " << blocksLength << " blocks total" << std::endl;
    DestroyBlocks();
    blockIds = (char*)malloc(sizeof(char) * blocksLength);

    //��������� ��������� ����, ������� �����
    for (int y = 0; y < sizeY; y++) 
    {
        for (int x = 0; x < sizeX; x++) 
        {
            for (int z = 0; z < sizeZ; z++) 
            {
                char b;
                if (y < 15)
                {
                    b = BLOCK_DIRT_ID;
                }
                else
                if (y < 16)
                {
                    b = BLOCK_GRASS_ID;
                }
                else
                {
                    b = BLOCK_AIR_ID;
                }
                SetBlockWithoutNotify(x, y, z, b);
            }
        }
    }

    //��������� ������� �� ������ ����� � ������ �����
    for (int x = 0; x < sizeX; x += chunkSize) 
    {
        for (int z = 0; z < sizeZ; z += chunkSize) 
        {
            SetBlockWithoutNotify(x, sizeY - 1, z, BLOCK_GRASS_ID);
            for (int y = sizeY - 2; y >= 16; y--)
            {
                SetBlockWithoutNotify(x, y, z, BLOCK_DIRT_ID);
            }
        }
    }

    //������� ��� �� �����
    renderer->CreateChunks();
}

/*
 * GetArrayId()
 * ��������� ��������� ���������� ���� � ������ ����������� �������.
 * ���������� ID ������ �������, ��������������� ���������� �����������.
 */
int CWorld::GetArrayId(int x, int y, int z) 
{
    return (sizeX * sizeZ * y) + (sizeZ * x) + z;
}

CBlock* CWorld::CreateBlock(char blockId)
{
    switch (blockId)
    {
        case BLOCK_AIR_ID:
            return new CBlockAir(this);
        case BLOCK_GRASS_ID:
            return new CBlockGrass(this, new POINT{ 0,0 });
        case BLOCK_DIRT_ID:
            return new CBlockDirt(this, new POINT{ 2,0 });
        default:
            return nullptr;
    }
}

/*
 * SetBlock()
 * �������� ���� �� ������� [x,y,z], ���������� �������
 * � �������� ����������� � ��������.
 */
bool CWorld::SetBlock(int x, int y, int z, char blockId) 
{
    //���� [x,y,z] ��������� �� ��������� ���� - ������� ��������� � �������.
    if (!IsBlockInWorld(x, y, z)) 
    {
        std::cout << "The block [" << x << "," << y << "," << z << "] is outside of the world!" << std::endl;
        return false;
    }
    int id = GetArrayId(x, y, z);
    blockIds[id] = blockId;

    //���� ����� ���� �� �������� ������ �������,
    //�� ����, ���� ���� ��� ������ ���������, � �� ��������,
    //�� ���������� �������.
    if (blockId != BLOCK_AIR_ID)
    {
        if (onBlockPlaced)
        {
            onBlockPlaced(x, y, z, blockId);
        }
    }

    //���������� �������� �� ��������� �����
    renderer->BlockChanged(x, y, z);

    return true;
}

/*
 * SetBlockWithoutNotify()
 * �������� ���� �� ������� [x,y,z] ��� ��������� ������� 
 * � ��� �������� ����������� � ��������.
 * ����������� ����� ���������� ������� ����� ������ ������� ������,
 * ����� ��������� ��������� ����� �� ����� �����.
 * ���� ����� ������������, ����� ����� � ���� ��������
 * ��� ������������� ������ (����� ����� �� �� ������ � �� ������).
 * � ��� ��, ������������ ��� ��������� �������� ����.
 */
bool CWorld::SetBlockWithoutNotify(int x, int y, int z, char blockId)
{
    //���� [x,y,z] ��������� �� ��������� ���� - ������� ��������� � �������.
    if (!IsBlockInWorld(x, y, z)) 
    {
        std::cout << "The block [" << x << "," << y << "," << z << "] is outside of the world!" << std::endl;
        return false;
    }

    int id = GetArrayId(x, y, z);
    blockIds[id] = blockId;

    return true;
}

/*
 * ReplaceBlock()
 * �������� ���� �� ������� [x,y,z], ���������� �������
 * � �������� ����������� � ��������.
 */
bool CWorld::ReplaceBlock(int x, int y, int z, char newBlockId)
{
    //���� [x,y,z] ��������� �� ��������� ���� - ������� ��������� � �������.
    if (!IsBlockInWorld(x, y, z))
    {
        std::cout << "The block [" << x << "," << y << "," << z << "] is outside of the world!" << std::endl;
        return false;
    }

    int id = GetArrayId(x, y, z);
    char oldBlockId = blockIds[id];
    blockIds[id] = newBlockId;

    //���������� ������� ������ �����
    if (onBlockChanged)
    {
        onBlockChanged(x, y, z, newBlockId, oldBlockId);
    }

    //���������� �������� �� ��������� �����
    renderer->BlockChanged(x, y, z);

    return true;
}

/*
 * GetBlock()
 * ������ � ���������� ��������� �� ��������� �����, ������������ � ��������� �����������.
 */
CBlock* CWorld::GetBlock(int x, int y, int z) 
{
    if (!IsBlockInWorld(x, y, z)) 
    {
        return nullptr;
    } 

    int id = GetArrayId(x, y, z);
    CBlock* b = CreateBlock(blockIds[id]);
    b->SetPosition(x, y, z);
    return b;
}

/*
 * GetBlock()
 * ���������� ��������� �� ��������� �����, ������������ � ��������� ������ �������.
 */
CBlock* CWorld::GetBlock(int arrayIndex)
{
    if (arrayIndex >= 0 && arrayIndex < blocksLength)
    {
        int plane = sizeX * sizeZ;        
        int y = arrayIndex / plane;
        int n = arrayIndex - plane * y;
        int x = n / sizeZ;
        int z = n - sizeZ * x;

        CBlock* b = CreateBlock(blockIds[arrayIndex]);
        b->SetPosition(x, y, z);
        return b;
    }
    return nullptr;
}

/*
 * GetBlockId()
 * ���������� ID �����, ������������ � ��������� �����������.
 */
char CWorld::GetBlockId(int x, int y, int z) 
{
    if (!IsBlockInWorld(x, y, z)) 
    {
        return 0;
    }
    int id = GetArrayId(x, y, z);
    return blockIds[id];
}

/*
 * DestroyBlock()
 * ������� ���� �� ������� [x,y,z], ���������� �������
 * � �������� ����������� � ��������.
 */
void CWorld::DestroyBlock(int x, int y, int z)
{
    char blockId = GetBlockId(x, y, z);
    if (blockId != BLOCK_AIR_ID)
    {
        SetBlockWithoutNotify(x, y, z, BLOCK_AIR_ID);

        //���������� �������� �� ��������� �����
        renderer->BlockChanged(x, y, z);

        //���������� ������� �������� �����
        if (onBlockDestroyed)
        {
            onBlockDestroyed(x, y, z, blockId);
        }
    }
}

/*
 * IsBlockInWorld()
 * ���������, ��������� �� ��������� ���������� � �������� ����.
 */
bool CWorld::IsBlockInWorld(int x, int y, int z) 
{
    return x >= 0 && y >= 0 && z >= 0 && x < sizeX && y < sizeY && z < sizeZ;
}

/*
 * GetBlocksInBox()
 * ���������� ������ ������, ����������� � �������� ������� box.
 */
std::vector<CBlock*> CWorld::GetBlocksInBox(CCollisionBox* box) 
{
    std::vector<CBlock*> blockList;
 
    int x0 = (int)box->x0;
    int y0 = (int)box->y0;
    int z0 = (int)box->z0;
    int x1 = (int)box->x1;
    int y1 = (int)box->y1;
    int z1 = (int)box->z1;
    for (int x = x0; x <= x1; x++) 
    {
        for (int y = y0; y <= y1; y++) 
        {
            for (int z = z0; z <= z1; z++) 
            {
                char blockId = GetBlockId(x, y, z);
                if (blockId != BLOCK_AIR_ID)
                {
                    CBlock* b = CreateBlock(blockId);
                    b->SetPosition(x, y, z);
                    blockList.emplace_back(b);
                }
            }
        }
    }
    return blockList;
}

/*
 * IsBlockSolid()
 * ���������, �������� �� ���� ������. ����� ������ ���� ���������� ������.
 * � ������� ������ �� ���������� ���������� ������,
 * ��-����� ���������� ��������� ������ ID �����.
 */
bool CWorld::IsBlockSolid(int x, int y, int z) 
{
    return GetBlockId(x, y, z) != BLOCK_AIR_ID;
}

CWorldRenderer* CWorld::GetRenderer()
{
    return renderer;
}

std::wstring CWorld::GetName() 
{
    return name;
}

void CWorld::SetSavingDirectory(std::wstring dir)
{
    savingDirectory = dir;
}

/*
 * Save()
 * ���������� �������� ����.
 * ������ ��� ��������: [x,z,y] - ����� � ������� short (������������ ����� �����).
 * �� ��������� - ������ ���� char (������������ ����� ����������� �����).
 * ���� ������ �������� ID ���� ������ ����. ������ ������� ������������ ��� blocksLen = x * y * z
 */
void CWorld::Save() 
{
    std::cout << "Saving the world..." << std::endl;

    std::wstring dir = savingDirectory.empty() ? L"saves" : savingDirectory;
    std::wstring fn = dir + L"\\" + name + L".dat";

    DWORD attr = GetFileAttributes(dir.c_str());
    if ((attr == INVALID_FILE_ATTRIBUTES) || ((attr & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)) 
        CreateDirectory(dir.c_str(), NULL);

    HANDLE fh = CreateFile(fn.c_str(), GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (fh && fh != INVALID_HANDLE_VALUE) 
    {
        DWORD dw = 0;
        WriteFile(fh, &sizeX, sizeof(short), &dw, NULL);
        WriteFile(fh, &sizeZ, sizeof(short), &dw, NULL);
        WriteFile(fh, &sizeY, sizeof(short), &dw, NULL);
        WriteFile(fh, blockIds, blocksLength, &dw, NULL);
        CloseHandle(fh);
    }
}

bool CWorld::Load()
{
    std::wcout << L"Loading world: " << name << L"..." << std::endl;

    std::wstring dir = savingDirectory.empty() ? L"saves" : savingDirectory;
    std::wstring fileName = dir + L"\\" + name + L".dat";

    HANDLE fileHandle = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (fileHandle && fileHandle != INVALID_HANDLE_VALUE) 
    {
        DWORD dw = 0;
        short x = 0;
        bool res = ReadFile(fileHandle, &x, sizeof(short), &dw, NULL);
        if (!res) 
        {
            CloseHandle(fileHandle);
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        
        short z = 0;
        res = ReadFile(fileHandle, &z, sizeof(short), &dw, NULL);
        if (!res) 
        {
            CloseHandle(fileHandle);
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        
        short y = 0;
        res = ReadFile(fileHandle, &y, sizeof(short), &dw, NULL);
        if (!res) 
        {
            CloseHandle(fileHandle);
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        
        if (x < chunkSize || y < chunkSize || z < chunkSize ||
            x % chunkSize != 0 || y % chunkSize != 0 || z % chunkSize != 0) 
        {
            CloseHandle(fileHandle);
            std::cout << "Load failed! Unable to load saved world! Incorrect world size!" << std::endl;
            return false;
        }
         
        blocksLength = x * y * z;
        
        DestroyBlocks();
        blockIds = (char*)malloc(sizeof(char) * blocksLength);
        if (blockIds) 
        {
            res = ReadFile(fileHandle, blockIds, blocksLength, &dw, NULL);
            CloseHandle(fileHandle);
            if (res) 
            {
                sizeX = x;
                sizeY = y;
                sizeZ = z;
            }
        }
        else 
        {
            CloseHandle(fileHandle);
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        
        //���� �������� ������ �������, �� ��������� ��� �� �����.
        if (res)
        {
            renderer->CreateChunks();
        }
        else
        {
            std::cout << "Load failed!" << std::endl;
        }
        return res;
    }
    else //���� �� ���������� ��� ��� �� ������� �������.
    {
        std::cout << "No saved world found!" << std::endl;
        return false;
    }

    std::cout << "Load failed!" << std::endl;
    return false;
}
