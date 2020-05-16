#include "tessellator.h"

CTessellator::CTessellator() 
{
    ClearQuads();
    ClearTextureQuads();
}

void CTessellator::AddVertexQuad(float x, float y, float z) 
{
    arrayVertexQuads[vertexQuadsPos++] = x;
    arrayVertexQuads[vertexQuadsPos++] = y;
    arrayVertexQuads[vertexQuadsPos++] = z;
    vertexQuadsCount++;
    if (vertexQuadsCount % 4 == 0 && vertexQuadsPos >= MAX_FLOATS - 48) 
    {
        FlushQuads();
    }
}

void CTessellator::FlushQuads() 
{
    if (vertexQuadsCount) 
    {
        glInterleavedArrays(GL_V3F, 0, &arrayVertexQuads);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_QUADS, 0, vertexQuadsCount);
        glDisableClientState(GL_VERTEX_ARRAY);
        ClearQuads();
    }
}

void CTessellator::ClearQuads() 
{
    vertexQuadsCount = 0;
    vertexQuadsPos = 0;
}

void CTessellator::AddTextureVertexQuad(float u, float v, float x, float y, float z) 
{
    arrayTextureVertexQuads[vertexTextureQuadsPos++] = u;
    arrayTextureVertexQuads[vertexTextureQuadsPos++] = v;
    arrayTextureVertexQuads[vertexTextureQuadsPos++] = x;
    arrayTextureVertexQuads[vertexTextureQuadsPos++] = y;
    arrayTextureVertexQuads[vertexTextureQuadsPos++] = z;
    vertexTextureQuadsCount++;
    if (vertexTextureQuadsCount % 4 == 0 && vertexTextureQuadsPos >= MAX_FLOATS - 80) 
    {
        FlushTextureQuads();
    }
}

void CTessellator::FlushTextureQuads() 
{
    if (vertexTextureQuadsCount) 
    {
        glInterleavedArrays(GL_T2F_V3F, 0, &arrayTextureVertexQuads);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDrawArrays(GL_QUADS, 0, vertexTextureQuadsCount);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        ClearTextureQuads();
    }
}

void CTessellator::ClearTextureQuads() 
{
    vertexTextureQuadsCount = 0;
    vertexTextureQuadsPos = 0;
}
