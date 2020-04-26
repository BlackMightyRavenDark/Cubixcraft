package game.cubixcraft;

public class Block {

    public static final int BLOCK_SIDE_FRONT  = 0;
    public static final int BLOCK_SIDE_BACK   = 1;
    public static final int BLOCK_SIDE_LEFT   = 2;
    public static final int BLOCK_SIDE_RIGHT  = 3;
    public static final int BLOCK_SIDE_TOP    = 4;
    public static final int BLOCK_SIDE_BOTTOM = 5;

    public int positionX;
    public int positionY;
    public int positionZ;

    public int textureId;

    /*
     * Область столкновения блока может быть больше или меньше его размеров.
     * Это можно использовать при создании блока нестандартной формы.
     */
    public CollisionBox collisionBox;

    //ID блока
    private byte id;

    /*
     * Экземпляр класса мира.
     * Нужен для того, чтобы блок имел доступ к необходимым параметрам мира,
     * в котором он находится.
     */
    private World world;

    public Block(World world, byte id, int texId, int positionX, int positionY, int positionZ) {
        this.world = world;
        this.positionX = positionX;
        this.positionY = positionY;
        this.positionZ = positionZ;
        this.id = id;
        textureId = texId;
        collisionBox = new CollisionBox((float)positionX, (float)positionY, (float)positionZ,
                (float)positionX + world.blockWidth,
                (float)positionY + world.blockHeight,
                (float)positionZ + world.blockWidth);
    }

    public void setPosition(int xPos, int yPos, int zPos){
        positionX = xPos;
        positionY = yPos;
        positionZ = zPos;
        collisionBox.x0 = (float)xPos;
        collisionBox.y0 = (float)yPos;
        collisionBox.z0 = (float)zPos;
        collisionBox.x1 = (float)xPos + world.blockWidth;
        collisionBox.y1 = (float)yPos + world.blockHeight;
        collisionBox.z1 = (float)zPos + world.blockWidth;
    }

    public byte getId(){
        return id;
    }

    public void setId(byte newId){
        id = newId;
    }

}