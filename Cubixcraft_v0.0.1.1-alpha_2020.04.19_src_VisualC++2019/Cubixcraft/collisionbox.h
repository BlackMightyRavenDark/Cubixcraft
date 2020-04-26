/*
 *  ласс CollisionBox используетс€ дл€ определени€ столкновений кубических и параллелепипедических объектов.
 * ѕо-умолчанию считаетс€, что задн€€ сторона объекта находитс€ в отрицательных координатах оси Z, а передн€€ -
 * в положительных. “о есть, по-умолчанию, объект смотрит в положительном направлении оси Z. “аким образом,
 * [x0,y0,z0] это права€ нижн€€ задн€€ точка объекта, а [x1,y1,z1] - лева€ верхн€€ передн€€ точка.
 */

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
    bool Intersects(CollisionBox box);
    void MoveRelative(float xOffset, float yOffset, float zOffset);
    void MoveAbsolute(float x, float y, float z);
    float ClipCollideX(CollisionBox box, float xMotion);
    float ClipCollideY(CollisionBox box, float yMotion);
    float ClipCollideZ(CollisionBox box, float zMotion);
};
