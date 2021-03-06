/*
 * Frustum - это та часть мира, которая попадает в поле зрения игрока.
 * Более подробно описано здесь:
 * https://masandilov.ru/opengl/opengl-frustum
 */
package game.cubixcraft;

import java.nio.*;
import org.lwjgl.*;

import static org.lwjgl.opengl.GL11.*;

public class Frustum {
    public float[][] m_Frustum;
    public static final int FRUSTUM_SIDE_RIGHT  = 0;
    public static final int FRUSTUM_SIDE_LEFT   = 1;
    public static final int FRUSTUM_SIDE_BOTTOM = 2;
    public static final int FRUSTUM_SIDE_TOP    = 3;
    public static final int FRUSTUM_SIDE_BACK   = 4;
    public static final int FRUSTUM_SIDE_FRONT  = 5;
    public static final int PLANE_A = 0;
    public static final int PLANE_B = 1;
    public static final int PLANE_C = 2;
    public static final int PLANE_D = 3;
    private static Frustum frustumObj;
    private FloatBuffer bufferMatrixProjection;
    private FloatBuffer bufferMatrixModel;

    static {
        Frustum.frustumObj = new Frustum();
    }

    private Frustum() {
        m_Frustum = new float[6][4];
        bufferMatrixProjection = BufferUtils.createFloatBuffer(16);
        bufferMatrixModel = BufferUtils.createFloatBuffer(16);
    }

    public static Frustum getFrustum() {
        Frustum.frustumObj.calculateFrustum();
        return Frustum.frustumObj;
    }

    private void normalizePlane(float[][] frustum, int side) {
        float magnitude = (float)Math.sqrt(Math.pow(frustum[side][PLANE_A], 2.0) +
                Math.pow(frustum[side][PLANE_B], 2.0) +
                Math.pow(frustum[side][PLANE_C], 2.0));
        frustum[side][PLANE_A] /= magnitude;
        frustum[side][PLANE_B] /= magnitude;
        frustum[side][PLANE_C] /= magnitude;
        frustum[side][PLANE_D] /= magnitude;
    }

    private void calculateFrustum() {
        bufferMatrixProjection.clear();
        bufferMatrixModel.clear();
        glGetFloat(GL_PROJECTION_MATRIX, bufferMatrixProjection);
        glGetFloat(GL_MODELVIEW_MATRIX, bufferMatrixModel);
        float[] matrixProjection = new float[16];
        float[] matrixModel = new float[16];
        float[] clip = new float[16];
        bufferMatrixProjection.flip().limit(16);
        bufferMatrixProjection.get(matrixProjection);
        bufferMatrixModel.flip().limit(16);
        bufferMatrixModel.get(matrixModel);
        clip[0] = matrixModel[0] * matrixProjection[0] + matrixModel[1] * matrixProjection[4] + matrixModel[2] * matrixProjection[8] + matrixModel[3] * matrixProjection[12];
        clip[1] = matrixModel[0] * matrixProjection[1] + matrixModel[1] * matrixProjection[5] + matrixModel[2] * matrixProjection[9] + matrixModel[3] * matrixProjection[13];
        clip[2] = matrixModel[0] * matrixProjection[2] + matrixModel[1] * matrixProjection[6] + matrixModel[2] * matrixProjection[10] + matrixModel[3] * matrixProjection[14];
        clip[3] = matrixModel[0] * matrixProjection[3] + matrixModel[1] * matrixProjection[7] + matrixModel[2] * matrixProjection[11] + matrixModel[3] * matrixProjection[15];
        clip[4] = matrixModel[4] * matrixProjection[0] + matrixModel[5] * matrixProjection[4] + matrixModel[6] * matrixProjection[8] + matrixModel[7] * matrixProjection[12];
        clip[5] = matrixModel[4] * matrixProjection[1] + matrixModel[5] * matrixProjection[5] + matrixModel[6] * matrixProjection[9] + matrixModel[7] * matrixProjection[13];
        clip[6] = matrixModel[4] * matrixProjection[2] + matrixModel[5] * matrixProjection[6] + matrixModel[6] * matrixProjection[10] + matrixModel[7] * matrixProjection[14];
        clip[7] = matrixModel[4] * matrixProjection[3] + matrixModel[5] * matrixProjection[7] + matrixModel[6] * matrixProjection[11] + matrixModel[7] * matrixProjection[15];
        clip[8] = matrixModel[8] * matrixProjection[0] + matrixModel[9] * matrixProjection[4] + matrixModel[10] * matrixProjection[8] + matrixModel[11] * matrixProjection[12];
        clip[9] = matrixModel[8] * matrixProjection[1] + matrixModel[9] * matrixProjection[5] + matrixModel[10] * matrixProjection[9] + matrixModel[11] * matrixProjection[13];
        clip[10] = matrixModel[8] * matrixProjection[2] + matrixModel[9] * matrixProjection[6] + matrixModel[10] * matrixProjection[10] + matrixModel[11] * matrixProjection[14];
        clip[11] = matrixModel[8] * matrixProjection[3] + matrixModel[9] * matrixProjection[7] + matrixModel[10] * matrixProjection[11] + matrixModel[11] * matrixProjection[15];
        clip[12] = matrixModel[12] * matrixProjection[0] + matrixModel[13] * matrixProjection[4] + matrixModel[14] * matrixProjection[8] + matrixModel[15] * matrixProjection[12];
        clip[13] = matrixModel[12] * matrixProjection[1] + matrixModel[13] * matrixProjection[5] + matrixModel[14] * matrixProjection[9] + matrixModel[15] * matrixProjection[13];
        clip[14] = matrixModel[12] * matrixProjection[2] + matrixModel[13] * matrixProjection[6] + matrixModel[14] * matrixProjection[10] + matrixModel[15] * matrixProjection[14];
        clip[15] = matrixModel[12] * matrixProjection[3] + matrixModel[13] * matrixProjection[7] + matrixModel[14] * matrixProjection[11] + matrixModel[15] * matrixProjection[15];

        m_Frustum[FRUSTUM_SIDE_RIGHT][PLANE_A] = clip[3] - clip[0];
        m_Frustum[FRUSTUM_SIDE_RIGHT][PLANE_B] = clip[7] - clip[4];
        m_Frustum[FRUSTUM_SIDE_RIGHT][PLANE_C] = clip[11] - clip[8];
        m_Frustum[FRUSTUM_SIDE_RIGHT][PLANE_D] = clip[15] - clip[12];
        normalizePlane(m_Frustum, FRUSTUM_SIDE_RIGHT);

        m_Frustum[FRUSTUM_SIDE_LEFT][PLANE_A] = clip[3] + clip[0];
        m_Frustum[FRUSTUM_SIDE_LEFT][PLANE_B] = clip[7] + clip[4];
        m_Frustum[FRUSTUM_SIDE_LEFT][PLANE_C] = clip[11] + clip[8];
        m_Frustum[FRUSTUM_SIDE_LEFT][PLANE_D] = clip[15] + clip[12];
        normalizePlane(m_Frustum, FRUSTUM_SIDE_LEFT);

        m_Frustum[FRUSTUM_SIDE_BOTTOM][PLANE_A] = clip[3] + clip[1];
        m_Frustum[FRUSTUM_SIDE_BOTTOM][PLANE_B] = clip[7] + clip[5];
        m_Frustum[FRUSTUM_SIDE_BOTTOM][PLANE_C] = clip[11] + clip[9];
        m_Frustum[FRUSTUM_SIDE_BOTTOM][PLANE_D] = clip[15] + clip[13];
        normalizePlane(m_Frustum, FRUSTUM_SIDE_BOTTOM);

        m_Frustum[FRUSTUM_SIDE_TOP][PLANE_A] = clip[3] - clip[1];
        m_Frustum[FRUSTUM_SIDE_TOP][PLANE_B] = clip[7] - clip[5];
        m_Frustum[FRUSTUM_SIDE_TOP][PLANE_C] = clip[11] - clip[9];
        m_Frustum[FRUSTUM_SIDE_TOP][PLANE_D] = clip[15] - clip[13];
        normalizePlane(m_Frustum, FRUSTUM_SIDE_TOP);

        m_Frustum[FRUSTUM_SIDE_BACK][PLANE_A] = clip[3] - clip[2];
        m_Frustum[FRUSTUM_SIDE_BACK][PLANE_B] = clip[7] - clip[6];
        m_Frustum[FRUSTUM_SIDE_BACK][PLANE_C] = clip[11] - clip[10];
        m_Frustum[FRUSTUM_SIDE_BACK][PLANE_D] = clip[15] - clip[14];
        normalizePlane(m_Frustum, FRUSTUM_SIDE_BACK);

        m_Frustum[FRUSTUM_SIDE_FRONT][PLANE_A] = clip[3] + clip[2];
        m_Frustum[FRUSTUM_SIDE_FRONT][PLANE_B] = clip[7] + clip[6];
        m_Frustum[FRUSTUM_SIDE_FRONT][PLANE_C] = clip[11] + clip[10];
        m_Frustum[FRUSTUM_SIDE_FRONT][PLANE_D] = clip[15] + clip[14];
        normalizePlane(m_Frustum, FRUSTUM_SIDE_FRONT);
    }

    public boolean pointInFrustum(float x, float y, float z) {
        for (int i = 0; i < 6; ++i) {
            if (m_Frustum[i][PLANE_A] * x +
                    m_Frustum[i][PLANE_B] * y +
                    m_Frustum[i][PLANE_C] * z +
                    m_Frustum[i][PLANE_D] <= 0.0f) {
                return false;
            }
        }
        return true;
    }

    public boolean sphereInFrustum(float x, float y, float z, float radius) {
        for (int i = 0; i < 6; ++i) {
            if (m_Frustum[i][PLANE_A] * x +
                    m_Frustum[i][PLANE_B] * y +
                    m_Frustum[i][PLANE_C] * z +
                    m_Frustum[i][PLANE_D] <= -radius) {
                return false;
            }
        }
        return true;
    }

    public boolean cubeFullyInFrustum(float x0, float y0, float z0, float x1, float y1, float z1) {
        for (int i = 0; i < 6; ++i) {
            if (m_Frustum[i][PLANE_A] * x0 + m_Frustum[i][PLANE_B] * y0 + m_Frustum[i][PLANE_C] * z0 + m_Frustum[i][PLANE_D] <= 0.0f) {
                return false;
            }
            if (m_Frustum[i][PLANE_A] * x1 + m_Frustum[i][PLANE_B] * y0 + m_Frustum[i][PLANE_C] * z0 + m_Frustum[i][PLANE_D] <= 0.0f) {
                return false;
            }
            if (m_Frustum[i][PLANE_A] * x0 + m_Frustum[i][PLANE_B] * y1 + m_Frustum[i][PLANE_C] * z0 + m_Frustum[i][PLANE_D] <= 0.0f) {
                return false;
            }
            if (m_Frustum[i][PLANE_A] * x1 + m_Frustum[i][PLANE_B] * y1 + m_Frustum[i][PLANE_C] * z0 + m_Frustum[i][PLANE_D] <= 0.0f) {
                return false;
            }
            if (m_Frustum[i][PLANE_A] * x0 + m_Frustum[i][PLANE_B] * y0 + m_Frustum[i][PLANE_C] * z1 + m_Frustum[i][PLANE_D] <= 0.0f) {
                return false;
            }
            if (m_Frustum[i][PLANE_A] * x1 + m_Frustum[i][PLANE_B] * y0 + m_Frustum[i][PLANE_C] * z1 + m_Frustum[i][PLANE_D] <= 0.0f) {
                return false;
            }
            if (m_Frustum[i][PLANE_A] * x0 + m_Frustum[i][PLANE_B] * y1 + m_Frustum[i][PLANE_C] * z1 + m_Frustum[i][PLANE_D] <= 0.0f) {
                return false;
            }
            if (m_Frustum[i][PLANE_A] * x1 + m_Frustum[i][PLANE_B] * y1 + m_Frustum[i][PLANE_C] * z1 + m_Frustum[i][PLANE_D] <= 0.0f) {
                return false;
            }
        }
        return true;
    }

    /*
     * cubeInFrustum()
     * Проверяет, попадает ли куб в зону видимости игрока.
     * Вычисляется относительно абсолютной позиции куба. Подробнее описано в классе CollisionBox.
     * Передаются абсолютные координаты начала куба и его полный размер.
     * Данная функция не работает с параллелепипедами.
     */
    public boolean cubeInFrustum(float x, float y, float z, float size_) {
        float x1 = x;
        float y1 = y;
        float z1 = z;
        float x2 = x + size_;
        float y2 = y + size_;
        float z2 = z + size_;
        for (int i = 0; i < 6; ++i) {
            if (m_Frustum[i][PLANE_A] * x1 + m_Frustum[i][PLANE_B] * y1 + m_Frustum[i][PLANE_C] * z1 + m_Frustum[i][PLANE_D] <= 0.0) {
                if (m_Frustum[i][PLANE_A] * x2 + m_Frustum[i][PLANE_B] * y1 + m_Frustum[i][PLANE_C] * z1 + m_Frustum[i][PLANE_D] <= 0) {
                    if (m_Frustum[i][PLANE_A] * x1 + m_Frustum[i][PLANE_B] * y2 + m_Frustum[i][PLANE_C] * z1 + m_Frustum[i][PLANE_D] <= 0) {
                        if (m_Frustum[i][PLANE_A] * x2 + m_Frustum[i][PLANE_B] * y2 + m_Frustum[i][PLANE_C] * z1 + m_Frustum[i][PLANE_D] <= 0) {
                            if (m_Frustum[i][PLANE_A] * x1 + m_Frustum[i][PLANE_B] * y1 + m_Frustum[i][PLANE_C] * z2 + m_Frustum[i][PLANE_D] <= 0) {
                                if (m_Frustum[i][PLANE_A] * x2 + m_Frustum[i][PLANE_B] * y1 + m_Frustum[i][PLANE_C] * z2 + m_Frustum[i][PLANE_D] <= 0) {
                                    if (m_Frustum[i][PLANE_A] * x1 + m_Frustum[i][PLANE_B] * y2 + m_Frustum[i][PLANE_C] * z2 + m_Frustum[i][PLANE_D] <= 0) {
                                        if (m_Frustum[i][PLANE_A] * x2 + m_Frustum[i][PLANE_B] * y2 + m_Frustum[i][PLANE_C] * z2 + m_Frustum[i][PLANE_D] <= 0) {
                                            return false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return true;
    }
}
