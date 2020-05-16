#include "collisionbox.h"

CCollisionBox::CCollisionBox(float x0, float y0, float z0, float x1, float y1, float z1) 
{
    this->x0 = x0;
    this->y0 = y0;
    this->z0 = z0;
    this->x1 = x1;
    this->y1 = y1;
    this->z1 = z1;
}

/*
 * Grow()
 * Расширить область столкновения по трём осям.
 * Создаёт новый экземпляр класса CCollisionBox и возвращает указатель на него.
 * Этот экземпляр необходимо уничтожить вручную, когда он больше не нужен.
 */
CCollisionBox* CCollisionBox::Grow(float x, float y, float z) 
{
    CCollisionBox* cb = new CCollisionBox(x0 - x, y0 - y, z0 - z, x1 + x, y1 + y, z1 + z);
    return cb;
}

/*
 * Intersects()
 * Проверяет, пересекаются ли два объекта друг с другом.
 */
bool CCollisionBox::Intersects(CCollisionBox* box) 
{
    return (x1 > box->x0 && box->x1 > x0) && 
           (y1 > box->y0 && box->y1 > y0) && 
           (z1 > box->z0 && box->z1 > z0);
}

/*
 * MoveRelative()
 * Перемещает область столкновения на указанное расстояние, относительно текущей позиции.
 */
void CCollisionBox::MoveRelative(float xOffset, float yOffset, float zOffset) 
{
    x0 += xOffset;
    y0 += yOffset;
    z0 += zOffset;
    x1 += xOffset;
    y1 += yOffset;
    z1 += zOffset;
}

/*
 * MoveAbsolute()
 * Перемещает область столкновения на указанную позицию.
 */
void CCollisionBox::MoveAbsolute(float x, float y, float z) 
{
    float xDelta = x1 - x0;
    float yDelta = y1 - y0;
    float zDelta = z1 - z0;
    x0 = x;
    y0 = y;
    z0 = z;
    x1 = x0 + xDelta;
    y1 = y0 + yDelta;
    z1 = z0 + zDelta;
}

/*
 * ClipCollideX()
 * Блокирует перемещение текущего объекта по оси X, если он столкнулся с другим объектом.
 */
float CCollisionBox::ClipCollideX(CCollisionBox box, float xMotion) 
{
    if (box.y1 <= y0 || box.y0 >= y1) 
    {
        return xMotion;
    }
    if (box.z1 <= z0 || box.z0 >= z1) 
    {
        return xMotion;
    }
    if (xMotion > 0.0f && box.x1 <= x0) 
    {
        float _max = x0 - box.x1;
        if (_max < xMotion) 
        {
            xMotion = _max;
        }
    }
    if (xMotion < 0.0f && box.x0 >= x1) 
    {
        float _max = x1 - box.x0;
        if (_max > xMotion) 
        {
            xMotion = _max;
        }
    }
    return xMotion;
}

/*
 * ClipCollideY()
 * Блокирует перемещение текущего объекта по оси Y, если он столкнулся с другим объектом.
 */
float CCollisionBox::ClipCollideY(CCollisionBox box, float yMotion) 
{
    if (box.x1 <= x0 || box.x0 >= x1) {
        return yMotion;
    }
    if (box.z1 <= z0 || box.z0 >= z1) 
    {
        return yMotion;
    }
    if (yMotion > 0.0f && box.y1 <= y0) 
    {
        float _max = y0 - box.y1;
        if (_max < yMotion) {
            yMotion = _max;
        }
    }
    if (yMotion < 0.0f && box.y0 >= y1) 
    {
        float _max = y1 - box.y0;
        if (_max > yMotion) 
        {
            yMotion = _max;
        }
    }
    return yMotion;
}

/*
 * ClipCollideZ()
 * Блокирует перемещение текущего объекта по оси Z, если он столкнулся с другим объектом.
 */
float CCollisionBox::ClipCollideZ(CCollisionBox box, float zMotion) 
{
    if (box.x1 <= x0 || box.x0 >= x1) 
    {
        return zMotion;
    }
    if (box.y1 <= y0 || box.y0 >= y1) 
    {
        return zMotion;
    }
    if (zMotion > 0.0f && box.z1 <= z0) 
    {
        float _max = z0 - box.z1;
        if (_max < zMotion) 
        {
            zMotion = _max;
        }
    }
    if (zMotion < 0.0f && box.z0 >= z1) 
    {
        float _max = z1 - box.z0;
        if (_max > zMotion) 
        {
            zMotion = _max;
        }
    }
    return zMotion;
}
