/*
 * ����� Player ��������� ������� ������� � �������� ���������� � ���������, ����������� � ������.
 * ������������ ����� ��� ���� ����� �������� ��-��������� ����� ���� ���������� � �������.
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>

class CollisionBox;
class World;

class Player {
public:
    float positionX;
    float positionY;
    float positionZ;
    float rotationYaw;
    float rotationPitch;
    float rotationRoll;

    const float height = 1.62f;

    World* worldObj;

    CollisionBox* collisionBox;

    void (*onCameraChanged)();

    Player(World* world, float positionX, float positionY, float positionZ,
        float rotationYaw, float rotationPitch, float rotationRoll);
    ~Player();

    void Reset();
    void SetPosition(float xPos, float yPos, float zPos);
    void SetRotation(float xRot, float yRot, float zRot);
    float GetCameraOffsetX();
    float GetCameraOffsetZ();
    void SetMotionVector(float xMotion, float yMotion, float zMotion);
    void SetupCamera();
    void Tick();
    void Move(float xAxis, float yAxis, float zAxis);

private:
    const float mouseSensitivity = 1.0f;

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
    bool KeyPressed(int vKey);
    void HandleMovement(float speed);
    void HandleMouseMovement();
};
