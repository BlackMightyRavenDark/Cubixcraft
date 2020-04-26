#include "player.h"
#include "block.h"
#include "collisionbox.h"
#include "frustum.h"
#include "main.h"
#include "world.h"

Player::Player(World* world, float positionX, float positionY, float positionZ,
               float rotationYaw, float rotationPitch, float rotationRoll) {
    this->positionX     = positionX;
    this->positionY     = positionY;
    this->positionZ     = positionZ;
    this->rotationYaw   = rotationYaw;
    this->rotationPitch = rotationPitch;
    this->rotationRoll  = rotationRoll;

    walkingSpeed = 1.0f;

    gravityY = 1.0f;

    jumpHeight = 1.0f;

    onGround = false;

    SetMotionVector(0.0f, 3.0f, 0.0f);

    worldObj = world;

    onCameraChanged = nullptr;

    //область столкновения это по сути длинна, ширина и рост игрока.
    collisionBox = new CollisionBox(positionX, positionY, positionZ,
        positionX + 0.7f, positionY + height, positionZ + 0.7f);

    /*
     * Смещение позиции камеры к центру области столкновения игрока.
     * Иначе, камера была бы слишком близко или далеко от объекта столкновения,
     * в зависимости от того, в какую сторону смотрит игрок.
     */
    cameraOffsetX = (collisionBox->x1 - collisionBox->x0) / 2.0f;
    cameraOffsetZ = (collisionBox->z1 - collisionBox->z0) / 2.0f;    
}

Player::~Player() {
    delete collisionBox;
}

/*
 * SetPosition()
 * Перемещает игрока на указанную позицию.
 */
void Player::SetPosition(float xPos, float yPos, float zPos) {
    positionX = xPos;
    positionY = yPos;
    positionZ = zPos;
    collisionBox->MoveAbsolute(positionX, positionY, positionZ);
    if (onCameraChanged) {
        onCameraChanged();
    }
}

/*
 * SetRotation()
 * Разворачивает игрока.
 */
void Player::SetRotation(float xRot, float yRot, float zRot) {
    rotationYaw   = xRot;
    rotationPitch = yRot;
    rotationRoll  = zRot;
    if (onCameraChanged) {
        onCameraChanged();
    }
}

float Player::GetCameraOffsetX() {
    return cameraOffsetX;
}

float Player::GetCameraOffsetZ() {
    return cameraOffsetZ;
}

/*
 * Reset()
 * Перемещает игрока на позицию по-умолчанию (respawn).
 */
void Player::Reset() {
    SetPosition(0.0f, worldObj->sizeZ + 1.0f, 0.0f);
    SetRotation(136.0f, 0.0f, 0.0f);
    SetMotionVector(0.0f, 3.0f, 0.0f);
    if (onCameraChanged) {
        onCameraChanged();
    }
}

/*
 * SetMotionVector()
 * Устанавливает текущий вектор движения игрока.
 */
void Player::SetMotionVector(float xMotion, float yMotion, float zMotion) {
    motionX = xMotion * motionCoef;
    motionY = yMotion * motionCoef;
    motionZ = zMotion * motionCoef;
}

/*
 * SetupCamera()
 * Перемещает камеру относительно позиции игрока.
 */
void Player::SetupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0, (double)glScreenWidth / (double)glScreenHeight, 0.03, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(rotationRoll,  0.0f, 0.0f, 1.0f);
    glRotatef(rotationPitch, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationYaw,   0.0f, 1.0f, 0.0f);

    glTranslatef(-positionX - cameraOffsetX, -positionY - height, -positionZ - cameraOffsetZ);

    if (bNeedToRecalculateFrustum) {
        frustumObj->CalculateFrustum();
        bNeedToRecalculateFrustum = false;
    }
}

/*
 * MoveRelative()
 * Вычисляет текущий вектор движения игрока.
 * Этот вектор каждую итерацию умножается на коэффициент трения.
 * Таким образом, если значение трения меньше единицы, то игрок постепенно останавливается.
 */
void Player::MoveRelative(float xOffset, float yOffset, float zOffset, float speed) {
    if (yOffset > 0.0f && onGround) {
        onGround = false;
        motionY = jumpHeight * jumpCoef;
    }
    float dist = powf(xOffset, 2.0f) + powf(zOffset, 2.0f);
    if (dist < 0.01f) {
        return;
    }
    dist = speed / sqrt(dist);
    xOffset *= dist;
    zOffset *= dist;
    float a = sinf(rotationYaw * (float)M_PI / 180.0f);
    float b = cosf(rotationYaw * (float)M_PI / 180.0f);
    motionX += xOffset * b - zOffset * a;
    motionZ += zOffset * b + xOffset * a;
}

bool Player::KeyPressed(int vKey) {
    short n = GetKeyState(vKey);
    return n < 0;
}

/*
 * HandleMovement()
 * Обработка клавиш, отвечающих за перемещение игрока.
 */
void Player::HandleMovement(float speed) {
    int xAxis = 0;
    int yAxis = 0;
    int zAxis = 0;
    if (KeyPressed(VK_W) || KeyPressed(VK_UP))    zAxis--;
    if (KeyPressed(VK_S) || KeyPressed(VK_DOWN))  zAxis++;
    if (KeyPressed(VK_A) || KeyPressed(VK_LEFT))  xAxis--;
    if (KeyPressed(VK_D) || KeyPressed(VK_RIGHT)) xAxis++;
    if (KeyPressed(VK_SPACE)) yAxis++;

    if (xAxis || yAxis || zAxis) {
        MoveRelative((float)xAxis, (float)yAxis, (float)zAxis, speed);
    }
}

/*
 * HandleMouseMovement()
 * Разворачивает игрока в соответствии с движением мыши.
 */
void Player::HandleMouseMovement() {
    POINT mousePos;
    if (GetCursorPos(&mousePos)) {
        if (mousePos.x != oldMousePos.x || mousePos.y != oldMousePos.y) {
            POINT mouseDelta;
            mouseDelta.x = mousePos.x - oldMousePos.x;
            mouseDelta.y = mousePos.y - oldMousePos.y;
            
            float yaw   = mouseDelta.x * mouseSensitivity * 0.1f;
            float pitch = mouseDelta.y * mouseSensitivity * 0.1f;

            rotationYaw   += yaw;
            rotationPitch += pitch;

            //ограничить разворот игрока, чтобы предотвратить переполнение переменной
            while (rotationYaw < 0.0f) {
                rotationYaw += 360.0f;
            }
            while (rotationYaw >= 360.0f) {
                rotationYaw -= 360.0f;
            }

            if (rotationPitch > 90.0f) {
                rotationPitch = 90.0f;
            }
            else
            {
                if (rotationPitch < -90.0f) {
                    rotationPitch = -90.0f;
                }
            }

            //вернуть курсор мыши в центр окна игры
            oldMousePos.x = glScreenWidth / 2;
            oldMousePos.y = glScreenHeight / 2;
            ClientToScreen(hPanelRender, &oldMousePos);
            SetCursorPos(oldMousePos.x, oldMousePos.y);
        }
    }
}

/*
 * Tick()
 * Жизненный цикл игрока.
 * В текущей версии здесь обрабатывается только перемещение игрока
 * и проверка его столкновений с блоками.
 */
void Player::Tick() {
    float oldRotationYaw   = rotationYaw;
    float oldRotationPitch = rotationPitch;
    float oldPositionX = positionX;
    float oldPositionY = positionY;
    float oldPositionZ = positionZ;

    if (mouseGrabbed) {
        HandleMouseMovement();
    }
    if (GetForegroundWindow() == hMainWindow) {
        HandleMovement(walkingSpeed * speedCoef * (float)deltaTime);
    }

    //симуляция гравитации
    motionY -= gravityY * gravityCoef * (float)deltaTime;
    Move(motionX, motionY * (float)deltaTime, motionZ);

    //чем больше коэффициенты трения, тем сильнее игрок скользит
    motionX *= frictionCoefX;
    motionZ *= frictionCoefZ;

    if ((positionX != oldPositionX || positionY != oldPositionY || positionZ != oldPositionZ ||
        rotationYaw != oldRotationYaw || rotationPitch != oldRotationPitch) && onCameraChanged) {
        onCameraChanged();
    }
}

/*
 * Move()
 * Перемещает игрока на указанное расстояние относительно текущей позиции.
 */
void Player::Move(float xAxis, float yAxis, float zAxis) {
    float xAxisOld = xAxis;
    float yAxisOld = yAxis;
    float zAxisOld = zAxis;

    //получить массив блоков вокруг игрока
    CollisionBox* box = collisionBox->Grow(1.0f, height <= 1.0f ? 2.0f : height, 1.0f);
    std::vector<Block*> nearBlocks = worldObj->GetBlocksInBox(box);
    delete box;

    //проверить, столкнулся ли игрок с блоками из массива
    for (size_t i = 0; i < nearBlocks.size(); i++) {
        if (nearBlocks[i]->GetId()) {
            xAxis = nearBlocks[i]->collisionBox->ClipCollideX(*collisionBox, xAxis);
            yAxis = nearBlocks[i]->collisionBox->ClipCollideY(*collisionBox, yAxis);
            zAxis = nearBlocks[i]->collisionBox->ClipCollideZ(*collisionBox, zAxis);
        }
    }

    //передвинуть область столкновения игрока
    collisionBox->MoveRelative(xAxis, yAxis, zAxis);

    if (xAxisOld != xAxis) {
        motionX = 0.0f;
    }
    if (yAxisOld != yAxis) {
        motionY = 0.0f;
    }
    if (zAxisOld != zAxis) {
        motionZ = 0.0f;
    }

    //предотвращение падения игрока в бесконечное небытиё
    if (collisionBox->y0 < 0.0f) {
        collisionBox->MoveAbsolute(collisionBox->x0, 0.0f, collisionBox->z0);
        onGround = true;
        motionY = 0.0f;
    }
    else {
       onGround = ((yAxisOld != yAxis && yAxisOld < 0.0f) || collisionBox->y0 <= 0.0f);
    }

    positionX = collisionBox->x0;
    positionY = collisionBox->y0;
    positionZ = collisionBox->z0;
}
