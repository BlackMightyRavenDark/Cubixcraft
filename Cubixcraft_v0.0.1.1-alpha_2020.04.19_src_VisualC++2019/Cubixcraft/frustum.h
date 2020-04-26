/*
 * Frustum - это та часть мира, которая попадает в поле зрения игрока.
 * Более подробно описано здесь: 
 */

#include <GL/gl.h>
#include <cmath>

enum FRUSTUMSIDE
{
	FRUSTUM_RIGHT  = 0,
	FRUSTUM_LEFT   = 1,
	FRUSTUM_BOTTOM = 2,
	FRUSTUM_TOP    = 3,
	FRUSTUM_BACK   = 4,
	FRUSTUM_FRONT  = 5
};

enum PLANEDATA
{
	PLANE_A = 0,
	PLANE_B = 1,
	PLANE_C = 2,
	PLANE_D = 3
};

class CFrustum {
public:
	void CalculateFrustum();
	bool PointInFrustum(float x, float y, float z);
	bool SphereInFrustum(float x, float y, float z, float radius);
	bool CubeInFrustum(float x, float y, float z, float size_);
	bool CubeInFrustum2(float x, float y, float z, float size_);
private:
	float m_Frustum[6][4];
	void NormalizePlane(float frustum[6][4], int side);
};
