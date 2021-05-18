/*
 * Интерфейс IWorldListener позволяет уведомлять другие классы о происходящих в мире событиях.
 */
package game.cubixcraft.world;

import game.cubixcraft.block.Block;

public interface IWorldListener {

    /*
     * blockPlaced()
     * Уведомление об изменении блока в мире.
     * Приходит, когда игрок ставит новый блок.
     */
    void blockPlaced(int x, int y, int z);

    /*
     * blockDestroyed()
     * Уведомление об удалении блока из мира.
     * Приходит, когда игрок ломает блок.
     */
    void blockDestroyed(int x, int y, int z);

    /*
     * blockChanged()
     * Уведомление об изменении блока в мире.
     * Приходит, когда в мире меняется блок,
     * если игрок его не построил и не разрушил.
     * Например, это происходит, если поставить
     * какой-нибудь блок на блок травы.
     * Тогда блок травы меняется на блок земли.
     */
    void blockChanged(Block oldBlock);

    void generationFinished();
}