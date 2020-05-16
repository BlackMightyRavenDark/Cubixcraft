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
     * Мир будет сохраняться в файл <savingDirectory>/<name>.dat
     * По-этому не стоит подсовывать в эти переменные системные символы!
     * А так же, название мира выводится на экран.
     */
    this->name = name;

    /*
     * Эти переменные определяют размер блоков. Заготовка для следующих версий.
     * В текущей версии должны быть равны единице!
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
 * Удаляет все блоки в мире и освобождает память.
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
 * Генерирует игровой мир.
 */
void CWorld::GenerateMap() 
{
    /*
     * Не забываем, что оси Y и Z перепутаны.
     * В генераторе мира считается, что X и Y это
     * ширина и длинна. Z это высота (или depth) мира.
     * А в остальных методах и при работе с камерой - наоборот.
     * Оси [X,Z] соответствуют перемещению по плоскости [X,Y] мира,
     * а Y перемещает игрока (камеру) вверх/вниз относительно мира.
     * То есть, Y это высота (или depth).
     */
    std::cout << "Generating map " << sizeX << "x" << sizeZ << "x" << sizeY;
    DestroyBlocks();
    blocksLength = sizeX * sizeY * sizeZ;
    std::cout << ", " << blocksLength << " blocks total" << std::endl;
    blockIds = (char*)malloc(sizeof(char) * blocksLength);
    blocks.resize(blocksLength);

    //по-этому здесь sizeY и sizeZ поменяны местами.
    //послойная генерация мира, начиная снизу
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

    //генерация столбов из земли в каждом чанке
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
 * Переводит трёхмерные координаты мира в формат одномерного массива.
 * Возвращает ID ячейки массива, соответствующей переданным координатам.
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
 * Заменяет блок на позиции [x,y,z], генерирует событие
 * и посылает уведомление в рендерер.
 */
bool CWorld::SetBlock(int x, int y, int z, CBlock* block) 
{
    //если [x,y,z] находятся за пределами мира - выводим сообщение и выходим
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

    //если новый блок не является блоком воздуха,
    //то есть если блок был именно поставлен, а не разрушен,
    //то генерируем событие.
    if (block->GetId() != BLOCK_AIR_ID)
    {
        if (onBlockPlaced)
        {
            onBlockPlaced(block);
        }
    }

    //уведомляем рендерер об изменении блока
    worldRenderer->BlockChanged(x, y, z);

    return true;
}

/*
 * SetBlockWithoutNotify()
 * Заменяет блок на позиции [x,y,z] без генерации события 
 * и без отправки уведомления в рендерер.
 * Уведомление нужно отправлять вручную после вызова данного метода,
 * иначе визуально изменения блока не будет видно.
 * Этот метод используется, когда блоки в мире меняются
 * без вмешательства игрока (когда игрок их не ломает и не строит).
 */
bool CWorld::SetBlockWithoutNotify(int x, int y, int z, CBlock* block)
{
    //если [x,y,z] находятся за пределами мира - выводим сообщение и выходим
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
 * Заменяет блок на позиции [x,y,z], генерирует событие
 * и посылает уведомление в рендерер.
 * Этот метод нужен для того чтобы метод или функция,
 * обрабатывающая событие, смогла получить доступ к
 * заменяемому блоку до того как он будет удалён.
 */
bool CWorld::ReplaceBlock(int x, int y, int z, CBlock* block)
{
    //если [x,y,z] находятся за пределами мира - выводим сообщение и выходим
    if (!IsBlockInWorld(x, y, z))
    {
        std::cout << "The block [" << x << "," << y << "," << z << "] is outside of the world!" << std::endl;
        delete block;
        return false;
    }

    block->SetPosition(x, y, z);

    int id = GetArrayId(x, y, z);
    blockIds[id] = block->GetId();

    //получаем экземпляр заменяемого блока
    CBlock* b = blocks[id];

    //заменяем старый блок на новый
    blocks[id] = block;

    //генерируем событие замены блока
    if (onBlockChanged)
    {
        onBlockChanged(b);
    }

    //удаляем экземпляр заменённого блока и освобождаем память
    delete b;

    //уведомляем рендерер об изменении блока
    worldRenderer->BlockChanged(x, y, z);

    return true;
}

/*
 * GetBlock()
 * Возвращает указатель на экземпляр блока, находящегося в указанных координатах.
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
 * Возвращает указатель на экземпляр блока, находящегося в указанной ячейке массива.
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
 * Возвращает ID блока, находящегося в указанных координатах.
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
 * Удаляет блок на позиции [x,y,z], генерирует событие
 * и посылает уведомление в рендерер.
 */
void CWorld::DestroyBlock(int x, int y, int z)
{
    if (IsBlockInWorld(x, y, z))
    {
        //генерируем событие удаления блока
        if (onBlockDestroyed)
        {
            onBlockDestroyed(GetBlock(x, y, z));
        }

        SetBlockWithoutNotify(x, y, z, CreateBlock(BLOCK_AIR_ID));

        //уведомляем рендерер об изменении блока
        worldRenderer->BlockChanged(x, y, z);
    }
}

/*
 * IsBlockInWorld()
 * Проверяет, находятся ли указанные координаты в пределах мира.
 */
bool CWorld::IsBlockInWorld(int x, int y, int z) 
{
    return x >= 0 && y >= 0 && z >= 0 && x < sizeX && y < sizeZ && z < sizeY;
}

/*
 * GetBlocksInBox()
 * Возвращает массив блоков, находящихся в пределах области box.
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
 * Проверяет, является ли блок твёрдым. Через твёрдый блок невозможно пройти.
 * В текущей версии не существует проходимых блоков,
 * по-этому достаточно проверять только ID блока.
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
 * Сохранение игрового мира.
 * Первые три значения: [x,z,y] - числа в формате short (двухбайтовое целое беззнаковое число).
 * x - размер мира (в блоках) по оси X
 * z - размер мира (в блоках) по оси Y
 * y - размер мира (в блоках) по оси Z (она же depth - ось прыжка) 
 * Всё остальное - массив типа char (однобайтовое целое беззнаковое число).
 * Этот массив содержит ID всех блоков мира. Размер массива определяется как blocksLen = x * y * z
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
