#include <vector>

class CChunk;
class CTessellator;
class CWorld;

class CWorldRenderer {
public:
    //количество чанков по осям X,Y,Z
    int xChunks;
    int yChunks;
    int zChunks;

    //массив указателей на экземпляры чанков
    std::vector<CChunk*> chunks;
    
    //Количество чанков, из которых состоит мир.
    //Это число определяет размер массива чанков.
    int chunksCount;

    /*
     * Ограничение на максимальное количество рисующихся за один кадр чанков.
     * Это нужно для ускорения рендеринга каждого кадра.
     * Если игрой, в какой-то момент, была запрошена отрисовка большого
     * количества чанков одновременно, то они будут отрисованы (отрендерены)
     * не все сразу, а по maxRebuildsPerFrame за каждый кадр.
     * Значение переменной по-умолчанию равно 8.
     * Это немного сокращает время ожидания. Однако, игрок какое-то время
     * будет видеть как вокруг него загружаются чанки.
     */
    int maxRebuildsPerFrame;

    CWorld* worldObj;

    CWorldRenderer(CWorld* world);
    ~CWorldRenderer();

    int Render();
    void SetAllChunksExpired();
    void BlockChanged(int x, int y, int z);
    int GetArrayId(int x, int y, int z);
    int GetTotalChunksRendered();
    int GetTotalBlocksRendered();
    int GetTotalQuadsRendered();
    int GetTotalBlocksPossible();
    int GetTotalQuadsPossible();
private:
    /*
     * Общее количество отрендеренных чанков.
     * Чанк считается отрендеренным, если в нем присутствует
     * хотя-бы один блок, не являющийся блоком воздуха.
     */
    int totalChunksRendered;

    //Общее количество отрендеренных блоков
    int totalBlocksRendered;

    //максимальное количество блоков, 
    //которые можно построить в мире
    int totalBlocksPossible;

    //Общее количество отрендеренных граней
    int totalQuadsRendered;

    /*
     * Максимальное количество граней кубов (блоков),
     * которое может находиться в мире.
     * Это число находится простым умножением
     * максимального количества возможных блоков на 6.
     */
    int totalQuadsPossible;

};
