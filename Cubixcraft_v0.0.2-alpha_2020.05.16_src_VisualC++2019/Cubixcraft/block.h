#pragma once
#include <Windows.h>
#include <string>
#include <GL/gl.h>

const int BLOCK_SIDE_FRONT  = 0;
const int BLOCK_SIDE_BACK   = 1;
const int BLOCK_SIDE_LEFT   = 2;
const int BLOCK_SIDE_RIGHT  = 3;
const int BLOCK_SIDE_TOP    = 4;
const int BLOCK_SIDE_BOTTOM = 5;

const char BLOCK_AIR_ID   = 0;
const char BLOCK_GRASS_ID = 1;
const char BLOCK_DIRT_ID  = 2;

class CCollisionBox;
class CTessellator;
class CWorld;

class CBlock {
public:

    //���������� �������� ����� � ���������� ������
    POINT* textureCoordinates;

    /*
     * ������� ������������ ����� ����� ���� ������ ��� ������ ��� ��������.
     * ��� ����� ������������ ��� �������� ����� ������������� �����.
     */
    CCollisionBox* collisionBox;

    //�������� �����
    std::wstring name;

    CBlock(CWorld* world, char id, POINT* textureCoord, std::wstring name);
    virtual ~CBlock();

    void SetPosition(int xPos, int yPos, int zPos);
    char GetId();
    int GetPositionX();
    int GetPositionY();
    int GetPositionZ();
    std::wstring GetName();
    virtual POINT GetTextureCoordinates(int side);
    virtual int Render(CTessellator* t);
    int RenderSide(CTessellator* t, float x, float y, float z, int side);
protected:
    int positionX;
    int positionY;
    int positionZ;

private:
    //ID �����
    char id;

    /*
     * ��������� �� ��������� ������ ����.
     * ����� ��� ����, ����� ���� ���� ������ � ����������� ���������� ����,
     * � ������� �� ���������.
     */
    CWorld* world;

    bool IsSideVisible(int side);
};
