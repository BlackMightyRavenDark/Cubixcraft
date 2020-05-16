#include "world.h"
#include "block.h"
#include "blockair.h"
#include "blockdirt.h"
#include "blockgrass.h"
#include "chunk.h"
#include "collisionbox.h"
#include "main.h"
#include "worldrenderer.h"

CWorld::CWorld(short xSize, short ySize, short zSize, std::wstring name) 
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
    savingDirectory = L"";
}

CWorld::~CWorld() 
{
    Save();
    std::cout << "Destructing world..." << std::endl;
    DestroyBlocks();
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
    for (size_t i = 0; i < blocks.size(); i++) 
    {
        delete blocks[i];
    }
    blocks.resize(0);
}

/*
 * GenerateMap()
 * ���������� ������� ���.
 */
void CWorld::GenerateMap() 
{
    /*
     * �� ��������, ��� ��� Y � Z ����������.
     * � ���������� ���� ���������, ��� X � Y ���
     * ������ � ������. Z ��� ������ (��� depth) ����.
     * � � ��������� ������� � ��� ������ � ������� - ��������.
     * ��� [X,Z] ������������� ����������� �� ��������� [X,Y] ����,
     * � Y ���������� ������ (������) �����/���� ������������ ����.
     * �� ����, Y ��� ������ (��� depth).
     */
    std::cout << "Generating map " << sizeX << "x" << sizeZ << "x" << sizeY;
    DestroyBlocks();
    blocksLength = sizeX * sizeY * sizeZ;
    std::cout << ", " << blocksLength << " blocks total" << std::endl;
    blockIds = (char*)malloc(sizeof(char) * blocksLength);
    blocks.resize(blocksLength);

    //��-����� ����� sizeY � sizeZ �������� �������.
    //��������� ��������� ����, ������� �����
    for (int y = 0; y < sizeZ; y++) 
    {
        for (int x = 0; x < sizeX; x++) 
        {
            for (int z = 0; z < sizeY; z++) 
            {
                CBlock* b;
                if (y < 15)
                {
                    b = CreateBlock(BLOCK_DIRT_ID);
                }
                else
                if (y < 16)
                {
                    b = CreateBlock(BLOCK_GRASS_ID);
                }
                else
                {
                    b = CreateBlock(BLOCK_AIR_ID);
                }
                SetBlockWithoutNotify(x, y, z, b);
            }
        }
    }

    //��������� ������� �� ����� � ������ �����
    for (int x = 0; x < sizeX; x += chunkSize) {
        for (int z = 0; z < sizeY; z += chunkSize) {
            for (int yy = sizeZ - 1; yy >= 16; yy--) {
                SetBlockWithoutNotify(x, yy, z,
                    yy == sizeZ - 1 ? CreateBlock(BLOCK_GRASS_ID) : CreateBlock(BLOCK_DIRT_ID));
            }
        }
    }
}

/*
 * GetArrayId()
 * ��������� ��������� ���������� ���� � ������ ����������� �������.
 * ���������� ID ������ �������, ��������������� ���������� �����������.
 */
int CWorld::GetArrayId(int x, int y, int z) 
{
    return (x * sizeY) + (sizeX * sizeY * y) + z;
}

CBlock* CWorld::CreateBlock(char id)
{
    CBlock* block = nullptr;
    switch (id)
    {
        case BLOCK_AIR_ID:
            block = new CBlockAir(this);
            break;
        case BLOCK_GRASS_ID:
            block = new CBlockGrass(this, new POINT{ 0,0 });
            break;
        case BLOCK_DIRT_ID:
            block = new CBlockDirt(this, new POINT{ 2,0 });
            break;
        default:
            return nullptr;
    }
    return block;
}

/*
 * SetBlock()
 * �������� ���� �� ������� [x,y,z], ���������� �������
 * � �������� ����������� � ��������.
 */
bool CWorld::SetBlock(int x, int y, int z, CBlock* block) 
{
    //���� [x,y,z] ��������� �� ��������� ���� - ������� ��������� � �������
    if (!IsBlockInWorld(x, y, z)) 
    {
        std::cout << "The block [" << x << "," << y << "," << z << "] is outside of the world!" << std::endl;
        delete block;
        return false;
    }
    int id = GetArrayId(x, y, z);
    blockIds[id] = block->GetId();

    delete blocks[id];

    block->SetPosition(x, y, z);
    blocks[id] = block;

    //���� ����� ���� �� �������� ������ �������,
    //�� ���� ���� ���� ��� ������ ���������, � �� ��������,
    //�� ���������� �������.
    if (block->GetId() != BLOCK_AIR_ID)
    {
        if (onBlockPlaced)
        {
            onBlockPlaced(block);
        }
    }

    //���������� �������� �� ��������� �����
    worldRenderer->BlockChanged(x, y, z);

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
 */
bool CWorld::SetBlockWithoutNotify(int x, int y, int z, CBlock* block)
{
    //���� [x,y,z] ��������� �� ��������� ���� - ������� ��������� � �������
    if (!IsBlockInWorld(x, y, z)) 
    {
        std::cout << "The block [" << x << "," << y << "," << z << "] is outside of the world!" << std::endl;
        delete block;
        return false;
    }

    block->SetPosition(x, y, z);

    int id = GetArrayId(x, y, z);
    blockIds[id] = block->GetId();

    if (!blocks[id]) 
    {
        delete blocks[id];
    }
    blocks[id] = block;

    return true;
}

/*
 * ReplaceBlock()
 * �������� ���� �� ������� [x,y,z], ���������� �������
 * � �������� ����������� � ��������.
 * ���� ����� ����� ��� ���� ����� ����� ��� �������,
 * �������������� �������, ������ �������� ������ �
 * ����������� ����� �� ���� ��� �� ����� �����.
 */
bool CWorld::ReplaceBlock(int x, int y, int z, CBlock* block)
{
    //���� [x,y,z] ��������� �� ��������� ���� - ������� ��������� � �������
    if (!IsBlockInWorld(x, y, z))
    {
        std::cout << "The block [" << x << "," << y << "," << z << "] is outside of the world!" << std::endl;
        delete block;
        return false;
    }

    block->SetPosition(x, y, z);

    int id = GetArrayId(x, y, z);
    blockIds[id] = block->GetId();

    //�������� ��������� ����������� �����
    CBlock* b = blocks[id];

    //�������� ������ ���� �� �����
    blocks[id] = block;

    //���������� ������� ������ �����
    if (onBlockChanged)
    {
        onBlockChanged(b);
    }

    //������� ��������� ���������� ����� � ����������� ������
    delete b;

    //���������� �������� �� ��������� �����
    worldRenderer->BlockChanged(x, y, z);

    return true;
}

/*
 * GetBlock()
 * ���������� ��������� �� ��������� �����, ������������ � ��������� �����������.
 */
CBlock* CWorld::GetBlock(int x, int y, int z) 
{
    if (!IsBlockInWorld(x, y, z)) 
    {
        return nullptr;
    } 
    int id = GetArrayId(x, y, z);
    return blocks[id];
}

/*
 * GetBlock()
 * ���������� ��������� �� ��������� �����, ������������ � ��������� ������ �������.
 */
CBlock* CWorld::GetBlock(int arrayIndex)
{
    if (arrayIndex >= 0 && arrayIndex < blocksLength)
    {
        return blocks[arrayIndex];
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
    if (IsBlockInWorld(x, y, z))
    {
        //���������� ������� �������� �����
        if (onBlockDestroyed)
        {
            onBlockDestroyed(GetBlock(x, y, z));
        }

        SetBlockWithoutNotify(x, y, z, CreateBlock(BLOCK_AIR_ID));

        //���������� �������� �� ��������� �����
        worldRenderer->BlockChanged(x, y, z);
    }
}

/*
 * IsBlockInWorld()
 * ���������, ��������� �� ��������� ���������� � �������� ����.
 */
bool CWorld::IsBlockInWorld(int x, int y, int z) 
{
    return x >= 0 && y >= 0 && z >= 0 && x < sizeX && y < sizeZ && z < sizeY;
}

/*
 * GetBlocksInBox()
 * ���������� ������ ������, ����������� � �������� ������� box.
 */
std::vector<CBlock*> CWorld::GetBlocksInBox(CCollisionBox* box) {
    std::vector<CBlock*> a;
    int x0 = (int)box->x0;
    int y0 = (int)box->y0;
    int z0 = (int)box->z0;
    int x1 = (int)box->x1;
    int y1 = (int)box->y1;
    int z1 = (int)box->z1;
    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            for (int z = z0; z <= z1; z++) {
                CBlock* b = GetBlock(x, y, z);
                if (b && b->GetId()) {
                    a.resize(a.size() + 1);
                    a[a.size() - 1] = b;
                }
            }
        }
    }
    return a;
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
 * ������ ��� ��������: [x,z,y] - ����� � ������� short (������������ ����� ����������� �����).
 * x - ������ ���� (� ������) �� ��� X
 * z - ������ ���� (� ������) �� ��� Y
 * y - ������ ���� (� ������) �� ��� Z (��� �� depth - ��� ������) 
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
    {
        CreateDirectory(dir.c_str(), NULL);
    }

    HANDLE fh = CreateFile(fn.c_str(), GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (fh && fh != INVALID_HANDLE_VALUE) 
    {
        DWORD dw = 0;
        WriteFile(fh, &sizeX, sizeof(short), &dw, NULL);
        WriteFile(fh, &sizeY, sizeof(short), &dw, NULL);
        WriteFile(fh, &sizeZ, sizeof(short), &dw, NULL);
        WriteFile(fh, blockIds, blocksLength, &dw, NULL);
        CloseHandle(fh);
    }
}

bool CWorld::Load()
{
    std::wcout << L"Loading world: " << name << L"..." << std::endl;

    std::wstring dir = savingDirectory.empty() ? L"saves" : savingDirectory;
    std::wstring fn = dir + L"\\" + name + L".dat";

    HANDLE fh = CreateFile(fn.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (fh && fh != INVALID_HANDLE_VALUE) {
        DWORD dw = 0;
        short x = 0;
        bool res = ReadFile(fh, &x, sizeof(short), &dw, NULL);
        if (!res) {
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        short y = 0;
        res = ReadFile(fh, &y, sizeof(short), &dw, NULL);
        if (!res) {
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        short z = 0;
        res = ReadFile(fh, &z, sizeof(short), &dw, NULL);
        if (!res) {
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        blocksLength = x * y * z;
        DestroyBlocks();
        blockIds = (char*)malloc(sizeof(char) * blocksLength);
        if (blockIds) {
            res = ReadFile(fh, blockIds, blocksLength, &dw, NULL);
            CloseHandle(fh);
            if (res) {
                sizeX = x;
                sizeY = y;
                sizeZ = z;
                blocks.resize(blocksLength);
                int i = 0;
                for (int yy = 0; yy < sizeZ; yy++) {
                    for (int xx = 0; xx < sizeX; xx++) {
                        for (int zz = 0; zz < sizeY; zz++) {
                            CBlock* b = CreateBlock(blockIds[i]);
                            b->SetPosition(xx, yy, zz);
                            blocks[i] = b;
                            i++;
                        }
                    }
                }
            }
        }
        else {
            CloseHandle(fh);
            std::cout << "Load failed!" << std::endl;
            return false;
        }
        return res;
    }
    else {
        std::cout << "Load failed!" << std::endl;
        return false;
    }
    return true;
}
