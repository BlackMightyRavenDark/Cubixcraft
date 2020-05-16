/*
 * ����� CObjectSelector �������� �� ����������� ������� ��������, �� ������� ������� �����.
 */
#include <Windows.h>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>

class CPlayer;
class CTessellator;

/*
 * ��������� HITRESULT �������� ���������� � ���, �� ����� ���� ������� �����.
 * ���� �������� ����� blockId ��� blockSideId ������ ����, ������, ��� �����
 * ������� � ������� ��� ��������� ������� ������ �� ���������� �����.
 */
struct HITRESULT {
    int x = -1;
    int y = -1;
    int z = -1;
    int xNear = -1;
    int yNear = -1;
    int zNear = -1;
    int blockId = -1;
    int blockSideId = -1;
};

class CObjectSelector {
public:
    CObjectSelector(CPlayer* player);
    ~CObjectSelector();
    int PickObject(float radius);
    void RenderHitCubeSide(CTessellator* t);
    int GetHitsCount();
    std::string SideIdToString(int side);
    std::string HitResultToString();
    std::string HitResultNearToString();

    HITRESULT* hitResult;

private:
    int viewportBuffer[16];
    static const int SELECT_BUFFER_SIZE = 400;
    GLuint selectBuffer[SELECT_BUFFER_SIZE];
    int hitsCount;

    CPlayer* player;

    void RenderGhostCubeSide(CTessellator* t, int x, int y, int z, int side);
    void RenderGhostCubeSides(float r);
};
