#include "block.h"
#include "collisionbox.h"
#include "tessellator.h"
#include "world.h"

CBlock::CBlock(CWorld* world, char id, POINT* textureCoord, std::wstring name)
{
    this->world = world;
    this->id    = id;
    textureCoordinates = textureCoord;
    this->name = name;
    positionX = 0;
    positionY = 0;
    positionZ = 0;
    collisionBox = new CCollisionBox(0.0f, 0.0f, 0.0f,
        world->blockWidth,
        world->blockHeight,
        world->blockWidth);
}

CBlock::~CBlock() 
{
    delete collisionBox;
}

void CBlock::SetPosition(int xPos, int yPos, int zPos) 
{
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

char CBlock::GetId() 
{
    return id;
}

int CBlock::GetPositionX()
{
    return positionX;
}

int CBlock::GetPositionY()
{
    return positionY;
}

int CBlock::GetPositionZ()
{
    return positionZ;
}

std::wstring CBlock::GetName()
{
    return name;
}

/*
 * GetTextureCoordinates()
 * Возвращает координаты текстуры блока в текстурном атласе.
 * Этот метод, перекрытый в классах-наследниках,
 * может вернуть разные координаты для разных сторон блока.
 * Это нужно для того, чтобы можно было сделать блок
 * с разными текстурами на разных его сторонах.
 */
POINT CBlock::GetTextureCoordinates(int side)
{
    return POINT{ textureCoordinates->x, textureCoordinates->y };
}

int CBlock::Render(CTessellator* t)
{
    int sidesRendered = 0;
    for (int i = 0; i < 6; i++) 
    {
        sidesRendered += RenderSide(t, (float)positionX, (float)positionY, (float)positionZ, i);
    }
    return sidesRendered;
}

int CBlock::RenderSide(CTessellator* t, float x, float y, float z, int side) 
{
    int sidesRendered = 0;
    POINT pt = GetTextureCoordinates(side);

    /*
     * u0, v0 - координаты x,y левой верхней точки текстуры текущей стороны блока в текстурном атласе.
     * u1, v1 - координаты x,y правой нижней точки текстуры текущей стороны блока в текстурном атласе.
     * Из текстурного атласа будет выделена область между этими точками
     * и наложена на текущую сторону блока в качестве ее текстуры.
     * 16 - это количество текстур в атласе по вертикали и горизонтали.
     * Текстурный атлас используется для сокращения количества переключений
     * между текстурами и, как следствие, ускорения рендеринга и повышения FPS.
     */
    float u0 = pt.x / 16.0f;
    float v0 = pt.y / 16.0f;
    float u1 = u0 + 1.0f / 16.0f;
    float v1 = v0 + 1.0f / 16.0f;

    switch (side) 
    {
        case BLOCK_SIDE_BACK:
            if (!world->IsBlockSolid((int)x, (int)y, (int)z - 1)) 
            {
                t->AddTextureVertexQuad(u0, v1, x, y, z);
                t->AddTextureVertexQuad(u1, v1, x + world->blockWidth, y, z);
                t->AddTextureVertexQuad(u1, v0, x + world->blockWidth, y + world->blockHeight, z);
                t->AddTextureVertexQuad(u0, v0, x, y + world->blockHeight, z);
                sidesRendered++;
            }
            break;
        case BLOCK_SIDE_FRONT:
            if (!world->IsBlockSolid((int)x, (int)y, (int)z + 1))
            {
                t->AddTextureVertexQuad(u0, v1, x, y, z + world->blockWidth);
                t->AddTextureVertexQuad(u1, v1, x + world->blockWidth, y, z + world->blockWidth);
                t->AddTextureVertexQuad(u1, v0, x + world->blockWidth, y + world->blockHeight, z + world->blockWidth);
                t->AddTextureVertexQuad(u0, v0, x, y + world->blockHeight, z + world->blockWidth);
                sidesRendered++;
            }
            break;
        case BLOCK_SIDE_LEFT:
            if (!world->IsBlockSolid((int)x - 1, (int)y, (int)z)) 
            {
                t->AddTextureVertexQuad(u0, v1, x, y, z);
                t->AddTextureVertexQuad(u1, v1, x, y, z + world->blockWidth);
                t->AddTextureVertexQuad(u1, v0, x, y + world->blockHeight, z + world->blockWidth);
                t->AddTextureVertexQuad(u0, v0, x, y + world->blockHeight, z);
                sidesRendered++;
            }
            break;
        case BLOCK_SIDE_RIGHT:
            if (!world->IsBlockSolid((int)x + 1, (int)y, (int)z)) 
            {
                t->AddTextureVertexQuad(u0, v1, x + world->blockWidth, y, z);
                t->AddTextureVertexQuad(u1, v1, x + world->blockWidth, y, z + world->blockWidth);
                t->AddTextureVertexQuad(u1, v0, x + world->blockWidth, y + world->blockHeight, z + world->blockWidth);
                t->AddTextureVertexQuad(u0, v0, x + world->blockWidth, y + world->blockHeight, z);
                sidesRendered++;
            }
            break;
        case BLOCK_SIDE_BOTTOM:
            if (!world->IsBlockSolid((int)x, (int)y - 1, (int)z)) 
            {
                t->AddTextureVertexQuad(u0, v1, x, y, z);
                t->AddTextureVertexQuad(u1, v1, x + world->blockWidth, y, z);
                t->AddTextureVertexQuad(u1, v0, x + world->blockWidth, y, z + world->blockWidth);
                t->AddTextureVertexQuad(u0, v0, x, y, z + world->blockWidth);
                sidesRendered++;
            }
            break;
        case BLOCK_SIDE_TOP:
            if (!world->IsBlockSolid((int)x, (int)y + 1, (int)z)) 
            {
                t->AddTextureVertexQuad(u0, v0, x, y + world->blockHeight, z);
                t->AddTextureVertexQuad(u1, v0, x + world->blockWidth, y + world->blockHeight, z);
                t->AddTextureVertexQuad(u1, v1, x + world->blockWidth, y + world->blockHeight, z + world->blockWidth);
                t->AddTextureVertexQuad(u0, v1, x, y + world->blockHeight, z + world->blockWidth);
                sidesRendered++;
            }
            break;
    }
    return sidesRendered++;
}

bool CBlock::IsSideVisible(int side) 
{
    switch (side) 
    {
        case BLOCK_SIDE_TOP:
            return !world->IsBlockSolid(positionX, positionY + 1, positionZ);
        case BLOCK_SIDE_BOTTOM:
            return !world->IsBlockSolid(positionX, positionY - 1, positionZ);
        case BLOCK_SIDE_FRONT:
            return !world->IsBlockSolid(positionX, positionY, positionZ + 1);
        case BLOCK_SIDE_BACK:
            return !world->IsBlockSolid(positionX, positionY, positionZ - 1);
        case BLOCK_SIDE_LEFT:
            return !world->IsBlockSolid(positionX - 1, positionY, positionZ);
        case BLOCK_SIDE_RIGHT:
            return !world->IsBlockSolid(positionX + 1, positionY, positionZ);
        default:
            return true;
    }
}
