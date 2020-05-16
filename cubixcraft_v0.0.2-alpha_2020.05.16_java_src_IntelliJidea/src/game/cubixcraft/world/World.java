package game.cubixcraft.world;

import game.cubixcraft.CubixCraft;
import game.cubixcraft.CollisionBox;
import game.cubixcraft.block.*;
import game.cubixcraft.utils.Vec2i;

import static game.cubixcraft.block.Block.*;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

public class World implements IWorldListener {

    public float cubeSize;
    public float blockWidth;
    public float blockHeight;

    //размер мира в блоках по оси X
    public int sizeX;
    //размер мира в блоках по оси Z
    public int sizeY;
    //размер мира в блоках по оси Y.
    //Она же - depth. По этой оси игрок прыгает.
    public int sizeZ;

    //массив, содержащий ID всех блоков
    private byte[] blockIds;

    //массив блоков
    private Block[] blocks;

    //Максимальное количество блоков в мире.
    //Это число определяет размеры массивов.
    public int blocksLength;

    //название мира
    private String name;

    //слушатели событий
    private ArrayList<IWorldListener> listeners;

    public World(int xSize, int ySize, int zSize, String name) {
        /*
         * Мир будет сохраняться в файл <user.dir>/saves/<name>.dat
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

        sizeX = xSize;
        sizeY = ySize;
        sizeZ = zSize;

        blocksLength = sizeX * sizeY * sizeZ;

        blockIds = null;

        listeners = new ArrayList<>();
        listeners.add(this);
    }

    /*
     * generateMap()
     * Генерирует игровой мир.
     */
    public void generateMap() {
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

        //По-этому здесь sizeY и sizeZ поменяны местами.
        //Послойная генерация мира, начиная снизу
        for (int y = 0; y < sizeZ; y++) {
            for (int x = 0; x < sizeX; x++) {
                for (int z = 0; z < sizeY; z++) {
                    Block b;
                    if (y < 16) {
                        b = createBlock(BLOCK_DIRT_ID);
                    } else
                    if (y == 16) {
                        b = createBlock(BLOCK_GRASS_ID);
                    }
                    else
                    {
                        b = new BlockAir(this);
                    }
                    setBlockWithoutNotify(x, y, z, b);
                }
            }
        }

        //генерация столбов из земли в каждом чанке
        for (int x = 0; x < sizeX; x += Chunk.chunkSize) {
            for (int z = 0; z < sizeY; z += Chunk.chunkSize) {
                for (int y = sizeZ - 1; y >= 16; y--) {
                    setBlockWithoutNotify(x, y, z,
                            y == sizeZ - 1 ? createBlock(BLOCK_GRASS_ID) : createBlock(BLOCK_DIRT_ID));
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

    public Block createBlock(byte id) {
        Block block = null;
        switch (id) {
            case BLOCK_AIR_ID:
                block = new BlockAir(this);
                break;
            case BLOCK_GRASS_ID:
                block = new BlockGrass(this, new Vec2i(0,0));
                break;
            case BLOCK_DIRT_ID:
                block = new BlockDirt(this, new Vec2i(2, 0));
                break;
        }
        return block;
    }

    /*
     * setBlock()
     * Заменяет блок на позиции [x,y,z], генерирует событие
     * и посылает уведомление в рендерер.
     */
    public void setBlock(int x, int y, int z, Block block) {
        if (setBlockWithoutNotify(x, y, z, block)) {

            //если новый блок не является блоком воздуха,
            //то есть если блок был именно поставлен, а не разрушен,
            //то оповещаем об этом всех слушателей.
            if (block.getId() != BLOCK_AIR_ID) {
                for (int i = 0; i < listeners.size(); i++) {
                    listeners.get(i).blockPlaced(x, y, z);
                }
            }

            //уведомляем рендерер об изменении блока
            CubixCraft.getCubixcraft().getWorldRenderer().blockChanged(x, y, z);
        }
    }

    /*
     * setBlockWithoutNotify()
     * Заменяет блок на позиции [x,y,z] без генерации события
     * и без отправки уведомления в рендерер.
     * Уведомление нужно отправлять вручную после вызова данного метода,
     * иначе визуально изменения блока не будет видно.
     * Этот метод используется, когда блоки в мире меняются
     * без вмешательства игрока (когда игрок их не ломает и не строит).
     */
    public boolean setBlockWithoutNotify(int x, int y, int z, Block block) {
        //если [x,y,z] находятся за пределами мира - выводим сообщение и выходим
        if (!isBlockInWorld(x, y, z)) {
            System.out.println("The block [" + x + "," + y + "," + z + "] is outside of the world!");
            return false;
        }
        block.setPosition(x, y, z);
        int id = getArrayId(x, y, z);
        blockIds[id] = block.getId();
        blocks[id] = block;
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
     * getBlock()
     * Возвращает экземпляр блока, находящегося в указанной ячейке массива.
     */
    public Block getBlock(int arrayIndex) {
        if (arrayIndex >= 0 && arrayIndex < blocks.length) {
            return blocks[arrayIndex];
        }
        return null;
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
     * destroyBlock()
     * Удаляет блок на позиции [x,y,z], генерирует событие
     * и посылает уведомление в рендерер.
     */
    public void destroyBlock(int x, int y, int z) {
        if (isBlockInWorld(x, y, z)) {
            //оповещаем всех слушателей об удалении блока
            for (int i = 0; i < listeners.size(); i++) {
                listeners.get(i).blockDestroyed(x, y, z);
            }

            setBlockWithoutNotify(x, y, z, createBlock(BLOCK_AIR_ID));

            //уведомляем рендерер об изменении блока
            CubixCraft.getCubixcraft().getWorldRenderer().blockChanged(x, y, z);
        }
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
    public ArrayList<Block> getBlocksInBox(CollisionBox box) {
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
    public boolean isBlockSolid(int x, int y, int z) {
        return getBlockId(x, y, z) != BLOCK_AIR_ID;
    }

    public String getName() {
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
    public void save() {
        System.out.println("Saving the world...");
        try {
            String pth = System.getProperty("user.dir") + File.separator + "saves";
            File p = new File(pth);
            if (!p.exists()) {
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
                        Block b = createBlock(blockIds[id]);
                        setBlockWithoutNotify(xx, yy, zz, b);
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

    public void addListener(IWorldListener worldListener) {
        listeners.add(worldListener);
    }

    public ArrayList<IWorldListener> getListeners() {
        return listeners;
    }

    @Override
    public void blockPlaced(int x, int y, int z) {
        System.out.println("Placed a " + getBlock(x, y, z).getName() +
                " block to [" + x + "," + y + "," + z + "]");
    }

    @Override
    public void blockDestroyed(int x, int y, int z) {
        System.out.println("Wrecked a " + getBlock(x, y, z).getName() +
                " block at [" + x + "," + y + "," + z + "]");
    }

    @Override
    public void blockChanged(Block oldBlock) {
        int x = oldBlock.getPositionX();
        int y = oldBlock.getPositionY();
        int z = oldBlock.getPositionZ();
        System.out.println("Changed a block at [" + x + "," + y + "," + z + "] from " +
                oldBlock.getName() + " to " + getBlock(x, y, z).getName());
    }
}