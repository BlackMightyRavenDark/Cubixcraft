#pragma once
#include "world.h"
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>

class Player {
public:
	float positionX;
	float positionY;
	float positionZ;
	float rotationYaw;
	float rotationPitch;
	float rotationRoll;

	float walkingSpeed;
	float flyingSpeed;

    float motionX;
	float motionY;
	float motionZ;

	float gravityX;
	float gravityY;
	float gravityZ;

	float jumpHeight;
	float jumpGravity;
	bool onGround;

	const float height = 1.62;

    World *worldObj;

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
	const float motionCoef = 0.45;

    float cameraOffsetX;
    float cameraOffsetZ;

    void MoveRelative(float xOffset, float yOffset, float zOffset, float speed);
    bool KeyPressed(int vKey);
    void HandleMovement(double speed);
    void HandleMouseMovement();
};
