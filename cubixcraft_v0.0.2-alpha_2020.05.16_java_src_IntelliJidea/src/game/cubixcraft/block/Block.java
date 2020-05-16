package game.cubixcraft.block;

import game.cubixcraft.CollisionBox;
import game.cubixcraft.utils.*;
import game.cubixcraft.Tessellator;
import game.cubixcraft.world.World;

public class Block {

    public static final byte BLOCK_AIR_ID   = 0;
    public static final byte BLOCK_GRASS_ID = 1;
    public static final byte BLOCK_DIRT_ID  = 2;

    public static final int BLOCK_SIDE_FRONT  = 0;
    public static final int BLOCK_SIDE_BACK   = 1;
    public static final int BLOCK_SIDE_LEFT   = 2;
    public static final int BLOCK_SIDE_RIGHT  = 3;
    public static final int BLOCK_SIDE_TOP    = 4;
    public static final int BLOCK_SIDE_BOTTOM = 5;

    private int positionX;
    private int positionY;
    private int positionZ;

    //координаты текстуры блока в текстурном атласе
    protected Vec2i textureCoordinates;

    /*
     * Область столкновения блока может быть больше или меньше его размеров.
     * Это можно использовать при создании блока нестандартной формы.
     */
    public CollisionBox collisionBox;

    //ID блока
    private byte id;

    //название блока
    private String name;

    /*
     * Экземпляр класса мира.
     * Нужен для того, чтобы блок имел доступ к необходимым параметрам мира,
     * в котором он находится.
     */
    protected World world;

    public Block(World world, byte id, Vec2i textureCoord, String name) {
        this.world = world;
        this.id = id;
        textureCoordinates = textureCoord;
        this.name = name;
        collisionBox = new CollisionBox(0.0f, 0.0f, 0.0f,
                world.blockWidth, world.blockHeight, world.blockWidth);
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

    public int getPositionX() {
        return positionX;
    }

    public int getPositionY() {
        return positionY;
    }

    public int getPositionZ() {
        return positionZ;
    }

    public byte getId(){
        return id;
    }

    /*
     * getTextureCoordinates()
     * Возвращает координаты текстуры блока в текстурном атласе.
     * Этот метод, перекрытый в классах-наследниках,
     * может вернуть разные координаты для разных сторон блока.
     * Это нужно для того, чтобы можно было сделать блок
     * с разными текстурами на разных его сторонах.
     */
    public Vec2i getTextureCoordinates(int side) {
        return textureCoordinates;
    }

    public String getName() {
        return name;
    }

    public int render(Tessellator t) {
        int sides = 0;
        if (getId() != BLOCK_AIR_ID) {
            for (int i = 0; i < 6; i++) {
                if (renderSide(t, positionX, positionY, positionZ, i) > 0) {
                    sides++;
                }
            }
        }
        return sides;
    }

    /*
     * renderSide()
     * Рендерит сторону (грань) блока, если она не заблокирована другим блоком.
     */
    public int renderSide(Tessellator t, float x, float y, float z, int side) {
        int sidesRendered = 0;
        Vec2i pt = getTextureCoordinates(side);

        /*
         * u0, v0 - координаты x,y левой верхней точки текстуры текущей стороны блока в текстурном атласе.
         * u1, v1 - координаты x,y правой нижней точки текстуры текущей стороны блока в текстурном атласе.
         * Из текстурного атласа будет выделена область между этими точками
         * и наложена на текущую сторону блока в качестве ее текстуры.
         * 16 - это количество текстур в атласе по вертикали и горизонтали.
         * Текстурный атлас используется для сокращения количества переключений
         * между текстурами и, как следствие, ускорения рендеринга и повышения FPS.
         */
        float u0 = pt.x / 16.0f;
        float v0 = pt.y / 16.0f;
        float u1 = u0 + 1.0f / 16.0f;
        float v1 = v0 + 1.0f / 16.0f;
        switch (side) {
            case BLOCK_SIDE_BACK:
                if (!world.isBlockSolid((int)x, (int)y, (int)z - 1)) {
                    t.addTextureVertexQuad(u0, v1, x, y, z);
                    t.addTextureVertexQuad(u1, v1, x + world.blockWidth, y, z);
                    t.addTextureVertexQuad(u1, v0, x + world.blockWidth, y + world.blockHeight, z);
                    t.addTextureVertexQuad(u0, v0, x, y + world.blockHeight, z);
                    sidesRendered++;
                }
                break;
            case BLOCK_SIDE_FRONT:
                if (!world.isBlockSolid((int)x, (int)y, (int)z + 1)) {
                    t.addTextureVertexQuad(u0, v1, x, y, z + world.blockWidth);
                    t.addTextureVertexQuad(u1, v1, x + world.blockWidth, y, z + world.blockWidth);
                    t.addTextureVertexQuad(u1, v0, x + world.blockWidth, y + world.blockHeight, z + world.blockWidth);
                    t.addTextureVertexQuad(u0, v0, x, y + world.blockHeight, z + world.blockWidth);
                    sidesRendered++;
                }
                break;
            case BLOCK_SIDE_LEFT:
                if (!world.isBlockSolid((int)x - 1, (int)y, (int)z)) {
                    t.addTextureVertexQuad(u0, v1, x, y, z);
                    t.addTextureVertexQuad(u1, v1, x, y, z + world.blockWidth);
                    t.addTextureVertexQuad(u1, v0, x, y + world.blockHeight, z + world.blockWidth);
                    t.addTextureVertexQuad(u0, v0, x, y + world.blockHeight, z);
                    sidesRendered++;
                }
                break;
            case BLOCK_SIDE_RIGHT:
                if (!world.isBlockSolid((int)x + 1, (int)y, (int)z)) {
                    t.addTextureVertexQuad(u0, v1, x + world.blockWidth, y, z);
                    t.addTextureVertexQuad(u1, v1, x + world.blockWidth, y, z + world.blockWidth);
                    t.addTextureVertexQuad(u1, v0, x + world.blockWidth, y + world.blockHeight, z + world.blockWidth);
                    t.addTextureVertexQuad(u0, v0, x + world.blockWidth, y + world.blockHeight, z);
                    sidesRendered++;
                }
                break;
            case BLOCK_SIDE_BOTTOM:
                if (!world.isBlockSolid((int)x, (int)y - 1, (int)z)) {
                    t.addTextureVertexQuad(u0, v1, x, y, z);
                    t.addTextureVertexQuad(u1, v1, x + world.blockWidth, y, z);
                    t.addTextureVertexQuad(u1, v0, x + world.blockWidth, y, z + world.blockWidth);
                    t.addTextureVertexQuad(u0, v0, x, y, z + world.blockWidth);
                    sidesRendered++;
                }
                break;
            case BLOCK_SIDE_TOP:
                if (!world.isBlockSolid((int)x, (int)y + 1, (int)z)) {
                    t.addTextureVertexQuad(u0, v1, x, y + world.blockHeight, z);
                    t.addTextureVertexQuad(u1, v1, x + world.blockWidth, y + world.blockHeight, z);
                    t.addTextureVertexQuad(u1, v0, x + world.blockWidth, y + world.blockHeight, z + world.blockWidth);
                    t.addTextureVertexQuad(u0, v0, x, y + world.blockHeight, z + world.blockWidth);
                    sidesRendered++;
                }
                break;
        }
        return sidesRendered;
    }

}

