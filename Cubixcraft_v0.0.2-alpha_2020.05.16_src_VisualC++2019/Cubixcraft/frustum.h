/*
 * Frustum - это та часть мира, которая попадает в поле зрения игрока.
 * Более подробно описано здесь: 
 * https://masandilov.ru/opengl/opengl-frustum
 */

#include <GL/gl.h>
#include <cmath>

enum FRUSTUMSIDE
{
	FRUSTUM_SIDE_RIGHT  = 0,
	FRUSTUM_SIDE_LEFT   = 1,
	FRUSTUM_SIDE_BOTTOM = 2,
	FRUSTUM_SIDE_TOP    = 3,
	FRUSTUM_SIDE_BACK   = 4,
	FRUSTUM_SIDE_FRONT  = 5
};

enum PLANEDATA
{
	FRUSTUM_PLANE_A = 0,
	FRUSTUM_PLANE_B = 1,
	FRUSTUM_PLANE_C = 2,
	FRUSTUM_PLANE_D = 3
};

class CFrustum {
public:
	void CalculateFrustum();
	bool CubeInFrustum(float x, float y, float z, float sizeFull);
	bool CubeInFrustum2(float x, float y, float z, float sizeHalf);
private:
	float m_Frustum[6][4];
	void NormalizePlane(float frustum[6][4], int side);
};
