#include "block.h"

Block::Block(World* world, char id, GLuint texId, int positionX, int positionY, int positionZ){
    this->world = world;
    this->id = id;
    textureId = texId;
    this->positionX = positionX;
    this->positionY = positionY;
    this->positionZ = positionZ;
    collisionBox = new CollisionBox(positionX, positionY, positionZ,
                                    positionX + world->blockWidth, positionY + world->blockHeight, positionZ + world->blockWidth);
}

Block::~Block(){
    delete collisionBox;
}

void Block::SetPosition(int xPos, int yPos, int zPos){
    positionX = xPos;
    positionY = yPos;
    positionZ = zPos;
    collisionBox->x0 = xPos;
    collisionBox->y0 = yPos;
    collisionBox->z0 = zPos;
    collisionBox->x1 = xPos + world->blockWidth;
    collisionBox->y1 = yPos + world->blockHeight;
    collisionBox->z1 = zPos + world->blockWidth;
}

char Block::GetId(){
    return id;
}

void Block::SetId(char newId){
    id = newId;
}
