#include "objectselector.h"
#include "block.h"
#include "collisionbox.h"
#include "frustum.h"
#include "main.h"
#include "player.h"
#include "tesselator.h"
#include "world.h"

ObjectSelector::ObjectSelector(Player* player) {
    this->player = player;
    hitResult = new HITRESULT();
    hitsCount = 0;
    ZeroMemory(&selectBuffer, SELECT_BUFFER_SIZE);
    ZeroMemory(&viewportBuffer, 16);
}

ObjectSelector::~ObjectSelector() {
    delete hitResult;
}

/*
 * RenderHitCubeSide()
 * ������������ ����� ����, �� ������� ������� �����.
 */
void ObjectSelector::RenderHitCubeSide(Tesselator* t) {
    float x0 = (float)hitResult->x;
    float y0 = (float)hitResult->y;
    float z0 = (float)hitResult->z;
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

/*
 * RenderGhostCubeSide()
 * �������� ��������� ������� (�����) �����, ���� ��� �� ������������� ������ ������.
 * ���������� � RenderGhostCubeSides().
 */
void ObjectSelector::RenderGhostCubeSide(Tesselator* t, int x, int y, int z, int side) {
    float x0 = (float)x;
    float y0 = (float)y;
    float z0 = (float)z;
    float x1;
    float y1;
    float z1;
    switch (side) {
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

/*
 * RenderGhostCubeSides()
 * ������ ��������� ����� ����� ��� ����������� �������������� ������ � ����.
 * ���� ����� ���������� �������� ����� ����������� �����, �������������� ��������������
 * � ����� GL_SELECT. �������� �������� �� �����. �� ���������� � PickObject().
 */
void ObjectSelector::RenderGhostCubeSides(float r) {
    /*
     * ������ ��������� ������� ������������.
     * ��� ����� ��������� ������� ������ �� ��� �������.
     * ��� ����� ��� ����������� ������� ������ ������ �������� R,
     * � �������� ������� ����� ������������� ����� ��������� � ������ �����.
     */
    CollisionBox* box = player->collisionBox->Grow(r, r, r);
    int x0 = (int)box->x0;
    int x1 = (int)box->x1;
    int y0 = (int)box->y0;
    int y1 = (int)box->y1;
    int z0 = (int)box->z0;
    int z1 = (int)box->z1;
    delete box;
    glInitNames();

    //���������� ��� ����� � �������� ������� box
    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            for (int z = z0; z <= z1; z++) {
                /*
                 * ���� � ����������� ���� [x,y,z] ���� ���� � �� �������� �� frustum,
                 * ��������� ��� ID � ����� ������ � ������ ��������� �����.
                 * � ��� �� ��������� � ����� ID ������ ����� ���� (�����).
                 * ���� ����� ������������� ������ ������,
                 * ��� �� ����� ���������� � ��������� � �����.
                 */
                if (player->worldObj->GetBlockId(x, y, z) &&
                    frustumObj->CubeInFrustum((float)x, (float)y, (float)z, player->worldObj->cubeSize)) {
                    /*
                     * �������� ����� (ID) ������ � ������ � �������.
                     * ��� ����� ��� ��������� ��������� ���������� � �����,
                     * ���� ����� ��� �������.
                     */
                    int id = player->worldObj->GetArrayId(x, y, z);

                    /*
                     * glPushName() ��������� ���� ID � ����� ������.
                     * �����, ������� ���� ����� ������.
                     */
                    glPushName(id);
                    Tesselator* t = new Tesselator();
                    for (int i = 0; i < 6; i++) {
                        /*
                         * ��������� � ����� ������ ID ������ ����� ���� � ������ ��.
                         * �������� ����� ����� �������� glPushName() � glPopName(),
                         * ����� � ����� ������ ������ �� �����������.
                         */
                        glPushName(i);
                        t->ClearQuads();
                        /*
                         * ����� RenderGhostCubeSide() ���������, ������������� ��
                         * ������� ����� ����. ���� ��� - ��� ����� ���������� �
                         * ��������� � ����� ������. ���� ��, �� ���������� �����
                         * glPushName() ������������, � � ����� ������ ������ �� �����������.
                         */
                        RenderGhostCubeSide(t, x, y, z, i);
                        t->FlushQuads();
                        glPopName();
                    }
                    delete t;
                    //������ ����� glPushName() ������ ����������� ������� glPopName()
                    glPopName();
                }
            }
        }
    }
}

/*
 * PickObject()
 * ������� �����, ������� ����������, �� ����� ��� (����) ������� �����.
 * ���� ����� ���������� �������� ����� ����������� �����.
 * ������������ ��������:
 * !=0:
 * ����� ������� �� ����. ���������� � ����� �������� � ��������� HITRESULT.
 * 0:
 * ����� ������� � ������� ��� ��������� ������� ������ �� ���������� �����.
 */
int ObjectSelector::PickObject(float radius) {
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
    glRotatef(player->rotationRoll,  0.0f, 0.0f, 1.0f);
    glRotatef(player->rotationPitch, 1.0f, 0.0f, 0.0f);
    glRotatef(player->rotationYaw,   0.0f, 1.0f, 0.0f);
    glTranslatef(-player->positionX - player->GetCameraOffsetZ(),
        -player->positionY - player->height,
        -player->positionZ - player->GetCameraOffsetZ());

    RenderGhostCubeSides(radius);
    
    //�������� ���������� ������� � ������ ������.
    //���������� ������� ����� ���������� ��������� ��������.
    hitsCount = glRenderMode(GL_RENDER);
    
    /*
     * ������ ����� ���������������� ������������ ����� ������.
     * ����� ������ ��� ������� ������ �� ����� ���� GLuint.
     * ������ �������� ������ ��������, �� ������� ������� ����� � ������ ������ RenderGhostCubeSides().
     * � ����� ������, ��� "�������" �������� ��������� (�������) ������ 
     * ��� ���������� ������. ��� ���� ������� �� ������ � ������ ������� ������� ������.
     * ����� ����� ������� �� ����, �� � ����� ������ ������������
     * ���������� �� ������ � ��� �������, ������� ����� �����,
     * �� � ��� ���� �������� �����, ����� ������� �������� ���.
     * �� ����, ��� �������� ����� ��� ����� ��������.
     * ���� �� ������ ��������� ��� ���� ���� � ��� � ���� ��������,
     * �� ���������� � ��� �������� ���� ����� �������� � ����� ������.
     * �� ����, ���� � ������� ����� �������� ���� � ����� �� ���� �������,
     * �� � ����� �������, ��� ������� ��� ��� ����� - ��, �������
     * ����� ����� � ��������� � ����������� �� ���� ��������� ������.
     * ���� ����� ������� � ����� ���� �����, �� � ����� ����� �������
     * ��� � ����� ������. ��� ��������, ��� ���������� ��������� ������
     * � ����� ��������� � ������ �����.
     * ��� �������� ������� ���������� ������� �������, � ������� ��������� 
     * �� ��������� ���������, ������� ����������� ��������� �������:
     * [0]: ���������� ���� � ����� (��������� �����)
     * [1]: ����������� ���������� �� ������
     * [2]: ������������ ���������� �� ������
     * [3]: ID �����, �������� ����������� ������� �����. �� ����� ID �����
     *      ���������� � �������� ������� � ������ World � �������� ������
     *      ���������� � �����.
     * [4]: ID ����� �����. ��� ������������� ��������� �� 0 �� 5, ������������
     *      �������� ����� �����. � �� ������� ����� ����������, �� �����
     *      ������ ������� (�����) ����� ������� �����.
     *      ������ ���� ����� �������� ����� ���������� � ������������ ����� "block.h".
     * 
     * ����� ����� ��� �����, ������� ����� ������������ ��� �������� ������� glPushName().
     * � ������ ������, ��� ����������� � ������� [3] � [4]. �� ���������� ����� 
     * ���� ������ � ��� �������� � ������ (�������) ������ �������.
     * �� ��� ��� ���� "���" � ��� ������ ���, �� ��������� ������� ������ ��� ������.
     * �� ����� ������ ����������.
     * ������� � ������ ������� � �������� [5], �� ����������� ������, �� ���
     * ��� ��������� �����. ����� [10], [15], [20] � �.�.
     * ����� ������� ���������� ��� �������� ������� � ���� ��������� � ������ �����.
     */
    if (hitsCount) {
        GLuint closest = selectBuffer[1];
        int cubeId = selectBuffer[3];
        int sideId = selectBuffer[4];
        for (int i = 0; i < hitsCount; i++) {
            int id = i * 5;
            GLuint dist = selectBuffer[id + 1];
            if (dist < closest) {
                closest = dist;
                cubeId = selectBuffer[id + 3];
                sideId = selectBuffer[id + 4];
            }
        }

        hitResult->blockId = cubeId;
        hitResult->blockSideId = sideId;
        hitResult->x = player->worldObj->blocks[cubeId]->positionX;
        hitResult->y = player->worldObj->blocks[cubeId]->positionY;
        hitResult->z = player->worldObj->blocks[cubeId]->positionZ;

        //���������� ���������� ������� ����� � ������,
        //���� ����� ����� ��������� ����� ����.
        switch (hitResult->blockSideId) {
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
    }
    else {
        memset(hitResult, -1, sizeof(HITRESULT));
    }

    return hitsCount;
}

int ObjectSelector::GetHitsCount() {
    return hitsCount;
}

std::string ObjectSelector::SideIdToString(int side) {
    switch (side) {
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

std::string ObjectSelector::HitResultToString() {
    return "[" + std::to_string(hitResult->x) + "," +
        std::to_string(hitResult->y) + "," +
        std::to_string(hitResult->z) + "] " +
        SideIdToString(hitResult->blockSideId);
}

std::string ObjectSelector::HitResultNearToString() {
    return std::to_string(hitResult->xNear) + "," +
        std::to_string(hitResult->yNear) + "," +
        std::to_string(hitResult->zNear);
}
