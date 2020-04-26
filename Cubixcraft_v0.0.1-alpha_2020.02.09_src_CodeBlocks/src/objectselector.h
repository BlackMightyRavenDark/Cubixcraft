#pragma once
#include "main.h"
#include "player.h"
#include "tesselator.h"
#include <GL/gl.h>
#include <GL/glu.h>

struct HITRESULT {
    int x;
    int y;
    int z;
    int xNear;
    int yNear;
    int zNear;
    int blockId = -1;
    int blockSideId = -1;
};

class ObjectSelector {
public:
    ObjectSelector(Player* player);
    ~ObjectSelector();
    int PickObject(float radius);
    void RenderHitSide(Tesselator* t);
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
