#include "ObjectSelector.h"
#include "block.h"
#include "CollisionBox.h"
#include "frustum.h"
#include "main.h"
#include "player.h"
#include "tessellator.h"
#include "world.h"

CObjectSelector::CObjectSelector(CPlayer* player) 
{
    this->player = player;
    hitResult = new HITRESULT();
    hitsCount = 0;
    ZeroMemory(&selectBuffer, SELECT_BUFFER_SIZE);
    ZeroMemory(&viewportBuffer, 16);
}

CObjectSelector::~CObjectSelector() 
{
    delete hitResult;
}

/*
 * RenderHitCubeSide()
 * ������������ ����� ����, �� ������� ������� �����.
 */
void CObjectSelector::RenderHitCubeSide(CTessellator* t) 
{
    float x0 = (float)hitResult->x;
    float y0 = (float)hitResult->y;
    float z0 = (float)hitResult->z;
    float x1;
    float y1;
    float z1;
    switch (hitResult->blockSideId) 
    {
        case BLOCK_SIDE_FRONT:
            x1 = x0 + player->world->blockWidth;
            y1 = y0 + player->world->blockHeight;
            z1 = z0 + player->world->blockWidth;
            t->AddVertexQuad(x0, y0, z1);
            t->AddVertexQuad(x1, y0, z1);
            t->AddVertexQuad(x1, y1, z1);
            t->AddVertexQuad(x0, y1, z1);
            break;

        case BLOCK_SIDE_BACK:
            x1 = x0 + player->world->blockWidth;
            y1 = y0 + player->world->blockHeight;
            t->AddVertexQuad(x0, y0, z0);
            t->AddVertexQuad(x1, y0, z0);
            t->AddVertexQuad(x1, y1, z0);
            t->AddVertexQuad(x0, y1, z0);
            break;

        case BLOCK_SIDE_LEFT:
            y1 = y0 + player->world->blockHeight;
            z1 = z0 + player->world->blockWidth;
            t->AddVertexQuad(x0, y0, z0);
            t->AddVertexQuad(x0, y0, z1);
            t->AddVertexQuad(x0, y1, z1);
            t->AddVertexQuad(x0, y1, z0);
            break;

        case BLOCK_SIDE_RIGHT:
            x1 = x0 + player->world->blockWidth;
            y1 = y0 + player->world->blockHeight;
            z1 = z0 + player->world->blockWidth;
            t->AddVertexQuad(x1, y0, z0);
            t->AddVertexQuad(x1, y0, z1);
            t->AddVertexQuad(x1, y1, z1);
            t->AddVertexQuad(x1, y1, z0);
            break;

        case BLOCK_SIDE_BOTTOM:
            x1 = x0 + player->world->blockWidth;
            z1 = z0 + player->world->blockWidth;
            t->AddVertexQuad(x0, y0, z0);
            t->AddVertexQuad(x1, y0, z0);
            t->AddVertexQuad(x1, y0, z1);
            t->AddVertexQuad(x0, y0, z1);
            break;

        case BLOCK_SIDE_TOP:
            x1 = x0 + player->world->blockWidth;
            y1 = y0 + player->world->blockHeight;
            z1 = z0 + player->world->blockWidth;
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
void CObjectSelector::RenderGhostCubeSide(CTessellator* t, int x, int y, int z, int sideId) 
{
    float x0 = (float)x;
    float y0 = (float)y;
    float z0 = (float)z;
    float x1;
    float y1;
    float z1;
    switch (sideId)
    {
        case BLOCK_SIDE_FRONT:
            if (!player->world->IsBlockSolid(x, y, z + 1)) 
            {
                x1 = x0 + player->world->blockWidth;
                y1 = y0 + player->world->blockHeight;
                z1 = z0 + player->world->blockWidth;
                t->AddVertexQuad(x0, y0, z1);
                t->AddVertexQuad(x1, y0, z1);
                t->AddVertexQuad(x1, y1, z1);
                t->AddVertexQuad(x0, y1, z1);
            }
            break;

        case BLOCK_SIDE_BACK:
            if (!player->world->IsBlockSolid(x, y, z - 1)) 
            {
                x1 = x0 + player->world->blockWidth;
                y1 = y0 + player->world->blockHeight;
                t->AddVertexQuad(x0, y0, z0);
                t->AddVertexQuad(x1, y0, z0);
                t->AddVertexQuad(x1, y1, z0);
                t->AddVertexQuad(x0, y1, z0);
            }
            break;

        case BLOCK_SIDE_LEFT:
            if (!player->world->IsBlockSolid(x - 1, y, z)) 
            {
                y1 = y0 + player->world->blockHeight;
                z1 = z0 + player->world->blockHeight;
                t->AddVertexQuad(x0, y0, z0);
                t->AddVertexQuad(x0, y0, z1);
                t->AddVertexQuad(x0, y1, z1);
                t->AddVertexQuad(x0, y1, z0);
            }
            break;

        case BLOCK_SIDE_RIGHT:
            if (!player->world->IsBlockSolid(x + 1, y, z)) 
            {
                x1 = x0 + player->world->blockWidth;
                y1 = y0 + player->world->blockHeight;
                z1 = z0 + player->world->blockWidth;
                t->AddVertexQuad(x1, y0, z0);
                t->AddVertexQuad(x1, y0, z1);
                t->AddVertexQuad(x1, y1, z1);
                t->AddVertexQuad(x1, y1, z0);
            }
            break;

        case BLOCK_SIDE_TOP:
            if (!player->world->IsBlockSolid(x, y + 1, z)) 
            {
                x1 = x0 + player->world->blockWidth;
                y1 = y0 + player->world->blockHeight;
                z1 = z0 + player->world->blockWidth;
                t->AddVertexQuad(x0, y1, z0);
                t->AddVertexQuad(x0, y1, z1);
                t->AddVertexQuad(x1, y1, z1);
                t->AddVertexQuad(x1, y1, z0);
            }
            break;

        case BLOCK_SIDE_BOTTOM:
            if (!player->world->IsBlockSolid(x, y - 1, z)) 
            {
                x1 = x0 + player->world->blockWidth;
                z1 = z0 + player->world->blockWidth;
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
void CObjectSelector::RenderGhostCubeSides(float r) 
{
    /*
     * ������ ��������� ������� ������������.
     * ��� ����� ��������� ������� ������������ ������ �� ��� �������.
     * ��� ����� ��� ����������� ������� ������ ������ �������� R,
     * � �������� ������� ����� ������������� ����� ��������� � ������ �����.
     */
    CCollisionBox* box = player->collisionBox->Grow(r, r, r);
    int x0 = (int)box->x0;
    int x1 = (int)box->x1;
    int y0 = (int)box->y0;
    int y1 = (int)box->y1;
    int z0 = (int)box->z0;
    int z1 = (int)box->z1;
    delete box;

    glInitNames();

    //���������� ��� ����� � �������� ������� box
    for (int x = x0; x <= x1; x++) 
    {
        for (int y = y0; y <= y1; y++) 
        {
            for (int z = z0; z <= z1; z++) 
            {
                /*
                 * ���� � ����������� ���� [x,y,z] ���� ���� � �� �������� �� frustum,
                 * �� ��������� ��� ID � ����� ������ � ������ ��������� �����.
                 * � ��� �� ��������� � ����� ID ������ ����� ���� (�����).
                 * ���� ����� ������������� ������ ������,
                 * ��� �� ����� ���������� � ��������� � �����.
                 */
                if (player->world->GetBlockId(x, y, z) &&
                    frustumObj->CubeInFrustum((float)x, (float)y, (float)z, player->world->cubeSize)) 
                {
                    /*
                     * �������� ����� (ID) ������ � ������ � �������.
                     * ��� ����� ��� ��������� ��������� ���������� � �����,
                     * ���� ����� ��� �������.
                     */
                    int id = player->world->GetArrayId(x, y, z);

                    /*
                     * glPushName() ��������� ���� ID � ����� ������.
                     * �����, ������� ���� ����� ������.
                     */
                    glPushName(id);

                    CTessellator* t = new CTessellator();
                    for (int i = 0; i < 6; i++) 
                    {
                        t->ClearQuads();

                        /*
                         * ��������� � ����� ������ ID ������ ����� ���� � ������ ��.
                         * �������� ����� ����� �������� glPushName() � glPopName(),
                         * ����� � ����� ������ ������ �� �����������.
                         */
                        glPushName(i);
                        
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
 * ����� ������� �� ����. ���������� � ����� �������� � ������������� ��������� HITRESULT.
 * 0:
 * ����� ������� � ������� ��� ��������� ������� ������ �� ���������� �����.
 */
int CObjectSelector::PickObject(float radius) 
{
    ZeroMemory(&selectBuffer, SELECT_BUFFER_SIZE);
    glSelectBuffer(SELECT_BUFFER_SIZE, selectBuffer);
    glGetIntegerv(GL_VIEWPORT, viewportBuffer);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glRenderMode(GL_SELECT);
    gluPickMatrix((double)viewportWidth / 2.0, (double)viewportHeight / 2.0, 2.0, 2.0, viewportBuffer);
    gluPerspective(45.0, (double)viewportWidth / (double)viewportHeight, 0.1, 50.0);
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
     *      ���������� � �������� ������� � ������ CWorld � �������� ������
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
    if (hitsCount) 
    {
        GLuint closest = selectBuffer[1];
        int cubeId = selectBuffer[3];
        int sideId = selectBuffer[4];
        for (int i = 1; i < hitsCount; i++) 
        {
            int id = i * 5;
            GLuint dist = selectBuffer[id + 1];
            if (dist < closest) 
            {
                closest = dist;
                cubeId = selectBuffer[id + 3];
                sideId = selectBuffer[id + 4];
            }
        }

        hitResult->blockId = cubeId;
        hitResult->blockSideId = sideId;
        CBlock* block = player->world->GetBlock(cubeId);
        hitResult->x = block->GetPositionX();
        hitResult->y = block->GetPositionY();
        hitResult->z = block->GetPositionZ();
        delete block;

        //���������� ���������� ������� ����� � ������,
        //���� ����� ����� ��������� ����� ����.
        switch (hitResult->blockSideId) 
        {
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
    else 
    {
        memset(hitResult, -1, sizeof(HITRESULT));
    }

    return hitsCount;
}

int CObjectSelector::GetHitsCount() 
{
    return hitsCount;
}

std::string CObjectSelector::SideIdToString(int sideId)
{
    switch (sideId)
    {
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

std::string CObjectSelector::HitResultToString() 
{
    return "[" + std::to_string(hitResult->x) + "," +
        std::to_string(hitResult->y) + "," +
        std::to_string(hitResult->z) + "] " +
        SideIdToString(hitResult->blockSideId);
}

std::string CObjectSelector::HitResultNearToString()
{
    return std::to_string(hitResult->xNear) + "," +
        std::to_string(hitResult->yNear) + "," +
        std::to_string(hitResult->zNear);
}
