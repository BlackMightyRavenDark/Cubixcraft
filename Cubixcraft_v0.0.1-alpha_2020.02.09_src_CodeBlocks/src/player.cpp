#include "player.h"
#include "main.h"

Player::Player(World* world, float positionX, float positionY, float positionZ,
               float rotationYaw, float rotationPitch, float rotationRoll){
    this->positionX     = positionX;
    this->positionY     = positionY;
    this->positionZ     = positionZ;
    this->rotationYaw   = rotationYaw;
    this->rotationPitch = rotationPitch;
    this->rotationRoll  = rotationRoll;

    walkingSpeed  = 1.0;

	gravityX      = 0.0;
	gravityY      = 0.98;
	gravityZ      = 0.0;

	jumpHeight    = 1.0;
	jumpGravity   = 1.0;

	onGround = false;

	SetMotionVector(0.0, 3.0, 0.0);

	worldObj = world;
	collisionBox = new CollisionBox(positionX, positionY, positionZ,
                                    positionX + 0.7, positionY + height, positionZ + 0.7);
    cameraOffsetX = (collisionBox->x1 - collisionBox->x0) / 2.0;
    cameraOffsetZ = (collisionBox->z1 - collisionBox->z0) / 2.0;
}

Player::~Player(){
    delete collisionBox;
}

void Player::SetPosition(float xPos, float yPos, float zPos){
    positionX = xPos;
    positionY = yPos;
    positionZ = zPos;
	collisionBox->MoveAbsolute(positionX, positionY, positionZ);
    if (onCameraChanged){
        onCameraChanged();
    }
}

void Player::SetRotation(float xRot, float yRot, float zRot){
    rotationYaw = xRot;
    rotationPitch = yRot;
    rotationRoll = zRot;
    if (onCameraChanged){
        onCameraChanged();
    }
}

float Player::GetCameraOffsetX(){
    return cameraOffsetX;
}

float Player::GetCameraOffsetZ(){
    return cameraOffsetZ;
}

void Player::Reset(){
    SetPosition(0.0, worldObj->sizeZ + 1.0, 0.0);
    SetRotation(136.0, 0.0, 0.0);
	SetMotionVector(0.0, 3.0, 0.0);
	if (onCameraChanged){
        onCameraChanged();
    }
}

void Player::SetMotionVector(float xMotion, float yMotion, float zMotion){
    motionX = xMotion * motionCoef;
    motionY = yMotion * motionCoef;
    motionZ = zMotion * motionCoef;
}

void Player::SetupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(75.0, (double)glScreenWidth / (double)glScreenHeight, 0.03, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glRotatef(rotationRoll,  0.0, 0.0, 1.0);
	glRotatef(rotationPitch, 1.0, 0.0, 0.0);
	glRotatef(rotationYaw,   0.0, 1.0, 0.0);

	glTranslatef(-positionX - cameraOffsetX, -positionY - height, -positionZ - cameraOffsetZ);

    if (bNeedToRecalculateFrustum){
        frustumObj->CalculateFrustum();
        bNeedToRecalculateFrustum = false;
	}
}

void Player::MoveRelative(float xOffset, float yOffset, float zOffset, float speed) {
    if (yOffset > 0.0 && onGround){
        onGround = false;
        motionY = jumpHeight * motionCoef;
    }
 	float dist = powf(xOffset, (float)2) + powf(zOffset, (float)2);
    if (dist < 0.01)
		return;
	dist = speed / sqrt(dist);
	xOffset *= dist;
	zOffset *= dist;
	float a = sinf(rotationYaw * M_PI / 180.0);
	float b = cosf(rotationYaw * M_PI / 180.0);
	motionX += xOffset * b - zOffset * a;
	motionZ += zOffset * b + xOffset * a;
}

bool Player::KeyPressed(int vKey){
    short n = GetKeyState(vKey);
    return n < 0;
}

void Player::HandleMovement(double speed) {
	int xAxis = 0;
	int yAxis = 0;
	int zAxis = 0;
	if (KeyPressed(VK_W) || KeyPressed(VK_UP))    zAxis--;
	if (KeyPressed(VK_S) || KeyPressed(VK_DOWN))  zAxis++;
	if (KeyPressed(VK_A) || KeyPressed(VK_LEFT))  xAxis--;
    if (KeyPressed(VK_D) || KeyPressed(VK_RIGHT)) xAxis++;
    if (KeyPressed(VK_SPACE)) yAxis++;

	if (xAxis || yAxis || zAxis){
        MoveRelative(xAxis, yAxis, zAxis, speed);
	}
}

void Player::HandleMouseMovement() {
    POINT mousePos;
    if (GetCursorPos(&mousePos)){
        if (mousePos.x != oldMousePos.x || mousePos.y != oldMousePos.y){
            POINT mouseDelta;
            mouseDelta.x = mousePos.x - oldMousePos.x;
            mouseDelta.y = mousePos.y - oldMousePos.y;
            float mouseSpeed = 1.0;
            float yaw   = mouseDelta.x * mouseSpeed * 0.1;
            float pitch = mouseDelta.y * mouseSpeed * 0.1;

            rotationYaw   += yaw;
            rotationPitch += pitch;

            while (rotationYaw < 0.0) {
                rotationYaw += 360.0;
            }
            while (rotationYaw >= 360.0) {
                rotationYaw -= 360.0;
            }

            if (rotationPitch > 90.0) {
                rotationPitch = 90.0;
            }
            else
            if (rotationPitch < -90.0) {
                rotationPitch = -90.0;
            }

            oldMousePos.x = glScreenWidth / 2;
            oldMousePos.y = glScreenHeight / 2;
            ClientToScreen(hPanelRender, &oldMousePos);
            SetCursorPos(oldMousePos.x, oldMousePos.y);
        }
    }
}

void Player::Tick() {
    float oldRotationYaw   = rotationYaw;
    float oldRotationPitch = rotationPitch;
    float oldPositionX     = positionX;
    float oldPositionY     = positionY;
    float oldPositionZ     = positionZ;

    if (mouseGrabbed){
        HandleMouseMovement();
    }
    if (GetForegroundWindow() == hMainWindow){
        HandleMovement(walkingSpeed * 0.1);
    }
    motionY -= jumpGravity * 0.05;
    Move(motionX, motionY, motionZ);
    motionX *= gravityX;
    motionY *= gravityY;
    motionZ *= gravityZ;

    if ((positionX != oldPositionX || positionY != oldPositionY || positionZ != oldPositionZ ||
         rotationYaw != oldRotationYaw || rotationPitch != oldRotationPitch) && onCameraChanged){
        onCameraChanged();
    }
}

void Player::Move(float xAxis, float yAxis, float zAxis) {
    float xAxisOld = xAxis;
    float yAxisOld = yAxis;
    float zAxisOld = zAxis;
    CollisionBox* box = collisionBox->Grow(1.0, height <= 1.0 ? 2.0 : height * 2, 1.0);
    std::vector<Block*> nearBlocks = worldObj->GetBlocksInBox(box);
    delete box;
    for (size_t i = 0; i < nearBlocks.size(); i++){
        if (nearBlocks[i]->GetId()){
            CollisionBox* cb = nearBlocks[i]->collisionBox;
            xAxis = cb->ClipCollideX(*collisionBox, xAxis);
            yAxis = cb->ClipCollideY(*collisionBox, yAxis);
            zAxis = cb->ClipCollideZ(*collisionBox, zAxis);
        }
    }

    collisionBox->MoveRelative(xAxis, yAxis, zAxis);

    if (xAxisOld != xAxis) {
        motionX = 0.0;
    }
    if (yAxisOld != yAxis) {
        motionY = 0.0;
    }
    if (zAxisOld != zAxis) {
        motionZ = 0.0;
    }
    if (collisionBox->y0 < 0.0) {
        collisionBox->MoveAbsolute(collisionBox->x0, 0.0, collisionBox->z0);
        onGround = true;
        motionY = 0.0;
    } else {
        onGround = ((yAxisOld != yAxis && yAxisOld < 0.0) || collisionBox->y0 <= 0.0);
    }
    positionX = collisionBox->x0;
    positionY = collisionBox->y0;
    positionZ = collisionBox->z0;
}
