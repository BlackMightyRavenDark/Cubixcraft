#include "frustum.h"

void CFrustum::CalculateFrustum()
{
	float matrixProjection[16];
	float matrixModel[16];
	float clip[16];

	glGetFloatv(GL_PROJECTION_MATRIX, matrixProjection);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrixModel);

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

	m_Frustum[FRUSTUM_RIGHT][PLANE_A] = clip[3] - clip[0];
	m_Frustum[FRUSTUM_RIGHT][PLANE_B] = clip[7] - clip[4];
	m_Frustum[FRUSTUM_RIGHT][PLANE_C] = clip[11] - clip[8];
	m_Frustum[FRUSTUM_RIGHT][PLANE_D] = clip[15] - clip[12];

	NormalizePlane(m_Frustum, FRUSTUM_RIGHT);

	m_Frustum[FRUSTUM_LEFT][PLANE_A] = clip[3] + clip[0];
	m_Frustum[FRUSTUM_LEFT][PLANE_B] = clip[7] + clip[4];
	m_Frustum[FRUSTUM_LEFT][PLANE_C] = clip[11] + clip[8];
	m_Frustum[FRUSTUM_LEFT][PLANE_D] = clip[15] + clip[12];

	NormalizePlane(m_Frustum, FRUSTUM_LEFT);

	m_Frustum[FRUSTUM_BOTTOM][PLANE_A] = clip[3] + clip[1];
	m_Frustum[FRUSTUM_BOTTOM][PLANE_B] = clip[7] + clip[5];
	m_Frustum[FRUSTUM_BOTTOM][PLANE_C] = clip[11] + clip[9];
	m_Frustum[FRUSTUM_BOTTOM][PLANE_D] = clip[15] + clip[13];

	NormalizePlane(m_Frustum, FRUSTUM_BOTTOM);

	m_Frustum[FRUSTUM_TOP][PLANE_A] = clip[3] - clip[1];
	m_Frustum[FRUSTUM_TOP][PLANE_B] = clip[7] - clip[5];
	m_Frustum[FRUSTUM_TOP][PLANE_C] = clip[11] - clip[9];
	m_Frustum[FRUSTUM_TOP][PLANE_D] = clip[15] - clip[13];

	NormalizePlane(m_Frustum, FRUSTUM_TOP);

	m_Frustum[FRUSTUM_BACK][PLANE_A] = clip[3] - clip[2];
	m_Frustum[FRUSTUM_BACK][PLANE_B] = clip[7] - clip[6];
	m_Frustum[FRUSTUM_BACK][PLANE_C] = clip[11] - clip[10];
	m_Frustum[FRUSTUM_BACK][PLANE_D] = clip[15] - clip[14];

	NormalizePlane(m_Frustum, FRUSTUM_BACK);

	m_Frustum[FRUSTUM_FRONT][PLANE_A] = clip[3] + clip[2];
	m_Frustum[FRUSTUM_FRONT][PLANE_B] = clip[7] + clip[6];
	m_Frustum[FRUSTUM_FRONT][PLANE_C] = clip[11] + clip[10];
	m_Frustum[FRUSTUM_FRONT][PLANE_D] = clip[15] + clip[14];

	NormalizePlane(m_Frustum, FRUSTUM_FRONT);
}

void CFrustum::NormalizePlane(float frustum[6][4], int side)
{
	float magnitude = sqrtf(powf(frustum[side][PLANE_A], 2.0f) +
		powf(frustum[side][PLANE_B], 2.0f) + powf(frustum[side][PLANE_C], 2.0f));
	frustum[side][PLANE_A] /= magnitude;
	frustum[side][PLANE_B] /= magnitude;
	frustum[side][PLANE_C] /= magnitude;
	frustum[side][PLANE_D] /= magnitude;
}

bool CFrustum::PointInFrustum(float x, float y, float z)
{
	for (int i = 0; i < 6; i++)
	{
		if (m_Frustum[i][PLANE_A] * x + m_Frustum[i][PLANE_B] * y + m_Frustum[i][PLANE_C] * z + m_Frustum[i][PLANE_D] <= 0)
		{
			return false;
		}
	}
	return true;
}

bool CFrustum::SphereInFrustum(float x, float y, float z, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		if (m_Frustum[i][PLANE_A] * x + m_Frustum[i][PLANE_B] * y + m_Frustum[i][PLANE_C] * z + m_Frustum[i][PLANE_D] <= -radius)
		{
			return false;
		}
	}
	return true;
}

/*
 * CubeInFrustum()
 * ѕровер€ет, попадает ли куб в зону видимости игрока.
 * ¬ычисл€етс€ относительно абсолютной позиции куба. ѕодробнее описано в классе CollisionBox.
 * ѕередаютс€ абсолютные координаты начала куба и его полный размер.
 * ƒанна€ функци€ не работает с параллелепипедами.
 */
bool CFrustum::CubeInFrustum(float x, float y, float z, float size_) {
	float x1 = x;
	float y1 = y;
	float z1 = z;
	float x2 = x + size_;
	float y2 = y + size_;
	float z2 = z + size_;
	for (int i = 0; i < 6; ++i) {
		if (m_Frustum[i][PLANE_A] * x1 + m_Frustum[i][PLANE_B] * y1 + m_Frustum[i][PLANE_C] * z1 + m_Frustum[i][PLANE_D] <= 0) {
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

/*
 * CubeInFrustum2()
 * ѕровер€ет, попадает ли куб в зону видимости игрока.
 * ¬ычисл€етс€ относительно абсолютной позиции центра куба.
 * ѕередаютс€ абсолютные координаты центра куба и половина его размера.
 * ѕодробнее о координатах написано в классе CollisionBox.
 * ƒанна€ функци€ не работает с параллелепипедами.
 */
bool CFrustum::CubeInFrustum2(float x, float y, float z, float size_) {
	for (int i = 0; i < 6; i++) {
		if (m_Frustum[i][PLANE_A] * (x - size_) + m_Frustum[i][PLANE_B] * (y - size_) + m_Frustum[i][PLANE_C] * (z - size_) + m_Frustum[i][PLANE_D] > 0)
			continue;
		if (m_Frustum[i][PLANE_A] * (x + size_) + m_Frustum[i][PLANE_B] * (y - size_) + m_Frustum[i][PLANE_C] * (z - size_) + m_Frustum[i][PLANE_D] > 0)
			continue;
		if (m_Frustum[i][PLANE_A] * (x - size_) + m_Frustum[i][PLANE_B] * (y + size_) + m_Frustum[i][PLANE_C] * (z - size_) + m_Frustum[i][PLANE_D] > 0)
			continue;
		if (m_Frustum[i][PLANE_A] * (x + size_) + m_Frustum[i][PLANE_B] * (y + size_) + m_Frustum[i][PLANE_C] * (z - size_) + m_Frustum[i][PLANE_D] > 0)
			continue;
		if (m_Frustum[i][PLANE_A] * (x - size_) + m_Frustum[i][PLANE_B] * (y - size_) + m_Frustum[i][PLANE_C] * (z + size_) + m_Frustum[i][PLANE_D] > 0)
			continue;
		if (m_Frustum[i][PLANE_A] * (x + size_) + m_Frustum[i][PLANE_B] * (y - size_) + m_Frustum[i][PLANE_C] * (z + size_) + m_Frustum[i][PLANE_D] > 0)
			continue;
		if (m_Frustum[i][PLANE_A] * (x - size_) + m_Frustum[i][PLANE_B] * (y + size_) + m_Frustum[i][PLANE_C] * (z + size_) + m_Frustum[i][PLANE_D] > 0)
			continue;
		if (m_Frustum[i][PLANE_A] * (x + size_) + m_Frustum[i][PLANE_B] * (y + size_) + m_Frustum[i][PLANE_C] * (z + size_) + m_Frustum[i][PLANE_D] > 0)
			continue;
		return false;
	}
	return true;
}
