/*
 * Класс HitResult содержит информацию о том, на какой блок смотрит игрок.
 * Если идентификатор класса равен null, это значит, что игрок смотрит в пустоту
 * или находится слишком далеко от ближайшего блока.
 */
package game.cubixcraft.utils;

public class HitResult {
    public int cubeId;
    public int sideId;
    public int x;
    public int y;
    public int z;
    public int xNear;
    public int yNear;
    public int zNear;

    public HitResult(int cubeId, int sideId) {
        this.cubeId = cubeId;
        this.sideId = sideId;
    }
}
