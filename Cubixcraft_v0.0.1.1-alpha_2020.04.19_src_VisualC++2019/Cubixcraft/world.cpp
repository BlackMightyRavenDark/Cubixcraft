#include "world.h"
#include "block.h"
#include "collisionbox.h"

World::World(int xSize, int ySize, int zSize, std::wstring name) {
    /*
     * ��� ����� ����������� � ���� <current_dir>/saves/<name>.dat
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

    //������ ���� � ������ �� ��� X
    sizeX = xSize;
    //������ ���� � ������ �� ��� Z
    sizeY = ySize;
    //������ ���� � ������ �� ��� Y.
    //��� �� - depth. �� ���� ��� ����� �������.
    sizeZ = zSize; 
    blocksLength = sizeX * sizeY * sizeZ;
    blockIds = nullptr;
}

World::~World() {
    Save();
    std::cout << "Destructing world..." << std::endl;
    DestroyBlocks();
}

void World::DestroyBlocks() {
    if (blockIds) {
        free(blockIds);
        blockIds = nullptr;
    }
    for (size_t i = 0; i < blocks.size(); i++) {
        delete blocks[i];
    }
    blocks.resize(0);
}

/*
 * GenerateMap()
 * ���������� ������� ���.
 */
void World::GenerateMap() {
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
    for (int y = 0; y < sizeZ; y++) {
        for (int x = 0; x < sizeX; x++) {
            for (int z = 0; z < sizeY; z++) {
                SetBlock(x, y, z, y < 16 ? 1 : 0);
            }
        }
    }
}

/*
 * GetArrayId()
 * ��������� ��������� ���������� ���� � ������ ����������� �������.
 * ���������� ID ������ �������, ��������������� ���������� �����������.
 */
int World::GetArrayId(int x, int y, int z) {
    return (x * sizeY) + (sizeX * sizeY * y) + z;
}

/*
 * SetBlock()
 * �������� ���� �� ������� [x,y,z].
 * � ������� ������ ���������� ������ ���� ���� - �����.
 * ��-�����, ������� 1 ���� ���� ���� ��� 0, ���� ����� ���.
 */
Block* World::SetBlock(int x, int y, int z, char blockId) {
    //���� [x,y,z] ��������� �� ��������� ���� - ������� ��������� � �������
    if (!IsBlockInWorld(x, y, z)) {
        std::cout << "The block [" << x << "," << y << "," << z << "] is outside of the world!" << std::endl;
        return nullptr;
    }
    int id = GetArrayId(x, y, z);
    blockIds[id] = blockId;

    //���� ������ ����� � ������� ����� - ������ ����� ����
    if (!blocks[id]) {
        blocks[id] = new Block(this, blockIds[id], blockIds[id], x, y, z);
    }
    //�����, ������ ������ �������� ����� � ���� ������
    else 
    {
        blocks[id]->SetPosition(x, y, z);
        blocks[id]->SetId(blockIds[id]);
        blocks[id]->textureId = (GLuint)blockIds[id];
    }
    return blocks[id];
}

/*
 * GetBlock()
 * ���������� ��������� �� ��������� �����, ������������ � ��������� �����������.
 */
Block* World::GetBlock(int x, int y, int z) {
    if (!IsBlockInWorld(x, y, z)) {
        return nullptr;
    } 
    int id = GetArrayId(x, y, z);
    return blocks[id];
}

/*
 * GetBlockId()
 * ���������� ID �����, ������������ � ��������� �����������.
 */
char World::GetBlockId(int x, int y, int z) {
    if (!IsBlockInWorld(x, y, z)) {
        return 0;
    }
    int id = GetArrayId(x, y, z);
    return blockIds[id];
}

/*
 * IsBlockInWorld()
 * ���������, ��������� �� ��������� ���������� � �������� ����.
 */
bool World::IsBlockInWorld(int x, int y, int z) {
    return x >= 0 && y >= 0 && z >= 0 && x < sizeX && y < sizeZ && z < sizeY;
}

/*
 * GetBlocksInBox()
 * ���������� ������ ������, ����������� � �������� ������� box.
 */
std::vector<Block*> World::GetBlocksInBox(CollisionBox* box) {
    std::vector<Block*> a;
    int x0 = (int)box->x0;
    int y0 = (int)box->y0;
    int z0 = (int)box->z0;
    int x1 = (int)box->x1;
    int y1 = (int)box->y1;
    int z1 = (int)box->z1;
    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            for (int z = z0; z <= z1; z++) {
                Block* b = GetBlock(x, y, z);
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
bool World::IsBlockSolid(int x, int y, int z) {
    return GetBlockId(x, y, z);
}

std::wstring World::GetName() {
    return name;
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
void World::Save() {
    std::cout << "Saving world..." << std::endl;
    std::wstring fn = L"saves/" + name + L".dat";

    DWORD attr = GetFileAttributes(L"saves");
    if ((attr == INVALID_FILE_ATTRIBUTES) || ((attr & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)) {
        CreateDirectory(L"saves", NULL);
    }

    HANDLE fh = CreateFile(fn.c_str(), GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (fh && fh != INVALID_HANDLE_VALUE) {
        DWORD dw = 0;
        WriteFile(fh, &sizeX, sizeof(short), &dw, NULL);
        WriteFile(fh, &sizeY, sizeof(short), &dw, NULL);
        WriteFile(fh, &sizeZ, sizeof(short), &dw, NULL);
        WriteFile(fh, blockIds, blocksLength, &dw, NULL);
        CloseHandle(fh);
    }
}

bool World::Load() {
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    std::string s = converterX.to_bytes(name);
    std::cout << "Loading world: " << s.c_str() << "..." << std::endl;
    std::wstring fn = L"saves/" + name + L".dat";

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
                            blocks[i] = new Block(this, blockIds[i], blockIds[i], xx, yy, zz);
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
