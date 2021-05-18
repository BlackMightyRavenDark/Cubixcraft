#include "frustum.h"

void CFrustum::CalculateFrustum()
{
	float matrixProjection[16];
	float matrixModel[16];
	float matrixClip[16];

	glGetFloatv(GL_PROJECTION_MATRIX, matrixProjection);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrixModel);

	matrixClip[0] = matrixModel[0] * matrixProjection[0] + matrixModel[1] * matrixProjection[4] + matrixModel[2] * matrixProjection[8] + matrixModel[3] * matrixProjection[12];
	matrixClip[1] = matrixModel[0] * matrixProjection[1] + matrixModel[1] * matrixProjection[5] + matrixModel[2] * matrixProjection[9] + matrixModel[3] * matrixProjection[13];
	matrixClip[2] = matrixModel[0] * matrixProjection[2] + matrixModel[1] * matrixProjection[6] + matrixModel[2] * matrixProjection[10] + matrixModel[3] * matrixProjection[14];
	matrixClip[3] = matrixModel[0] * matrixProjection[3] + matrixModel[1] * matrixProjection[7] + matrixModel[2] * matrixProjection[11] + matrixModel[3] * matrixProjection[15];

	matrixClip[4] = matrixModel[4] * matrixProjection[0] + matrixModel[5] * matrixProjection[4] + matrixModel[6] * matrixProjection[8] + matrixModel[7] * matrixProjection[12];
	matrixClip[5] = matrixModel[4] * matrixProjection[1] + matrixModel[5] * matrixProjection[5] + matrixModel[6] * matrixProjection[9] + matrixModel[7] * matrixProjection[13];
	matrixClip[6] = matrixModel[4] * matrixProjection[2] + matrixModel[5] * matrixProjection[6] + matrixModel[6] * matrixProjection[10] + matrixModel[7] * matrixProjection[14];
	matrixClip[7] = matrixModel[4] * matrixProjection[3] + matrixModel[5] * matrixProjection[7] + matrixModel[6] * matrixProjection[11] + matrixModel[7] * matrixProjection[15];

	matrixClip[8] = matrixModel[8] * matrixProjection[0] + matrixModel[9] * matrixProjection[4] + matrixModel[10] * matrixProjection[8] + matrixModel[11] * matrixProjection[12];
	matrixClip[9] = matrixModel[8] * matrixProjection[1] + matrixModel[9] * matrixProjection[5] + matrixModel[10] * matrixProjection[9] + matrixModel[11] * matrixProjection[13];
	matrixClip[10] = matrixModel[8] * matrixProjection[2] + matrixModel[9] * matrixProjection[6] + matrixModel[10] * matrixProjection[10] + matrixModel[11] * matrixProjection[14];
	matrixClip[11] = matrixModel[8] * matrixProjection[3] + matrixModel[9] * matrixProjection[7] + matrixModel[10] * matrixProjection[11] + matrixModel[11] * matrixProjection[15];

	matrixClip[12] = matrixModel[12] * matrixProjection[0] + matrixModel[13] * matrixProjection[4] + matrixModel[14] * matrixProjection[8] + matrixModel[15] * matrixProjection[12];
	matrixClip[13] = matrixModel[12] * matrixProjection[1] + matrixModel[13] * matrixProjection[5] + matrixModel[14] * matrixProjection[9] + matrixModel[15] * matrixProjection[13];
	matrixClip[14] = matrixModel[12] * matrixProjection[2] + matrixModel[13] * matrixProjection[6] + matrixModel[14] * matrixProjection[10] + matrixModel[15] * matrixProjection[14];
	matrixClip[15] = matrixModel[12] * matrixProjection[3] + matrixModel[13] * matrixProjection[7] + matrixModel[14] * matrixProjection[11] + matrixModel[15] * matrixProjection[15];

	m_Frustum[FRUSTUM_SIDE_RIGHT][FRUSTUM_PLANE_A] = matrixClip[3] - matrixClip[0];
	m_Frustum[FRUSTUM_SIDE_RIGHT][FRUSTUM_PLANE_B] = matrixClip[7] - matrixClip[4];
	m_Frustum[FRUSTUM_SIDE_RIGHT][FRUSTUM_PLANE_C] = matrixClip[11] - matrixClip[8];
	m_Frustum[FRUSTUM_SIDE_RIGHT][FRUSTUM_PLANE_D] = matrixClip[15] - matrixClip[12];

	NormalizePlane(m_Frustum, FRUSTUM_SIDE_RIGHT);

	m_Frustum[FRUSTUM_SIDE_LEFT][FRUSTUM_PLANE_A] = matrixClip[3] + matrixClip[0];
	m_Frustum[FRUSTUM_SIDE_LEFT][FRUSTUM_PLANE_B] = matrixClip[7] + matrixClip[4];
	m_Frustum[FRUSTUM_SIDE_LEFT][FRUSTUM_PLANE_C] = matrixClip[11] + matrixClip[8];
	m_Frustum[FRUSTUM_SIDE_LEFT][FRUSTUM_PLANE_D] = matrixClip[15] + matrixClip[12];

	NormalizePlane(m_Frustum, FRUSTUM_SIDE_LEFT);

	m_Frustum[FRUSTUM_SIDE_BOTTOM][FRUSTUM_PLANE_A] = matrixClip[3] + matrixClip[1];
	m_Frustum[FRUSTUM_SIDE_BOTTOM][FRUSTUM_PLANE_B] = matrixClip[7] + matrixClip[5];
	m_Frustum[FRUSTUM_SIDE_BOTTOM][FRUSTUM_PLANE_C] = matrixClip[11] + matrixClip[9];
	m_Frustum[FRUSTUM_SIDE_BOTTOM][FRUSTUM_PLANE_D] = matrixClip[15] + matrixClip[13];

	NormalizePlane(m_Frustum, FRUSTUM_SIDE_BOTTOM);

	m_Frustum[FRUSTUM_SIDE_TOP][FRUSTUM_PLANE_A] = matrixClip[3] - matrixClip[1];
	m_Frustum[FRUSTUM_SIDE_TOP][FRUSTUM_PLANE_B] = matrixClip[7] - matrixClip[5];
	m_Frustum[FRUSTUM_SIDE_TOP][FRUSTUM_PLANE_C] = matrixClip[11] - matrixClip[9];
	m_Frustum[FRUSTUM_SIDE_TOP][FRUSTUM_PLANE_D] = matrixClip[15] - matrixClip[13];

	NormalizePlane(m_Frustum, FRUSTUM_SIDE_TOP);

	m_Frustum[FRUSTUM_SIDE_BACK][FRUSTUM_PLANE_A] = matrixClip[3] - matrixClip[2];
	m_Frustum[FRUSTUM_SIDE_BACK][FRUSTUM_PLANE_B] = matrixClip[7] - matrixClip[6];
	m_Frustum[FRUSTUM_SIDE_BACK][FRUSTUM_PLANE_C] = matrixClip[11] - matrixClip[10];
	m_Frustum[FRUSTUM_SIDE_BACK][FRUSTUM_PLANE_D] = matrixClip[15] - matrixClip[14];

	NormalizePlane(m_Frustum, FRUSTUM_SIDE_BACK);

	m_Frustum[FRUSTUM_SIDE_FRONT][FRUSTUM_PLANE_A] = matrixClip[3] + matrixClip[2];
	m_Frustum[FRUSTUM_SIDE_FRONT][FRUSTUM_PLANE_B] = matrixClip[7] + matrixClip[6];
	m_Frustum[FRUSTUM_SIDE_FRONT][FRUSTUM_PLANE_C] = matrixClip[11] + matrixClip[10];
	m_Frustum[FRUSTUM_SIDE_FRONT][FRUSTUM_PLANE_D] = matrixClip[15] + matrixClip[14];

	NormalizePlane(m_Frustum, FRUSTUM_SIDE_FRONT);
}

void CFrustum::NormalizePlane(float frustum[6][4], int sideId)
{
	float length = sqrtf(
		powf(frustum[sideId][FRUSTUM_PLANE_A], 2.0f) +
		powf(frustum[sideId][FRUSTUM_PLANE_B], 2.0f) +
		powf(frustum[sideId][FRUSTUM_PLANE_C], 2.0f));
	frustum[sideId][FRUSTUM_PLANE_A] /= length;
	frustum[sideId][FRUSTUM_PLANE_B] /= length;
	frustum[sideId][FRUSTUM_PLANE_C] /= length;
	frustum[sideId][FRUSTUM_PLANE_D] /= length;
}

/*
 * CubeInFrustum()
 * ѕровер€ет, попадает ли куб в зону видимости игрока.
 * ¬ычисл€етс€ относительно абсолютной позиции куба. 
 * ѕодробнее описано в классе CCollisionBox.
 * ѕередаютс€ абсолютные координаты начала куба и его полный размер.
 * ƒанный метод не работает с параллелепипедами.
 */
bool CFrustum::CubeInFrustum(float x, float y, float z, float sizeFull) 
{
	float x1 = x + sizeFull;
	float y1 = y + sizeFull;
	float z1 = z + sizeFull;
	for (int i = 0; i < 6; ++i) {
		if (m_Frustum[i][FRUSTUM_PLANE_A] * x + m_Frustum[i][FRUSTUM_PLANE_B] * y + m_Frustum[i][FRUSTUM_PLANE_C] * z + m_Frustum[i][FRUSTUM_PLANE_D] <= 0.0f) {
			if (m_Frustum[i][FRUSTUM_PLANE_A] * x1 + m_Frustum[i][FRUSTUM_PLANE_B] * y + m_Frustum[i][FRUSTUM_PLANE_C] * z + m_Frustum[i][FRUSTUM_PLANE_D] <= 0.0f) {
				if (m_Frustum[i][FRUSTUM_PLANE_A] * x + m_Frustum[i][FRUSTUM_PLANE_B] * y1 + m_Frustum[i][FRUSTUM_PLANE_C] * z + m_Frustum[i][FRUSTUM_PLANE_D] <= 0.0f) {
					if (m_Frustum[i][FRUSTUM_PLANE_A] * x1 + m_Frustum[i][FRUSTUM_PLANE_B] * y1 + m_Frustum[i][FRUSTUM_PLANE_C] * z + m_Frustum[i][FRUSTUM_PLANE_D] <= 0.0f) {
						if (m_Frustum[i][FRUSTUM_PLANE_A] * x + m_Frustum[i][FRUSTUM_PLANE_B] * y + m_Frustum[i][FRUSTUM_PLANE_C] * z1 + m_Frustum[i][FRUSTUM_PLANE_D] <= 0.0f) {
							if (m_Frustum[i][FRUSTUM_PLANE_A] * x1 + m_Frustum[i][FRUSTUM_PLANE_B] * y + m_Frustum[i][FRUSTUM_PLANE_C] * z1 + m_Frustum[i][FRUSTUM_PLANE_D] <= 0.0f) {
								if (m_Frustum[i][FRUSTUM_PLANE_A] * x + m_Frustum[i][FRUSTUM_PLANE_B] * y1 + m_Frustum[i][FRUSTUM_PLANE_C] * z1 + m_Frustum[i][FRUSTUM_PLANE_D] <= 0.0f) {
									if (m_Frustum[i][FRUSTUM_PLANE_A] * x1 + m_Frustum[i][FRUSTUM_PLANE_B] * y1 + m_Frustum[i][FRUSTUM_PLANE_C] * z1 + m_Frustum[i][FRUSTUM_PLANE_D] <= 0.0f) {
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
