/*
 * ����� CCollisionBox ������������ ��� ����������� ������������ ���������� � ��������������������� ��������.
 * ��-��������� ���������, ��� ������ ������� ������� ��������� � ������������� ����������� ��� Z, � �������� -
 * � �������������. �� ����, ��-���������, ������ ������� � ������������� ����������� ��� Z. ����� �������,
 * [x0,y0,z0] ��� ������ ������ ������ ����� �������, � [x1,y1,z1] - ����� ������� �������� �����.
 */

class CCollisionBox {
public:
    float x0;
    float y0;
    float z0;
    float x1;
    float y1;
    float z1;

    CCollisionBox(float x0, float y0, float z0, float x1, float y1, float z1);

    CCollisionBox* Grow(float x, float y, float z);
    bool Intersects(CCollisionBox* box);
    void MoveRelative(float xOffset, float yOffset, float zOffset);
    void MoveAbsolute(float x, float y, float z);
    float ClipCollideX(CCollisionBox* box, float xMotion);
    float ClipCollideY(CCollisionBox* box, float yMotion);
    float ClipCollideZ(CCollisionBox* box, float zMotion);
};
