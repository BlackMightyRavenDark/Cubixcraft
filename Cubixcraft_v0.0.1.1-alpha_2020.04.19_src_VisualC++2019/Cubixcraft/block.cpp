#include "block.h"
#include "collisionbox.h"
#include "world.h"

Block::Block(World* world, char id, GLuint texId, int positionX, int positionY, int positionZ) {
    this->world     = world;
    this->id        = id;
    textureId       = texId;
    this->positionX = positionX;
    this->positionY = positionY;
    this->positionZ = positionZ;
    collisionBox = new CollisionBox((float)positionX, (float)positionY, (float)positionZ,
        (float)positionX + world->blockWidth,
        (float)positionY + world->blockHeight,
        (float)positionZ + world->blockWidth);
}

Block::~Block() {
    delete collisionBox;
}

void Block::SetPosition(int xPos, int yPos, int zPos) {
    positionX = xPos;
    positionY = yPos;
    positionZ = zPos;
    collisionBox->x0 = (float)xPos;
    collisionBox->y0 = (float)yPos;
    collisionBox->z0 = (float)zPos;
    collisionBox->x1 = (float)xPos + world->blockWidth;
    collisionBox->y1 = (float)yPos + world->blockHeight;
    collisionBox->z1 = (float)zPos + world->blockWidth;
}

char Block::GetId() {
    return id;
}

void Block::SetId(char newId) {
    id = newId;
}
