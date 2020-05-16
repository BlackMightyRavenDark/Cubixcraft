#include <vector>

class CChunk;
class CTessellator;
class CWorld;

class CWorldRenderer {
public:
    //���������� ������ �� ���� X,Y,Z
    int xChunks;
    int yChunks;
    int zChunks;

    //������ ���������� �� ���������� ������
    std::vector<CChunk*> chunks;
    
    //���������� ������, �� ������� ������� ���.
    //��� ����� ���������� ������ ������� ������.
    int chunksCount;

    /*
     * ����������� �� ������������ ���������� ���������� �� ���� ���� ������.
     * ��� ����� ��� ��������� ���������� ������� �����.
     * ���� �����, � �����-�� ������, ���� ��������� ��������� ��������
     * ���������� ������ ������������, �� ��� ����� ���������� (�����������)
     * �� ��� �����, � �� maxRebuildsPerFrame �� ������ ����.
     * �������� ���������� ��-��������� ����� 8.
     * ��� ������� ��������� ����� ��������. ������, ����� �����-�� �����
     * ����� ������ ��� ������ ���� ����������� �����.
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
     * ����� ���������� ������������� ������.
     * ���� ��������� �������������, ���� � ��� ������������
     * ����-�� ���� ����, �� ���������� ������ �������.
     */
    int totalChunksRendered;

    //����� ���������� ������������� ������
    int totalBlocksRendered;

    //������������ ���������� ������, 
    //������� ����� ��������� � ����
    int totalBlocksPossible;

    //����� ���������� ������������� ������
    int totalQuadsRendered;

    /*
     * ������������ ���������� ������ ����� (������),
     * ������� ����� ���������� � ����.
     * ��� ����� ��������� ������� ����������
     * ������������� ���������� ��������� ������ �� 6.
     */
    int totalQuadsPossible;

};
