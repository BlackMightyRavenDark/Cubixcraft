package game.cubixcraft.world;

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

    //размер мира в блоках по осям X,Y,Z
    public int sizeX;
    public int sizeY; //она же - depth.
    public int sizeZ;

    //массив, содержащий ID всех блоков
    private byte[] blockIds;

    //Максимальное количество блоков в мире.
    //Это число определяет размер массива.
    public int blocksLength;

    //название мира
    private final String name;

    private final WorldRenderer renderer;
    
    //слушатели событий
    private final ArrayList<IWorldListener> listeners;

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

        renderer = new WorldRenderer(this);
    }

    /*
     * generateMap()
     * Генерирует игровой мир.
     */
    public void generateMap() {
        blocksLength = sizeX * sizeY * sizeZ;
        System.out.println("Generating map " + sizeX + "x" + sizeY + "x" + sizeZ + ", " + blocksLength + " blocks total.");

        blockIds = new byte[blocksLength];

        //Послойная генерация мира, начиная снизу
        for (int y = 0; y < sizeY; y++) {
            for (int x = 0; x < sizeX; x++) {
                for (int z = 0; z < sizeZ; z++) {
                    byte b;
                    if (y < 16) {
                        b = BLOCK_DIRT_ID;
                    } else if (y == 16) {
                        b = BLOCK_GRASS_ID;
                    }
                    else {
                        b = BLOCK_AIR_ID;
                    }
                    setBlockWithoutNotify(x, y, z, b);
                }
            }
        }

        //генерация столбов из земли в каждом чанке
        for (int x = 0; x < sizeX; x += Chunk.chunkSize) {
            for (int z = 0; z < sizeZ; z += Chunk.chunkSize) {
                setBlockWithoutNotify(x, sizeY - 1, z, BLOCK_GRASS_ID);
                for (int y = sizeY - 2; y >= 16; y--) {
                    setBlockWithoutNotify(x, y, z, BLOCK_DIRT_ID);
                }
            }
        }

        //уведомляем слушателей о том, что мир сгенерирован.
        for (IWorldListener listener : listeners) {
            listener.generationFinished();
        }
    }

    /*
     * getArrayId()
     * Переводит трёхмерные координаты мира в формат одномерного массива.
     * Возвращает ID ячейки массива, соответствующей переданным координатам.
     */
    public int getArrayId(int x, int y, int z){
        return (sizeX * sizeZ * y) + (sizeZ * x) + z;
    }

    public Block createBlock(byte blockId) {
        switch (blockId) {
            case BLOCK_AIR_ID:
                return new BlockAir(this);
            case BLOCK_GRASS_ID:
                return new BlockGrass(this, new Vec2i(0,0));
            case BLOCK_DIRT_ID:
                return new BlockDirt(this, new Vec2i(2, 0));
            default:
                return null;
        }
    }

    /*
     * setBlock()
     * Заменяет блок на позиции [x,y,z], генерирует событие
     * и посылает уведомление в рендерер.
     */
    public void setBlock(int x, int y, int z, byte blockId) {
        if (setBlockWithoutNotify(x, y, z, blockId)) {

            //если новый блок не является блоком воздуха,
            //то есть если блок был именно поставлен, а не разрушен,
            //то оповещаем об этом всех слушателей.
            if (blockId != BLOCK_AIR_ID) {
                for (IWorldListener listener : listeners) {
                    listener.blockPlaced(x, y, z);
                }
            }

            //уведомляем рендерер об изменении блока
            renderer.blockChanged(x, y, z);
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
     * А так же, используется при генерации игрового мира.
     */
    public boolean setBlockWithoutNotify(int x, int y, int z, byte blockId) {
        //если [x,y,z] находятся за пределами мира - выводим сообщение и выходим.
        if (!isBlockInWorld(x, y, z)) {
            System.out.println("The block [" + x + "," + y + "," + z + "] is outside of the world!");
            return false;
        }
        int id = getArrayId(x, y, z);
        if (id >= blockIds.length) {
            System.out.println("Out of bounds [" + x + "," + y + "," + z + "]");
            return false;
        }
        blockIds[id] = blockId;
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
        Block b = createBlock(blockIds[id]);
        b.setPosition(x, y, z);
        return b;
    }

    /*
     * getBlock()
     * Возвращает экземпляр блока, находящегося в указанной ячейке массива.
     */
    public Block getBlock(int arrayIndex) {
        if (arrayIndex >= 0 && arrayIndex < blockIds.length) {
            int plane = sizeX * sizeZ;
            int y = arrayIndex / plane;
            int n = arrayIndex - plane * y;
            int x = n / sizeZ;
            int z = n - sizeZ * x;

            Block b = createBlock(blockIds[arrayIndex]);
            b.setPosition(x, y, z);
            return b;
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
        if (getBlockId(x, y, z) != BLOCK_AIR_ID) {
            //оповещаем всех слушателей об удалении блока
            for (IWorldListener listener : listeners) {
                listener.blockDestroyed(x, y, z);
            }

            setBlockWithoutNotify(x, y, z, BLOCK_AIR_ID);

            //уведомляем рендерер об изменении блока
            renderer.blockChanged(x, y, z);
        }
    }

    /*
     * isBlockInWorld()
     * Проверяет, находятся ли указанные координаты в пределах мира.
     */
    public boolean isBlockInWorld(int x, int y, int z) {
        return x >= 0 && y >= 0 && z >= 0 && x < sizeX && y < sizeY && z < sizeZ;
    }

    /*
     * getBlocksInBox()
     * Возвращает массив блоков, находящихся в пределах области box.
     */
    public ArrayList<Block> getBlocksInBox(CollisionBox box) {
        ArrayList<Block> blockList = new ArrayList<Block>();
        int x0 = (int)box.x0;
        int y0 = (int)box.y0;
        int z0 = (int)box.z0;
        int x1 = (int)box.x1;
        int y1 = (int)box.y1;
        int z1 = (int)box.z1;

        for (int x = x0; x <= x1; x++) {
            for (int y = y0; y <= y1; y++) {
                for (int z = z0; z <= z1; z++) {
                    byte blockId = getBlockId(x, y, z);
                    if (blockId != BLOCK_AIR_ID) {
                        Block block = createBlock(blockId);
                        block.setPosition(x, y, z);
                        blockList.add(block);
                    }
                }
            }
        }
        return blockList;
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
     * Первые три значения: [x,z,y] - числа в формате short (двухбайтовое целое число).
     * Всё остальное - массив типа byte (однобайтовое целое число).
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

            String fileName = pth + File.separator + name + ".dat";
            DataOutputStream dos = new DataOutputStream(new FileOutputStream(fileName));

            /*
             * Для совместимости с windows-версиями игры,
             * первые три short'а необходимо записать в формате Little Endian
             */
            ByteBuffer bb = ByteBuffer.allocate(6);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            bb.putShort((short)sizeX);
            bb.putShort((short)sizeZ);
            bb.putShort((short)sizeY);

            dos.write(bb.array());

            int offset = 0;
            int chunkArea = Chunk.chunkSize * Chunk.chunkSize * Chunk.chunkSize;
            for (int i = 0; i < renderer.chunksCount; i++) {
                dos.write(blockIds, offset, chunkArea);
                offset += chunkArea;
            }
            dos.close();
        } catch (IOException e) {
            System.out.println("Can't save the world! Sorry :'(");
            e.printStackTrace();
        }
    }

    public boolean load() {
        System.out.println("Loading world " + name + "...");

        String fileName = System.getProperty("user.dir") + File.separator + "saves" + File.separator + name + ".dat";
        File f = new File(fileName);
        if (!f.exists()) {
            System.out.println("No saved world found!");
            return false;
        }
        try (DataInputStream dis = new DataInputStream(new FileInputStream(f))) {
            ByteBuffer byteBuffer = ByteBuffer.allocate(6);
            byteBuffer.order(ByteOrder.BIG_ENDIAN);
            for (int i = 0; i < 3; i++) {
                byteBuffer.putShort(dis.readShort());
            }
            //Для совместимости с windows-версиями игры,
            //читаем первые три short'a как Little Endian.
            int x = (byteBuffer.get(0) & 0xFF) | ((byteBuffer.get(1) & 0xFF) << 8);
            int z = (byteBuffer.get(2) & 0xFF) | ((byteBuffer.get(3) & 0xFF) << 8);
            int y = (byteBuffer.get(4) & 0xFF) | ((byteBuffer.get(5) & 0xFF) << 8);

            if (x < Chunk.chunkSize || y < Chunk.chunkSize || z < Chunk.chunkSize ||
                    x % Chunk.chunkSize != 0 || y % Chunk.chunkSize != 0 || z % Chunk.chunkSize != 0) {
                System.out.println("Load failed! Unable to load saved world! Incorrect world size!");
                return false;
            }
            blocksLength = x * y * z;
            sizeX = x;
            sizeY = y;
            sizeZ = z;

            blockIds = new byte[blocksLength];
            if (dis.read(blockIds) != blocksLength) {
                System.out.println("Load failed!");
                return false;
            }
        } catch (IOException ex) {
            System.out.println("Load failed!");
            ex.printStackTrace();
            return false;
        }

        //если загрузка прошла успешно,
        //то уведомляем об этом всех слушателей.
        for (IWorldListener listener : listeners) {
            listener.generationFinished();
        }

        return true;
    }

    public void addListener(IWorldListener worldListener) {
        listeners.add(worldListener);
    }

    public ArrayList<IWorldListener> getListeners() {
        return listeners;
    }

    public WorldRenderer getRenderer() {
        return renderer;
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

    @Override
    public void generationFinished() {
        renderer.createChunks();
    }

}
