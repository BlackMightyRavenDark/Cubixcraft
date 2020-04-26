/*
 *  ласс ObjectSelector отвечает за определение игровых объектов, на которые смотрит игрок.
 */
#include <Windows.h>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>

class Player;
class Tesselator;

/*
 * —труктура HITRESULT содержит информацию о том, на какой блок смотрит игрок.
 * ≈сли значени€ полей blockId или blockSideId меньше нул€, значит, что игрок
 * смотрит в пустоту или находитс€ слишком далеко от ближайшего блока.
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

class ObjectSelector {
public:
    ObjectSelector(Player* player);
    ~ObjectSelector();
    int PickObject(float radius);
    void RenderHitCubeSide(Tesselator* t);
    std::string SideIdToString(int side);
    std::string HitResultToString();
    std::string HitResultNearToString();
    int GetHitsCount();

    HITRESULT* hitResult;

private:
    int viewportBuffer[16];
    static const int SELECT_BUFFER_SIZE = 400;
    GLuint selectBuffer[SELECT_BUFFER_SIZE];
    int hitsCount;

    Player* player;

    void RenderGhostCubeSide(Tesselator* t, int x, int y, int z, int side);
    void RenderGhostCubeSides(float r);
};
