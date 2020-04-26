#include <GL/gl.h>

const int BLOCK_SIDE_FRONT  = 0;
const int BLOCK_SIDE_BACK   = 1;
const int BLOCK_SIDE_LEFT   = 2;
const int BLOCK_SIDE_RIGHT  = 3;
const int BLOCK_SIDE_TOP    = 4;
const int BLOCK_SIDE_BOTTOM = 5;

class CollisionBox;
class World;

class Block {
public:
    int positionX;
    int positionY;
    int positionZ;

    GLuint textureId;

    /*
     * Область столкновения блока может быть больше или меньше его размеров.
     * Это можно использовать при создании блока нестандартной формы.
     */
    CollisionBox* collisionBox;

    Block(World* world, char id, GLuint texId, int positionX, int positionY, int positionZ);
    ~Block();

    void SetPosition(int xPos, int yPos, int zPos);
    char GetId();
    void SetId(char newId);

private:
    //ID блока
    char id;

    /*
     * Указатель на экземпляр класса мира.
     * Нужен для того, чтобы блок имел доступ к необходимым параметрам мира,
     * в котором он находится.
     */
    World* world;
};
