#include "objectselector.h"

ObjectSelector::ObjectSelector(Player* player){
    this->player = player;
    hitResult = new HITRESULT();
    hitsCount = 0;
}

ObjectSelector::~ObjectSelector(){
    delete hitResult;
}

void ObjectSelector::RenderHitSide(Tesselator* t){
    float x0 = hitResult->x;
    float y0 = hitResult->y;
    float z0 = hitResult->z;
    float x1;
    float y1;
    float z1;
    switch (hitResult->blockSideId) {
    case BLOCK_SIDE_FRONT:
        x1 = x0 + player->worldObj->blockWidth;
        y1 = y0 + player->worldObj->blockHeight;
        z1 = z0 + player->worldObj->blockWidth;
        t->AddVertexQuad(x0, y0, z1);
        t->AddVertexQuad(x1, y0, z1);
        t->AddVertexQuad(x1, y1, z1);
        t->AddVertexQuad(x0, y1, z1);
        break;

    case BLOCK_SIDE_BACK:
        x1 = x0 + player->worldObj->blockWidth;
        y1 = y0 + player->worldObj->blockHeight;
        t->AddVertexQuad(x0, y0, z0);
        t->AddVertexQuad(x1, y0, z0);
        t->AddVertexQuad(x1, y1, z0);
        t->AddVertexQuad(x0, y1, z0);
        break;

    case BLOCK_SIDE_LEFT:
        y1 = y0 + player->worldObj->blockHeight;
        z1 = z0 + player->worldObj->blockWidth;
        t->AddVertexQuad(x0, y0, z0);
        t->AddVertexQuad(x0, y0, z1);
        t->AddVertexQuad(x0, y1, z1);
        t->AddVertexQuad(x0, y1, z0);
        break;

    case BLOCK_SIDE_RIGHT:
        x1 = x0 + player->worldObj->blockWidth;
        y1 = y0 + player->worldObj->blockHeight;
        z1 = z0 + player->worldObj->blockWidth;
        t->AddVertexQuad(x1, y0, z0);
        t->AddVertexQuad(x1, y0, z1);
        t->AddVertexQuad(x1, y1, z1);
        t->AddVertexQuad(x1, y1, z0);
        break;

    case BLOCK_SIDE_BOTTOM:
        x1 = x0 + player->worldObj->blockWidth;
        z1 = z0 + player->worldObj->blockWidth;
        t->AddVertexQuad(x0, y0, z0);
        t->AddVertexQuad(x1, y0, z0);
        t->AddVertexQuad(x1, y0, z1);
        t->AddVertexQuad(x0, y0, z1);
        break;

    case BLOCK_SIDE_TOP:
        x1 = x0 + player->worldObj->blockWidth;
        y1 = y0 + player->worldObj->blockHeight;
        z1 = z0 + player->worldObj->blockWidth;
        t->AddVertexQuad(x0, y1, z0);
        t->AddVertexQuad(x1, y1, z0);
        t->AddVertexQuad(x1, y1, z1);
        t->AddVertexQuad(x0, y1, z1);
        break;
    }
}

void ObjectSelector::RenderGhostCubeSide(Tesselator* t, int x, int y, int z, int side){
    float x0 = x;
    float y0 = y;
    float z0 = z;
    float x1;
    float y1;
    float z1;
    switch (side){
    case BLOCK_SIDE_FRONT:
        if (!player->worldObj->IsBlockSolid(x, y, z + 1)) {
            x1 = x0 + player->worldObj->blockWidth;
            y1 = y0 + player->worldObj->blockHeight;
            z1 = z0 + player->worldObj->blockWidth;
            t->AddVertexQuad(x0, y0, z1);
            t->AddVertexQuad(x1, y0, z1);
            t->AddVertexQuad(x1, y1, z1);
            t->AddVertexQuad(x0, y1, z1);
        }
        break;

    case BLOCK_SIDE_BACK:
        if (!player->worldObj->IsBlockSolid(x, y, z - 1)) {
            x1 = x0 + player->worldObj->blockWidth;
            y1 = y0 + player->worldObj->blockHeight;
            t->AddVertexQuad(x0, y0, z0);
            t->AddVertexQuad(x1, y0, z0);
            t->AddVertexQuad(x1, y1, z0);
            t->AddVertexQuad(x0, y1, z0);
        }
        break;

    case BLOCK_SIDE_LEFT:
        if (!player->worldObj->IsBlockSolid(x - 1, y, z)) {
            y1 = y0 + player->worldObj->blockHeight;
            z1 = z0 + player->worldObj->blockHeight;
            t->AddVertexQuad(x0, y0, z0);
            t->AddVertexQuad(x0, y0, z1);
            t->AddVertexQuad(x0, y1, z1);
            t->AddVertexQuad(x0, y1, z0);
        }
        break;

    case BLOCK_SIDE_RIGHT:
        if (!player->worldObj->IsBlockSolid(x + 1, y, z)) {
            x1 = x0 + player->worldObj->blockWidth;
            y1 = y0 + player->worldObj->blockHeight;
            z1 = z0 + player->worldObj->blockWidth;
            t->AddVertexQuad(x1, y0, z0);
            t->AddVertexQuad(x1, y0, z1);
            t->AddVertexQuad(x1, y1, z1);
            t->AddVertexQuad(x1, y1, z0);
        }
        break;

    case BLOCK_SIDE_TOP:
        if (!player->worldObj->IsBlockSolid(x, y + 1, z)) {
            x1 = x0 + player->worldObj->blockWidth;
            y1 = y0 + player->worldObj->blockHeight;
            z1 = z0 + player->worldObj->blockWidth;
            t->AddVertexQuad(x0, y1, z0);
            t->AddVertexQuad(x0, y1, z1);
            t->AddVertexQuad(x1, y1, z1);
            t->AddVertexQuad(x1, y1, z0);
        }
        break;

    case BLOCK_SIDE_BOTTOM:
        if (!player->worldObj->IsBlockSolid(x, y - 1, z)) {
            x1 = x0 + player->worldObj->blockWidth;
            z1 = z0 + player->worldObj->blockWidth;
            t->AddVertexQuad(x0, y0, z0);
            t->AddVertexQuad(x0, y0, z1);
            t->AddVertexQuad(x1, y0, z1);
            t->AddVertexQuad(x1, y0, z0);
        }
        break;
    }
}

void ObjectSelector::RenderGhostCubeSides(float r){
    CollisionBox* box = player->collisionBox->Grow(r, r, r);
    int x0 = (int)box->x0;
    int x1 = (int)box->x1;
    int y0 = (int)box->y0;
    int y1 = (int)box->y1;
    int z0 = (int)box->z0;
    int z1 = (int)box->z1;
    delete box;
    glInitNames();
    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            for (int z = z0; z <= z1; z++){
                if (player->worldObj->GetBlockId(x, y, z) &&
                    frustumObj->CubeInFrustum(x, y, z, player->worldObj->cubeSize)) {
                    int id = player->worldObj->GetArrayId(x, y, z);
                    glPushName(id);
                    Tesselator* t = new Tesselator();
                    for (int i = 0; i < 6; i++) {
                        glPushName(i);
                        t->ClearQuads();
                        RenderGhostCubeSide(t, x, y, z, i);
                        t->FlushQuads();
                        glPopName();
                    }
                    delete t;
                    glPopName();
                }
            }
        }
    }
}

int ObjectSelector::PickObject(float radius){
    ZeroMemory(&selectBuffer, SELECT_BUFFER_SIZE);
    glSelectBuffer(SELECT_BUFFER_SIZE, selectBuffer);
    glGetIntegerv(GL_VIEWPORT, viewportBuffer);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glRenderMode(GL_SELECT);
    gluPickMatrix((double)glScreenWidth / 2.0, (double)glScreenHeight / 2.0, 2.0, 2.0, viewportBuffer);
    gluPerspective(75.0, (double)glScreenWidth / (double)glScreenHeight, 0.05, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(player->rotationRoll,  0.0, 0.0, 1.0);
    glRotatef(player->rotationPitch, 1.0, 0.0, 0.0);
    glRotatef(player->rotationYaw,   0.0, 1.0, 0.0);
    glTranslatef(-player->positionX - player->GetCameraOffsetZ(),
                 -player->positionY - player->height,
                 -player->positionZ - player->GetCameraOffsetZ());

    RenderGhostCubeSides(radius);

    hitsCount = glRenderMode(GL_RENDER);

    if (hitsCount){
        int cubeId = -1;
        int sideId = -1;
        GLuint closest = selectBuffer[1];
        cubeId = selectBuffer[3];
        sideId = selectBuffer[4];
        for (int i = 0; i < hitsCount; i++){
            int n = i * 5;
            GLuint dist = selectBuffer[n + 1];
            if (dist < closest){
                closest = dist;
                cubeId = selectBuffer[n + 3];
                sideId = selectBuffer[n + 4];
            }
        }
        hitResult->blockId = cubeId;
        hitResult->blockSideId = sideId;
        hitResult->x = player->worldObj->blocks[cubeId]->positionX;
        hitResult->y = player->worldObj->blocks[cubeId]->positionY;
        hitResult->z = player->worldObj->blocks[cubeId]->positionZ;
        switch (hitResult->blockSideId){
        case BLOCK_SIDE_FRONT:
            hitResult->xNear = hitResult->x;
            hitResult->yNear = hitResult->y;
            hitResult->zNear = hitResult->z + 1;
            break;
        case BLOCK_SIDE_BACK:
            hitResult->xNear = hitResult->x;
            hitResult->yNear = hitResult->y;
            hitResult->zNear = hitResult->z - 1;
            break;
        case BLOCK_SIDE_LEFT:
            hitResult->xNear = hitResult->x - 1;
            hitResult->yNear = hitResult->y;
            hitResult->zNear = hitResult->z;
            break;
        case BLOCK_SIDE_RIGHT:
            hitResult->xNear = hitResult->x + 1;
            hitResult->yNear = hitResult->y;
            hitResult->zNear = hitResult->z;
            break;
        case BLOCK_SIDE_TOP:
            hitResult->xNear = hitResult->x;
            hitResult->yNear = hitResult->y + 1;
            hitResult->zNear = hitResult->z;
            break;
        case BLOCK_SIDE_BOTTOM:
            hitResult->xNear = hitResult->x;
            hitResult->yNear = hitResult->y - 1;
            hitResult->zNear = hitResult->z;
            break;
        }
    } else {
        hitResult->blockId = -1;
        hitResult->blockSideId = -1;
    }

    return hitsCount;
}

int ObjectSelector::GetHitsCount(){
    return hitsCount;
}

std::string ObjectSelector::SideIdToString(int side){
    switch (side){
    case BLOCK_SIDE_FRONT:
        return "Front";
    case BLOCK_SIDE_BACK:
        return "Back";
    case BLOCK_SIDE_LEFT:
        return "Left";
    case BLOCK_SIDE_RIGHT:
        return "Right";
    case BLOCK_SIDE_TOP:
        return "Top";
    case BLOCK_SIDE_BOTTOM:
        return "Bottom";
    default:
        return "Unknown";
    }
}

std::string IntToStr(int x){
    char c[20];
    itoa(x, c, 10);
    return c;
}

std::string ObjectSelector::HitResultToString(){
    return "[" + IntToStr(hitResult->x) + "," +
                 IntToStr(hitResult->y) + "," +
                 IntToStr(hitResult->z) + "] " +
                 SideIdToString(hitResult->blockSideId);
}

std::string ObjectSelector::HitResultNearToString(){
    return IntToStr(hitResult->xNear) + "," +
           IntToStr(hitResult->yNear) + "," +
           IntToStr(hitResult->zNear);
}
