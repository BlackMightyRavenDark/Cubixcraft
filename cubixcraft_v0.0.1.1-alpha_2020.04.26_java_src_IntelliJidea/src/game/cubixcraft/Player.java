/*
 * Класс Player управляет игровой камерой и содержит переменные и константы, относящиеся к игроку.
 * Коэффициенты нужны для того чтобы значения по-умолчанию можно было приравнять к единице.
 */

package game.cubixcraft;

import org.lwjgl.input.Keyboard;
import org.lwjgl.input.Mouse;
import org.lwjgl.util.glu.GLU;

import java.util.ArrayList;

import static org.lwjgl.opengl.GL11.*;

public class Player {
    public float positionX;
    public float positionY;
    public float positionZ;
    public float rotationYaw;
    public float rotationPitch;
    public float rotationRoll;

    public final float height = 1.62f;

    private final float mouseSensitivity = 1.0f;

    //скорость перемещения игрока пешком
    private float walkingSpeed;
    private final float speedCoef = 2.0f;

    //вектор движения игрока
    private float motionX;
    private float motionY;
    private float motionZ;

    //Трение игрока при движении по поверхности.
    //Вектор движения по осям X,Z умножается на эти коэффициенты.
    private final float frictionCoefX = 0.0f;
    private final float frictionCoefZ = 0.0f;

    //то как игрока тянет вниз
    private float gravityY;
    private final float gravityCoef = 18.0f;

    //высота прыжка
    private float jumpHeight;
    private final float jumpCoef = 7.7f;

    //Коэффициент движения
    private final float motionCoef = 7.0f;

    //смещение камеры относительно позиции игрока
    private float cameraOffsetX;
    private float cameraOffsetZ;

    //игрок стоит на поверхности или нет
    private boolean onGround;

    public World worldObj;

    public CollisionBox collisionBox;

    public Player(World world, float positionX, float positionY, float positionZ,
           float rotationYaw, float rotationPitch, float rotationRoll){
        this.positionX     = positionX;
        this.positionY     = positionY;
        this.positionZ     = positionZ;
        this.rotationYaw   = rotationYaw;
        this.rotationPitch = rotationPitch;
        this.rotationRoll  = rotationRoll;

        walkingSpeed = 1.0f;

        gravityY = 1.0f;

        jumpHeight = 1.0f;

        onGround = false;

        setMotionVector(0.0f, 3.0f, 0.0f);

        worldObj = world;

        //область столкновения это по сути длинна, ширина и рост игрока.
        collisionBox = new CollisionBox(positionX, positionY, positionZ,
                positionX + 0.7f, positionY + height, positionZ + 0.7f);

        /*
         * Смещение позиции камеры к центру области столкновения игрока.
         * Иначе, камера была бы слишком близко или далеко от объекта столкновения,
         * в зависимости от того, в какую сторону смотрит игрок.
         */
        cameraOffsetX = (collisionBox.x1 - collisionBox.x0) / 2.0f;
        cameraOffsetZ = (collisionBox.z1 - collisionBox.z0) / 2.0f;
    }

    /*
     * setPosition()
     * Перемещает игрока на указанную позицию.
     */
    public void setPosition(float xPos, float yPos, float zPos){
        positionX = xPos;
        positionY = yPos;
        positionZ = zPos;
        collisionBox.moveAbsolute(positionX, positionY, positionZ);
    }

    /*
     * setRotation()
     * Разворачивает игрока.
     */
    public void setRotation(float xRot, float yRot, float zRot){
        rotationYaw   = xRot;
        rotationPitch = yRot;
        rotationRoll  = zRot;
    }

    public float getCameraOffsetX(){
        return cameraOffsetX;
    }

    public float getCameraOffsetZ(){
        return cameraOffsetZ;
    }

    /*
     * reset()
     * Перемещает игрока на позицию по-умолчанию (respawn).
     */
    public void reset() {
        setPosition(0.0f, worldObj.sizeZ + 1.0f, 0.0f);
        setRotation(135.0f, 0.0f, 0.0f);
        setMotionVector(0.0f, 3.0f, 0.0f);
    }

    /*
     * setMotionVector()
     * Устанавливает текущий вектор движения игрока.
     */
    public void setMotionVector(float xMotion, float yMotion, float zMotion){
        motionX = xMotion * motionCoef;
        motionY = yMotion * motionCoef;
        motionZ = zMotion * motionCoef;
    }

    /*
     * setupCamera()
     * Перемещает камеру относительно позиции игрока.
     */
    public void setupCamera() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        GLU.gluPerspective(75.0f,
                (float)CubixCraft.windowWidth / (float)CubixCraft.windowHeight, 0.03f, 1000.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glRotatef(rotationRoll,  0.0f, 0.0f, 1.0f);
        glRotatef(rotationPitch, 1.0f, 0.0f, 0.0f);
        glRotatef(rotationYaw,   0.0f, 1.0f, 0.0f);

        glTranslatef(-positionX - cameraOffsetX, -positionY - height, -positionZ - cameraOffsetZ);
    }

    /*
     * handleMovement()
     * Обработка клавиш, отвечающих за перемещение игрока.
     */
    private void handleMovement(float speed){
        int xAxis = 0;
        int yAxis = 0;
        int zAxis = 0;
        if (Keyboard.isKeyDown(Keyboard.KEY_W) || Keyboard.isKeyDown(Keyboard.KEY_UP))    zAxis--;
        if (Keyboard.isKeyDown(Keyboard.KEY_S) || Keyboard.isKeyDown(Keyboard.KEY_DOWN))  zAxis++;
        if (Keyboard.isKeyDown(Keyboard.KEY_A) || Keyboard.isKeyDown(Keyboard.KEY_LEFT))  xAxis--;
        if (Keyboard.isKeyDown(Keyboard.KEY_D) || Keyboard.isKeyDown(Keyboard.KEY_RIGHT)) xAxis++;
        if (Keyboard.isKeyDown(Keyboard.KEY_SPACE)) yAxis++;

        if (xAxis != 0 || yAxis != 0 || zAxis != 0) {
            moveRelative((float)xAxis, (float)yAxis, (float)zAxis, speed);
        }
    }

    /*
     * handleMouseMovement()
     * Разворачивает игрока в соответствии с движением мыши.
     */
    private void handleMouseMovement(float speed){
        rotationYaw   += Mouse.getDX() * speed * 0.1f;
        rotationPitch -= Mouse.getDY() * speed * 0.1f;
        if (rotationPitch < -90.0f) {
            rotationPitch = -90.0f;
        }
        else
            if (rotationPitch > 90.0f) {
            rotationPitch = 90.0f;
        }
        while (rotationYaw >= 360.0f){
            rotationYaw -= 360.0f;
        }
        while (rotationYaw <= -360f){
            rotationYaw += 360.0f;
        }
    }

    /*
     * moveRelative()
     * Вычисляет текущий вектор движения игрока.
     * Этот вектор каждую итерацию умножается на коэффициент трения.
     * Таким образом, если значение трения меньше единицы, то игрок постепенно останавливается.
     */
    private void moveRelative(float xOffset, float yOffset, float zOffset, float speed){
        if (yOffset > 0.0f && onGround) {
            onGround = false;
            motionY = jumpHeight * jumpCoef;
        }
        float dist = (float)(Math.pow(xOffset, 2.0) + Math.pow(zOffset, 2.0));
        if (dist < 0.01f) {
            return;
        }
        dist = speed / (float)Math.sqrt(dist);
        xOffset *= dist;
        zOffset *= dist;
        float a = (float)Math.sin(rotationYaw * Math.PI / 180.0);
        float b = (float)Math.cos(rotationYaw * Math.PI / 180.0);
        motionX += xOffset * b - zOffset * a;
        motionZ += zOffset * b + xOffset * a;
    }

    /*
     * move()
     * Перемещает игрока на указанное расстояние относительно текущей позиции.
     */
    public void move(float xAxis, float yAxis, float zAxis){
        float xAxisOld = xAxis;
        float yAxisOld = yAxis;
        float zAxisOld = zAxis;

        //получить массив блоков вокруг игрока
        CollisionBox box = collisionBox.grow(1.0f, height <= 1.0f ? 2.0f : height, 1.0f);
        ArrayList<Block> nearBlocks = worldObj.getBlocksInBox(box);

        //проверить, столкнулся ли игрок с блоками из массива
        for (int i = 0; i < nearBlocks.size(); i++) {
            if (nearBlocks.get(i).getId() > 0) {
                xAxis = nearBlocks.get(i).collisionBox.clipCollideX(collisionBox, xAxis);
                yAxis = nearBlocks.get(i).collisionBox.clipCollideY(collisionBox, yAxis);
                zAxis = nearBlocks.get(i).collisionBox.clipCollideZ(collisionBox, zAxis);
            }
        }

        //передвинуть область столкновения игрока
        collisionBox.moveRelative(xAxis, yAxis, zAxis);

        if (xAxisOld != xAxis) {
            motionX = 0.0f;
        }
        if (yAxisOld != yAxis) {
            motionY = 0.0f;
        }
        if (zAxisOld != zAxis) {
            motionZ = 0.0f;
        }
        //предотвращение падения игрока в бесконечное небытиё
        if (collisionBox.y0 < 0.0f) {
            collisionBox.moveAbsolute(collisionBox.x0, 0.0f, collisionBox.z0);
            onGround = true;
            motionY = 0.0f;
        }
        else {
            onGround = ((yAxisOld != yAxis && yAxisOld < 0.0f) || collisionBox.y0 <= 0.0f);
        }
        positionX = collisionBox.x0;
        positionY = collisionBox.y0;
        positionZ = collisionBox.z0;
    }

    /*
     * Tick()
     * Жизненный цикл игрока.
     * В текущей версии здесь обрабатывается только перемещение игрока
     * и проверка его столкновений с блоками.
     */
    public void tick(){

        if (Mouse.isGrabbed()) {
            handleMouseMovement(mouseSensitivity);
        }

        handleMovement(walkingSpeed * speedCoef * (float)CubixCraft.gameTimer.deltaTime);

        //симуляция гравитации
        motionY -= gravityY * gravityCoef * (float)CubixCraft.gameTimer.deltaTime;

        move(motionX, motionY * (float)CubixCraft.gameTimer.deltaTime, motionZ);

        //чем больше коэффициенты трения, тем сильнее игрок скользит
        motionX *= frictionCoefX;
        motionZ *= frictionCoefZ;
    }

    public String printPosition() {
        return String.format("%.2f", positionX) +
               " " + String.format("%.2f", positionY) +
               " " + String.format("%.2f", positionZ);
    }

    public String printRotation() {
        return String.format("%.2f", rotationYaw) + " " + String.format("%.2f", rotationPitch);
    }
}
