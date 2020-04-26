/*
 * Класс Player управляет игровой камерой и содержит переменные и константы, относящиеся к игроку.
 * Коэффициенты нужны для того чтобы значения по-умолчанию можно было приравнять к единице.
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

    //скорость перемещения игрока пешком
    float walkingSpeed;
    const float speedCoef = 2.0f;

    //вектор движения игрока
    float motionX;
    float motionY;
    float motionZ;

    //Трение игрока при движении по поверхности.
    //Вектор движения по осям X,Z умножается на эти коэффициенты.
    const float frictionCoefX = 0.0f;
    const float frictionCoefZ = 0.0f;

    //то как игрока тянет вниз
    float gravityY;
    const float gravityCoef = 18.0f;

    //высота прыжка
    float jumpHeight;
    const float jumpCoef = 7.7f;

    //Коэффициент движения
    const float motionCoef = 7.0f;

    //смещение камеры относительно позиции игрока
    float cameraOffsetX;
    float cameraOffsetZ;

    //игрок стоит на поверхности или нет
    bool onGround;

    void MoveRelative(float xOffset, float yOffset, float zOffset, float speed);
    bool KeyPressed(int vKey);
    void HandleMovement(float speed);
    void HandleMouseMovement();
};
