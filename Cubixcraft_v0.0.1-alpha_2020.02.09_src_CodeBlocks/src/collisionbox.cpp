#include "collisionbox.h"

CollisionBox::CollisionBox(float x0, float y0, float z0, float x1, float y1, float z1){
    this->x0 = x0;
    this->y0 = y0;
    this->z0 = z0;
    this->x1 = x1;
    this->y1 = y1;
    this->z1 = z1;
}

CollisionBox* CollisionBox::Grow(float x, float y, float z){
    CollisionBox* cb = new CollisionBox(x0 - x, y0 - y, z0 - z, x1 + x, y1 + y, z1 + z);
    return cb;
}

CollisionBox* CollisionBox::Expand(float x, float y, float z){
    float _x0 = this->x0;
    float _y0 = this->y0;
    float _z0 = this->z0;
    float _x2 = this->x1;
    float _y2 = this->y1;
    float _z2 = this->z1;
    if (x < 0.0) {
        _x0 += x;
    }
    if (x > 0.0) {
        _x2 += x;
    }
    if (y < 0.0) {
        _y0 += y;
    }
    if (y > 0.0) {
        _y2 += y;
    }
    if (z < 0.0) {
        _z0 += z;
    }
    if (z > 0.0) {
        _z2 += z;
    }
    CollisionBox* cb = new CollisionBox(_x0, _y0, _z0, _x2, _y2, _z2);
    return cb;
}

bool CollisionBox::Intersects(CollisionBox box){
    return (x1 > box.x0 && box.x1 > x0) && (y1 > box.y0 && box.y1 > y0) && (z1 > box.z0 && box.z1 > z0);
}

void CollisionBox::MoveRelative(float xOffset, float yOffset, float zOffset){
    x0 += xOffset;
    y0 += yOffset;
    z0 += zOffset;
    x1 += xOffset;
    y1 += yOffset;
    z1 += zOffset;
}

void CollisionBox::MoveAbsolute(float x, float y, float z){
    float xx = x1 - x0;
    float yy = y1 - y0;
    float zz = z1 - z0;
    x0 = x;
    y0 = y;
    z0 = z;
    x1 = x0 + xx;
    y1 = y0 + yy;
    z1 = z0 + zz;
}

float CollisionBox::ClipCollideX(CollisionBox box, float xMotion){
    if (box.y1 <= y0 || box.y0 >= y1) {
        return xMotion;
    }
    if (box.z1 <= z0 || box.z0 >= z1) {
        return xMotion;
    }
    if (xMotion > 0.0 && box.x1 <= x0) {
        float _max = x0 - box.x1;
        if (_max < xMotion) {
            xMotion = _max;
        }
    }
    if (xMotion < 0.0 && box.x0 >= x1) {
        float _max = x1 - box.x0;
        if (_max > xMotion) {
            xMotion = _max;
        }
    }
    return xMotion;
}

float CollisionBox::ClipCollideY(CollisionBox box, float yMotion) {
    if (box.x1 <= x0 || box.x0 >= x1) {
        return yMotion;
    }
    if (box.z1 <= z0 || box.z0 >= z1) {
        return yMotion;
    }
    if (yMotion > 0.0 && box.y1 <= y0) {
        float _max = y0 - box.y1;
        if (_max < yMotion) {
            yMotion = _max;
        }
    }
    if (yMotion < 0.0 && box.y0 >= y1) {
        float _max = y1 - box.y0;
        if (_max > yMotion) {
            yMotion = _max;
        }
    }
    return yMotion;
}


float CollisionBox::ClipCollideZ(CollisionBox box, float zMotion){
    if (box.x1 <= x0 || box.x0 >= x1) {
        return zMotion;
    }
    if (box.y1 <= y0 || box.y0 >= y1) {
        return zMotion;
    }
    if (zMotion > 0.0 && box.z1 <= z0) {
        float _max = z0 - box.z1;
        if (_max < zMotion) {
            zMotion = _max;
        }
    }
    if (zMotion < 0.0 && box.z0 >= z1) {
        float _max = z1 - box.z0;
        if (_max > zMotion) {
            zMotion = _max;
        }
    }
    return zMotion;
}
