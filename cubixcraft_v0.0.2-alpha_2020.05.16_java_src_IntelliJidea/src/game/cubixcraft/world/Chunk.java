/*
 * Чанк - это условный кусок мира.
 * Мир разделён на несколько чанков по [chunkSize,chunkSize,chunkSize] блоков в каждом.
 * Размеры мира должны быть кратны размерам чанка, иначе мы получим краш при генерации мира
 * или чанки просто не создадутся и, в результате, нечему будет рендериться.
 * В текущей версии, по-умолчанию, значение переменной chunkSize равно 16.
 * Проще говоря, чанк это большой куб, который содержит в себе много блоков (как
 * кубик Рубика). При попадании чанка во frustum, рендерятся все блоки внутри него,
 * вне зависимости от того, попадают ли они в поле видимости (frustum) или нет.
 * Для каждого чанка создаётся отдельный лист, который перекомпилируется каждый раз
 * когда игрок разрушает какой-то блок или ставит новый.
 * Разделение мира на чанки нужно для увеличения скорости рендеринга.
 * Если рендерить каждый блок по-отдельности, то будут жуткие тормоза.
 */
package game.cubixcraft.world;

import game.cubixcraft.Tessellator;
import game.cubixcraft.block.Block;

import static org.lwjgl.opengl.GL11.*;

public class Chunk {

    //размер чанка по X,Y,Z
    public static int chunkSize = 16;

    //Счётчик общего количества компиляций листов чанков с момента последнего сброса.
    //Нужно для определения количества обновлений чанков в секунду.
    public static int totalChunkUpdates = 0;

    private WorldRenderer renderer;

    //[x0,y0,z0] - координаты правого нижнего заднего блока в чанке.
    //Этот блок считается первым в чанке, а его координаты считаются позицией чанка.
    public int x0;
    public int y0;
    public int z0;
    //[x1,y1,z1] - координаты левого верхнего переднего блока в чанке.
    //Этот блок считается последним в чанке и находится в противоположном углу.
    public int x1;
    public int y1;
    public int z1;

    //Количество блоков в чанке, которые были отрендерены при последней компиляции листа.
    //Нужно для определения общего количества чанков и блоков, попадающих в поле зрения игрока.
    private int blocksRenderedCount;

    //Количество граней блоков в чанке, которые были отрендерены при последней компиляции листа.
    private int quadsRenderedCount;

    /*
     * Флаг, определяющий, нужно ли перекомпилировать лист чанка.
     * Когда игрок ставит или ломает блок, устанавливается значение TRUE и лист
     * будет перекомпилирован в следующей итерации цикла рендера.
     */
    private boolean isExpired;

    //собственно, сам лист
    private int glList;

    public Chunk(WorldRenderer worldRenderer, int x0, int y0, int z0, int x1, int y1, int z1) {
        this.x0 = x0;
        this.y0 = y0;
        this.z0 = z0;
        this.x1 = x1;
        this.y1 = y1;
        this.z1 = z1;
        renderer = worldRenderer;
        glList = glGenLists(1);
        blocksRenderedCount = quadsRenderedCount = 0;

        //При создании чанка необходимо установить этот флаг в TRUE,
        //чтобы лист автоматически скомпилировался при рендеринге.
        isExpired = true;
    }

    /*
     * rebuild()
     * Компилирует лист чанка без проверки, необходимо ли это делать.
     * Проверка должна производиться вне метода.
     * Возвращает количество отрендеренных блоков в чанке.
     */
    public int rebuild() {
        Chunk.totalChunkUpdates++;
        glNewList(glList, GL_COMPILE);
        blocksRenderedCount = quadsRenderedCount = 0;

        Tessellator tessellator = new Tessellator();

        //проходим по всем блокам внутри чанка
        for (int x = x0; x < x1; x++) {
            for (int y = y0; y < y1; y++) {
                for (int z = z0; z < z1; z++) {
                    Block b = renderer.worldObj.getBlock(x, y, z);

                    //если блок не является воздухом, тогда рендерим его
                    if (b != null && b.getId() != Block.BLOCK_AIR_ID) {

                        int sidesRendered = b.render(tessellator);

                        //Если отрендерена хотя-бы одна грань блока,
                        //то увеличиваем количество отрендеренных блоков на единицу
                        //и суммируем количество отрендеренных граней в чанке.
                        if (sidesRendered > 0) {
                            blocksRenderedCount++;
                            quadsRenderedCount += sidesRendered;
                        }
                    }
                }
            }
        }
        tessellator.flushTextureQuads();
        glEndList();
        isExpired = false;
        return blocksRenderedCount;
    }

    /*
     * render()
     * Рендерит чанк и возвращает количество отрендеренных блоков.
     */
    public int render() {
        glCallList(glList);
        return blocksRenderedCount;
    }

    public int getQuadsRenderedCount() {
        return quadsRenderedCount;
    }

    public boolean getIsExpired() {
        return isExpired;
    }

    /*
     * expireThisChunk()
     * Удаляет лист чанка и создаёт его заново.
     * Новый лист создаётся пустым.
     */
    public int expireThisChunk() {
        if (glList > 0) {
            glDeleteLists(glList, 1);
        }
        glList = glGenLists(1);
        isExpired = true;
        return glList;
    }

    /*
     * destroyList()
     * Удаляет лист и освобождает память.
     */
    public void destroyList() {
        glDeleteLists(glList, 1);
    }
}
