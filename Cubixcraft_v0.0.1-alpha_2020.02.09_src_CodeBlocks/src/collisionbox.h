class CollisionBox {
public:
    float x0;
    float y0;
    float z0;
    float x1;
    float y1;
    float z1;

    CollisionBox(float x0, float y0, float z0, float x1, float y1, float z1);

    CollisionBox* Grow(float x, float y, float z);
    CollisionBox* Expand(float x, float y, float z);
    bool Intersects(CollisionBox box);
    void MoveRelative(float xOffset, float yOffset, float zOffset);
    void MoveAbsolute(float x, float y, float z);
    float ClipCollideX(CollisionBox box, float xMotion);
    float ClipCollideY(CollisionBox box, float yMotion);
    float ClipCollideZ(CollisionBox box, float zMotion);
};
