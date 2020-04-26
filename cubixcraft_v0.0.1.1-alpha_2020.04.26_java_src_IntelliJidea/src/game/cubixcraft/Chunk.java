/*
 * Чанк - это условный кусок мира.
 * Мир разделён на несколько чанков по [chunkSize,chunkSize,chunkSize] блоков в каждом.
 * Размеры мира должны быть кратны размерам чанка, иначе мы получим краш при генерации мира.
 * В текущей версии, по-умолчанию, значение переменной chunkSize равно 16.
 * Проще говоря, чанк это большой куб, который содержит в себе много блоков (как
 * кубик Рубика). При попадании чанка во frustum, рендерятся все блоки внутри него,
 * вне зависимости от того, попадают ли они в поле видимости (frustum) или нет.
 * Для каждого чанка создаётся отдельный лист, который перекомпилируется каждый раз
 * когда игрок разрушает какой-то блок или ставит новый.
 * Разделение мира на чанки нужно для увеличения скорости рендеринга.
 * Если рендерить каждый блок по-отдельности, то будут жуткие тормоза.
 */
package game.cubixcraft;

import static org.lwjgl.opengl.GL11.*;

public class Chunk {
    public static int chunkSize = 16;
    private WorldRenderer renderer;

    //[x0,y0,z0] - правая нижняя задняя точка чанка.
    //Эти координаты считаются позицией чанка.
    public int x0;
    public int y0;
    public int z0;
    //[x1,y1,z1] - левая верхняя передняя точка чанка
    public int x1;
    public int y1;
    public int z1;
    //По сути, чанк это большой куб, а значит [x0,y0,z0] и [x1,y1,z1] это два противоположных его угла.

    //Количество блоков в чанке, которые были отрендерены при последней компиляции листа.
    //Нужно для определения общего количества чанков и блоков, попадающих в поле зрения игрока.
    public int blocksRendered;

    /*
     * Флаг необходимости перекомпилировать лист.
     * Когда игрок ставит или ломает блок, устанавливается значение TRUE
     * и лист будет перекомпилирован в следующей итерации цикла рендера.
     */
    public boolean needUpdate;

    //собственно, сам лист
    public int glList;

    public Chunk(WorldRenderer worldRenderer, int x0, int y0, int z0, int x1, int y1, int z1){
        this.x0 = x0;
        this.y0 = y0;
        this.z0 = z0;
        this.x1 = x1;
        this.y1 = y1;
        this.z1 = z1;
        renderer = worldRenderer;
        glList = glGenLists(1);
        blocksRendered = 0;

        //При создании чанка необходимо установить этот флаг в TRUE,
        //чтобы лист автоматически скомпилировался при рендеринге.
        needUpdate = true;
    }

    /*
     * rebuild()
     * Компилирует лист чанка без проверки, необходимо ли это делать.
     * Проверка должна производиться вне метода.
     */
    private int rebuild(){
        glNewList(glList, GL_COMPILE);
        blocksRendered = 0;
        Tesselator tesselator = new Tesselator();

        //проходим по всем блокам внутри чанка
        for (int x = x0; x < x1; x++) {
            for (int y = y0; y < y1; y++) {
                for (int z = z0; z < z1; z++) {
                    int id = renderer.worldObj.getBlockId(x, y, z);

                    //если ID блока не 0 - рендерим его
                    if (id > 0) {
                        int res = 0;
                        for (int i = 0; i < 6; i++) {
                            res += renderer.renderBlockSide(tesselator,
                                    (float)x, (float)y, (float)z, i);
                        }

                        //если отрендерена хотя-бы одна грань блока,
                        //то увеличиваем количество отрендеренных блоков на единицу
                        if (res > 0) {
                            blocksRendered++;
                        }
                    }
                }
            }
        }
        tesselator.flushTextureQuads();
        glEndList();
        return blocksRendered;
    }

    /*
     * render()
     * Отрендерить чанк, предварительно скомпилировав его лист, если это необходимо.
     */
    public int render(){
        if (needUpdate) {
            rebuild();
            needUpdate = false;
        }
        glCallList(glList);
        return blocksRendered;
    }
}
