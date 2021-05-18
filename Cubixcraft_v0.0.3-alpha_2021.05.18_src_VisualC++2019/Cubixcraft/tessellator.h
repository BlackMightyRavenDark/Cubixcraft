/*
 * Класс CTessellator это обёртка над glInterleavedArrays(), glDrawArrays()
 * и другими сопутствующими функциями.
 */

#include <GL/gl.h>

class CTessellator {
public:
    CTessellator();

    void AddVertexQuad(float x, float y, float z);
    void FlushQuads();
    void ClearQuads();
    void AddTextureVertexQuad(float u, float v, float x, float y, float z);
    void FlushTextureQuads();
    void ClearTextureQuads();
private:
    static const int MAX_FLOATS = 128 * 128;

    float arrayVertexQuads[MAX_FLOATS];
    float arrayTextureVertexQuads[MAX_FLOATS];

    int vertexQuadsPos;
    int vertexQuadsCount;

    int vertexTextureQuadsPos;
    int vertexTextureQuadsCount;
};
