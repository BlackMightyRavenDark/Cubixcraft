#include "frustum.h"

enum FrustumSide
{
	RIGHT  = 0,
	LEFT = 1,
	BOTTOM = 2,
	TOP = 3,
	BACK = 4,
	FRONT = 5
};

enum PlaneData
{
	A = 0,
	B = 1,
	C = 2,
	D = 3
};

void NormalizePlane(float frustum[6][4], int side)
{
	float magnitude = (float)sqrtf(frustum[side][A] * frustum[side][A] +
		frustum[side][B] * frustum[side][B] +
		frustum[side][C] * frustum[side][C]);
	frustum[side][A] /= magnitude;
	frustum[side][B] /= magnitude;
	frustum[side][C] /= magnitude;
	frustum[side][D] /= magnitude;
}

void CFrustum::CalculateFrustum()
{
	float matrixMProjection[16];
	float matrixModel[16];
	float clip[16];

	glGetFloatv(GL_PROJECTION_MATRIX, matrixMProjection);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrixModel);
	clip[0] = matrixModel[0] * matrixMProjection[0] + matrixModel[1] * matrixMProjection[4] + matrixModel[2] * matrixMProjection[8] + matrixModel[3] * matrixMProjection[12];
	clip[1] = matrixModel[0] * matrixMProjection[1] + matrixModel[1] * matrixMProjection[5] + matrixModel[2] * matrixMProjection[9] + matrixModel[3] * matrixMProjection[13];
	clip[2] = matrixModel[0] * matrixMProjection[2] + matrixModel[1] * matrixMProjection[6] + matrixModel[2] * matrixMProjection[10] + matrixModel[3] * matrixMProjection[14];
	clip[3] = matrixModel[0] * matrixMProjection[3] + matrixModel[1] * matrixMProjection[7] + matrixModel[2] * matrixMProjection[11] + matrixModel[3] * matrixMProjection[15];

	clip[4] = matrixModel[4] * matrixMProjection[0] + matrixModel[5] * matrixMProjection[4] + matrixModel[6] * matrixMProjection[8] + matrixModel[7] * matrixMProjection[12];
	clip[5] = matrixModel[4] * matrixMProjection[1] + matrixModel[5] * matrixMProjection[5] + matrixModel[6] * matrixMProjection[9] + matrixModel[7] * matrixMProjection[13];
	clip[6] = matrixModel[4] * matrixMProjection[2] + matrixModel[5] * matrixMProjection[6] + matrixModel[6] * matrixMProjection[10] + matrixModel[7] * matrixMProjection[14];
	clip[7] = matrixModel[4] * matrixMProjection[3] + matrixModel[5] * matrixMProjection[7] + matrixModel[6] * matrixMProjection[11] + matrixModel[7] * matrixMProjection[15];

	clip[8] = matrixModel[8] * matrixMProjection[0] + matrixModel[9] * matrixMProjection[4] + matrixModel[10] * matrixMProjection[8] + matrixModel[11] * matrixMProjection[12];
	clip[9] = matrixModel[8] * matrixMProjection[1] + matrixModel[9] * matrixMProjection[5] + matrixModel[10] * matrixMProjection[9] + matrixModel[11] * matrixMProjection[13];
	clip[10] = matrixModel[8] * matrixMProjection[2] + matrixModel[9] * matrixMProjection[6] + matrixModel[10] * matrixMProjection[10] + matrixModel[11] * matrixMProjection[14];
	clip[11] = matrixModel[8] * matrixMProjection[3] + matrixModel[9] * matrixMProjection[7] + matrixModel[10] * matrixMProjection[11] + matrixModel[11] * matrixMProjection[15];

	clip[12] = matrixModel[12] * matrixMProjection[0] + matrixModel[13] * matrixMProjection[4] + matrixModel[14] * matrixMProjection[8] + matrixModel[15] * matrixMProjection[12];
	clip[13] = matrixModel[12] * matrixMProjection[1] + matrixModel[13] * matrixMProjection[5] + matrixModel[14] * matrixMProjection[9] + matrixModel[15] * matrixMProjection[13];
	clip[14] = matrixModel[12] * matrixMProjection[2] + matrixModel[13] * matrixMProjection[6] + matrixModel[14] * matrixMProjection[10] + matrixModel[15] * matrixMProjection[14];
	clip[15] = matrixModel[12] * matrixMProjection[3] + matrixModel[13] * matrixMProjection[7] + matrixModel[14] * matrixMProjection[11] + matrixModel[15] * matrixMProjection[15];

	m_Frustum[RIGHT][A] = clip[3] - clip[0];
	m_Frustum[RIGHT][B] = clip[7] - clip[4];
	m_Frustum[RIGHT][C] = clip[11] - clip[8];
	m_Frustum[RIGHT][D] = clip[15] - clip[12];

	NormalizePlane(m_Frustum, RIGHT);

	m_Frustum[LEFT][A] = clip[3] + clip[0];
	m_Frustum[LEFT][B] = clip[7] + clip[4];
	m_Frustum[LEFT][C] = clip[11] + clip[8];
	m_Frustum[LEFT][D] = clip[15] + clip[12];

	NormalizePlane(m_Frustum, LEFT);

	m_Frustum[BOTTOM][A] = clip[3] + clip[1];
	m_Frustum[BOTTOM][B] = clip[7] + clip[5];
	m_Frustum[BOTTOM][C] = clip[11] + clip[9];
	m_Frustum[BOTTOM][D] = clip[15] + clip[13];

	NormalizePlane(m_Frustum, BOTTOM);

	m_Frustum[TOP][A] = clip[3] - clip[1];
	m_Frustum[TOP][B] = clip[7] - clip[5];
	m_Frustum[TOP][C] = clip[11] - clip[9];
	m_Frustum[TOP][D] = clip[15] - clip[13];

	NormalizePlane(m_Frustum, TOP);

	m_Frustum[BACK][A] = clip[3] - clip[2];
	m_Frustum[BACK][B] = clip[7] - clip[6];
	m_Frustum[BACK][C] = clip[11] - clip[10];
	m_Frustum[BACK][D] = clip[15] - clip[14];

	NormalizePlane(m_Frustum, BACK);

	m_Frustum[FRONT][A] = clip[3] + clip[2];
	m_Frustum[FRONT][B] = clip[7] + clip[6];
	m_Frustum[FRONT][C] = clip[11] + clip[10];
	m_Frustum[FRONT][D] = clip[15] + clip[14];

	NormalizePlane(m_Frustum, FRONT);
}

bool CFrustum::PointInFrustum(float x, float y, float z)
{
	for (int i = 0; i < 6; i++)
	{
		if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= 0)
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
		if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= -radius)
		{
			return false;
		}
	}
	return true;
}

bool CFrustum::CubeInFrustum(float x, float y, float z, float size_){
    float x1 = x;
    float y1 = y;
    float z1 = z;
    float x2 = x + size_;
    float y2 = y + size_;
    float z2 = z + size_;
    for (int i = 0; i < 6; ++i) {
        if (m_Frustum[i][A] * x1 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z1 + m_Frustum[i][D] <= 0.0) {
            if (m_Frustum[i][A] * x2 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z1 + m_Frustum[i][D] <= 0.0) {
                if (m_Frustum[i][A] * x1 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z1 + m_Frustum[i][D] <= 0.0) {
                    if (m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z1 + m_Frustum[i][D] <= 0.0) {
                        if (m_Frustum[i][A] * x1 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z2 + m_Frustum[i][D] <= 0.0) {
                            if (m_Frustum[i][A] * x2 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z2 + m_Frustum[i][D] <= 0.0) {
                                if (m_Frustum[i][A] * x1 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] <= 0.0) {
                                    if (m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] <= 0.0) {
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

bool CFrustum::CubeInFrustum2(float x, float y, float z, float size_){
    for (int i = 0; i < 6; i++){
        if (m_Frustum[i][A] * (x - size_) + m_Frustum[i][B] * (y - size_) + m_Frustum[i][C] * (z - size_) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x + size_) + m_Frustum[i][B] * (y - size_) + m_Frustum[i][C] * (z - size_) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x - size_) + m_Frustum[i][B] * (y + size_) + m_Frustum[i][C] * (z - size_) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x + size_) + m_Frustum[i][B] * (y + size_) + m_Frustum[i][C] * (z - size_) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x - size_) + m_Frustum[i][B] * (y - size_) + m_Frustum[i][C] * (z + size_) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x + size_) + m_Frustum[i][B] * (y - size_) + m_Frustum[i][C] * (z + size_) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x - size_) + m_Frustum[i][B] * (y + size_) + m_Frustum[i][C] * (z + size_) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x + size_) + m_Frustum[i][B] * (y + size_) + m_Frustum[i][C] * (z + size_) + m_Frustum[i][D] > 0)
            continue;
        return false;
    }
    return true;
}
