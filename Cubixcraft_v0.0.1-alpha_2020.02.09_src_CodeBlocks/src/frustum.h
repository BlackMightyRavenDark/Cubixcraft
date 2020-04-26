#pragma once
#include <GL/gl.h>
#include <cmath>

class CFrustum {
public:
	void CalculateFrustum();
	bool PointInFrustum(float x, float y, float z);
	bool SphereInFrustum(float x, float y, float z, float radius);
	bool CubeInFrustum(float x, float y, float z, float size_);
	bool CubeInFrustum2(float x, float y, float z, float size_);
private:
	float m_Frustum[6][4];
};
