package game.cubixcraft;

import static game.cubixcraft.Textures.*;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

public class World {
    private String name;

    public float cubeSize;
    public float blockWidth;
    public float blockHeight;

    public int sizeX;
    public int sizeY;
    public int sizeZ;

    public byte blockIds[];

    public Block blocks[];
    public int blocksLength;

    public World(int xSize, int ySize, int zSize, String name){
        /*
         * Мир будет сохраняться в файл <current_dir>/saves/<name>.dat
         * По-этому не стоит подсовывать в эту переменную системные символы!
         * А так же, название мира выводится на экран.
         */
        this.name = name;

        /*
         * Эти переменные определяют размер блоков. Заготовка для следующих версий.
         * В текущей версии должны быть равны единице!
         */
        cubeSize    = 1.0f;
        blockWidth  = 1.0f;
        blockHeight = 1.0f;

        //размер мира в блоках по оси X
        sizeX = xSize;
        //размер мира в блоках по оси Z
        sizeY = ySize;
        //размер мира в блоках по оси Y.
        //Она же - depth. По этой оси игрок прыгает.
        sizeZ = zSize;
        blocksLength = sizeX * sizeY * sizeZ;
        blockIds = null;
    }

    /*
     * generateMap()
     * Генерирует игровой мир.
     */
    public void generateMap(){
        /*
         * Не забываем, что оси Y и Z перепутаны.
         * В генераторе мира считается, что X и Y это
         * ширина и длинна. Z это высота (или depth) мира.
         * А в остальных методах и при работе с камерой - наоборот.
         * Оси [X,Z] соответствуют перемещению по плоскости [X,Y] мира,
         * а Y перемещает игрока (камеру) вверх/вниз относительно мира.
         * То есть, Y это высота (или depth).
         */
        System.out.print("Generating map " + sizeX + "x" + sizeZ + "x" + sizeY);
        blocksLength = sizeX * sizeY * sizeZ;
        System.out.println(", " + blocksLength + " blocks total.");
        blockIds = new byte[blocksLength];
        blocks = new Block[blocksLength];

        //по-этому здесь sizeY и sizeZ поменяны местами.
        //послойная генерация мира, начиная снизу
        for (int y = 0; y < sizeZ; y++) {
            for (int x = 0; x < sizeX; x++) {
                for (int z = 0; z < sizeY; z++) {
                    setBlock(x, y, z, y < 16 ? (byte)1 : (byte)0);
                }
            }
        }
    }

    /*
     * getArrayId()
     * Переводит трёхмерные координаты мира в формат одномерного массива.
     * Возвращает ID ячейки массива, соответствующей переданным координатам.
     */
    public int getArrayId(int x, int y, int z){
        return (x * sizeY) + (sizeX * sizeY * y) + z;
    }

    /*
     * setBlock()
     * Заменяет блок на позиции [x,y,z].
     * В текущей версии существует только один блок - трава.
     * По-этому, передаём 1 если блок есть или 0, если блока нет.
     */
    public boolean setBlock(int x, int y, int z, byte blockId){
        //если [x,y,z] находятся за пределами мира - выводим сообщение и выходим
        if (!isBlockInWorld(x, y, z)) {
            System.out.println("The block [" + x + "," + y + "," + z + "] is outside of the world!");
            return false;
        }
        int id = getArrayId(x, y, z);
        blockIds[id] = blockId;
        blocks[id] = new Block(this, blockIds[id], textureGrass, x, y, z);
        return true;
    }

    /*
     * getBlock()
     * Возвращает экземпляр блока, находящегося в указанных координатах.
     */
    public Block getBlock(int x, int y, int z) {
        if (!isBlockInWorld(x, y, z)) {
            return null;
        }
        int id = getArrayId(x, y, z);
        return blocks[id];
    }

    /*
     * getBlockId()
     * Возвращает ID блока, находящегося в указанных координатах.
     */
    public byte getBlockId(int x, int y, int z) {
        if (!isBlockInWorld(x, y, z)) {
            return 0;
        }
        int id = getArrayId(x, y, z);
        return blockIds[id];
    }

    /*
     * isBlockInWorld()
     * Проверяет, находятся ли указанные координаты в пределах мира.
     */
    public boolean isBlockInWorld(int x, int y, int z) {
        return x >= 0 && y >= 0 && z >= 0 && x < sizeX && y < sizeZ && z < sizeY;
    }

    /*
     * getBlocksInBox()
     * Возвращает массив блоков, находящихся в пределах области box.
     */
    public ArrayList<Block> getBlocksInBox(CollisionBox box){
        ArrayList<Block> a = new ArrayList<Block>();
        int x0 = (int)box.x0;
        int y0 = (int)box.y0;
        int z0 = (int)box.z0;
        int x1 = (int)box.x1;
        int y1 = (int)box.y1;
        int z1 = (int)box.z1;
        for (int x = x0; x <= x1; x++) {
            for (int y = y0; y <= y1; y++) {
                for (int z = z0; z <= z1; z++) {
                    Block b = getBlock(x, y, z);
                    if (b != null && b.getId() > 0) {
                        a.add(b);
                    }
                }
            }
        }
        return a;
    }

    /*
     * isBlockSolid()
     * Проверяет, является ли блок твёрдым. Через твёрдый блок невозможно пройти.
     * В текущей версии не существует проходимых блоков.
     * По-этому достаточно проверять только ID блока.
     */
    public boolean isBlockSolid(int x, int y, int z){
        return getBlockId(x, y, z) > 0;
    }

    public String getName(){
        return name;
    }

    /*
     * Save()
     * Сохранение игрового мира.
     * Первые три значения: [x,z,y] - числа в формате short (двухбайтовое целое беззнаковое число).
     * x - размер мира (в блоках) по оси X
     * z - размер мира (в блоках) по оси Y
     * y - размер мира (в блоках) по оси Z (она же depth - ось прыжка)
     * Всё остальное - массив типа byte (однобайтовое целое беззнаковое число).
     * Этот массив содержит ID всех блоков мира. Размер массива определяется как blocksLen = x * y * z
     */
    public void save(){
        System.out.println("Saving the world...");
        try {
            String pth = System.getProperty("user.dir") + File.separator + "saves";
            File p = new File(pth);
            if (!p.exists()){
                p.mkdirs();
            }

            DataOutputStream dos = new DataOutputStream(new FileOutputStream(
                    new File(pth + File.separator + name + ".dat")));

            /*
             * Для совместимости с windows-версиями игры,
             * первые три short'а необходимо записать в формате Little Endian
             */
            ByteBuffer bb = ByteBuffer.allocate(6);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            bb.putShort((short)sizeX);
            bb.putShort((short)sizeY);
            bb.putShort((short)sizeZ);

            dos.write(bb.array());
            dos.write(blockIds);
            dos.close();
        } catch (IOException e) {
            System.out.println("Can't save the world! Sorry :'(");
            e.printStackTrace();
        }
    }

    public boolean load() {
        System.out.println("Loading world " + name + "...");
        String fn = System.getProperty("user.dir") + File.separator + "saves" + File.separator + name + ".dat";
        File f = new File(fn);
        if (!f.exists()) {
            return false;
        }
        try {
            DataInputStream dis = new DataInputStream(new FileInputStream(f));
            ByteBuffer byteBuffer = ByteBuffer.allocate(6);
            byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
            for (int i = 0; i < 3; i++) {
                byteBuffer.putShort(dis.readShort());
            }
            int x = (byteBuffer.get(0) << 8) + byteBuffer.get(1);
            int y = (byteBuffer.get(2) << 8) + byteBuffer.get(3);
            int z = (byteBuffer.get(4) << 8) + byteBuffer.get(5);
            blocksLength = x * y * z;
            if (blocksLength <= 0){
                dis.close();
                return false;
            }
            sizeX = x;
            sizeY = y;
            sizeZ = z;
            blockIds = new byte[blocksLength];
            dis.read(blockIds);
            dis.close();
            blocks = new Block[blocksLength];
            int id = 0;
            for (int yy = 0; yy < sizeZ; yy++){
                for (int xx = 0; xx < sizeX; xx++){
                    for (int zz = 0; zz < sizeY; zz++){
                        Block b = new Block(this, blockIds[id], textureGrass, xx,yy,zz);
                        blocks[id] = b;
                        id++;
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }
}