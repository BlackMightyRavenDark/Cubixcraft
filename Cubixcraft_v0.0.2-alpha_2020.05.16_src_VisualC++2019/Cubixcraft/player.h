/*
 * ����� CPlayer ��������� ������� ������� � �������� ���������� � ���������, ����������� � ������.
 * ������������ ����� ��� ���� ����� �������� ��-��������� ����� ���� ���������� � �������.
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>

class CCollisionBox;
class CWorld;

class CPlayer {
public:
    float positionX;
    float positionY;
    float positionZ;
    float rotationYaw;
    float rotationPitch;
    float rotationRoll;

    //ID �����, ������� ������ �����
    char blockIdInHand;

    //���� ������
    const float height = 1.62f;

    //������� ������������ ������
    CCollisionBox* collisionBox;

    CWorld* worldObj;

    void (*onCameraChanged)();

    CPlayer(CWorld* world, float positionX, float positionY, float positionZ,
        float rotationYaw, float rotationPitch, float rotationRoll);
    ~CPlayer();

    void SetPosition(float xPos, float yPos, float zPos);
    void SetRotation(float xRot, float yRot, float zRot);
    float GetCameraOffsetX();
    float GetCameraOffsetZ();
    void Reset();
    void SetMotionVector(float xMotion, float yMotion, float zMotion);
    void SetupCamera();
    void Move(float xAxis, float yAxis, float zAxis);
    void Tick();

private:
    //���������������� ����
    const float mouseSensitivity = 1.0f;
    const float mouseSensitivityCoef = 0.1f;

    //�������� ����������� ������ ������
    float walkingSpeed;
    const float speedCoef = 2.0f;

    //������ �������� ������
    float motionX;
    float motionY;
    float motionZ;

    //������ ������ ��� �������� �� �����������.
    //������ �������� �� ���� X,Z ���������� �� ��� ������������.
    const float frictionCoefX = 0.0f;
    const float frictionCoefZ = 0.0f;

    //�� ��� ������ ����� ����
    float gravityY;
    const float gravityCoef = 18.0f;

    //������ ������
    float jumpHeight;
    const float jumpCoef = 7.7f;

    //����������� ��������
    const float motionCoef = 7.0f;

    //�������� ������ ������������ ������� ������
    float cameraOffsetX;
    float cameraOffsetZ;

    //����� ����� �� ����������� ��� ���
    bool onGround;

    void MoveRelative(float xOffset, float yOffset, float zOffset, float speed);
    void HandleMovement(float speed);
    void HandleMouseMovement();
};
