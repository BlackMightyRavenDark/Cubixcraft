#pragma once
#include "collisionbox.h"
#include "world.h"
#include <GL/gl.h>

const int BLOCK_SIDE_FRONT = 0;
const int BLOCK_SIDE_BACK = 1;
const int BLOCK_SIDE_LEFT = 2;
const int BLOCK_SIDE_RIGHT = 3;
const int BLOCK_SIDE_TOP = 4;
const int BLOCK_SIDE_BOTTOM = 5;

class World;

class Block {
public:
    int positionX;
    int positionY;
    int positionZ;

    GLuint textureId;

    CollisionBox* collisionBox;

    World* world;

    Block(World* world, char id, GLuint texId, int positionX, int positionY, int positionZ);
    ~Block();

    void SetPosition(int xPos, int yPos, int zPos);
    char GetId();
    void SetId(char newId);

private:
    char id;
};
