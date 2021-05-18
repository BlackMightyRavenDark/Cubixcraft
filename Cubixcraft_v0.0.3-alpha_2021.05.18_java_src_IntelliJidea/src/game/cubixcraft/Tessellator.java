/*
 * Класс Tessellator это обёртка над glInterleavedArrays(), glDrawArrays()
 * и другими сопутствующими функциями.
 */
package game.cubixcraft;

import org.lwjgl.BufferUtils;

import java.nio.FloatBuffer;

import static org.lwjgl.opengl.GL11.*;

public class Tessellator {

    private final int MAX_FLOATS = 128 * 128;

    private final float[] arrayVertexQuads;
    private final float[] arrayTextureVertexQuads;

    private final FloatBuffer bufferArrayVertexQuads;
    private final FloatBuffer bufferArrayTextureVertexQuad;

    private int vertexQuadsPos;
    private int vertexQuadsCount;

    private int vertexTextureQuadsPos;
    private int vertexTextureQuadsCount;

    public Tessellator() {
        bufferArrayVertexQuads = BufferUtils.createFloatBuffer(MAX_FLOATS);
        bufferArrayTextureVertexQuad = BufferUtils.createFloatBuffer(MAX_FLOATS);
        arrayVertexQuads = new float[MAX_FLOATS];
        arrayTextureVertexQuads = new float[MAX_FLOATS];
        clearQuads();
        clearTextureQuads();
    }

    public void addVertexQuad(float x, float y, float z) {
        arrayVertexQuads[vertexQuadsPos++] = x;
        arrayVertexQuads[vertexQuadsPos++] = y;
        arrayVertexQuads[vertexQuadsPos++] = z;
        vertexQuadsCount++;
        if (vertexQuadsCount % 4 == 0 && vertexQuadsPos >= MAX_FLOATS - 48) {
            flushQuads();
        }
    }

    public void flushQuads() {
        if (vertexQuadsCount > 0) {
            bufferArrayVertexQuads.put(arrayVertexQuads, 0, vertexQuadsPos);
            bufferArrayVertexQuads.flip();
            glInterleavedArrays(GL_V3F, 0, bufferArrayVertexQuads);
            glEnableClientState(GL_VERTEX_ARRAY);
            glDrawArrays(GL_QUADS, 0, vertexQuadsCount);
            glDisableClientState(GL_VERTEX_ARRAY);
            clearQuads();
        }
    }

    public void clearQuads() {
        vertexQuadsCount = vertexQuadsPos = 0;
        bufferArrayVertexQuads.clear();
    }

    public void addTextureVertexQuad(float u, float v, float x, float y, float z) {
        arrayTextureVertexQuads[vertexTextureQuadsPos++] = u;
        arrayTextureVertexQuads[vertexTextureQuadsPos++] = v;
        arrayTextureVertexQuads[vertexTextureQuadsPos++] = x;
        arrayTextureVertexQuads[vertexTextureQuadsPos++] = y;
        arrayTextureVertexQuads[vertexTextureQuadsPos++] = z;
        vertexTextureQuadsCount++;
        if (vertexTextureQuadsCount % 4 == 0 && vertexTextureQuadsPos >= MAX_FLOATS - 80) {
            flushTextureQuads();
        }
    }

    public void flushTextureQuads() {
        if (vertexTextureQuadsCount > 0) {
            bufferArrayTextureVertexQuad.put(arrayTextureVertexQuads, 0, vertexTextureQuadsPos);
            bufferArrayTextureVertexQuad.flip();
            glInterleavedArrays(GL_T2F_V3F, 0, bufferArrayTextureVertexQuad);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glDrawArrays(GL_QUADS, 0, vertexTextureQuadsCount);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            clearTextureQuads();
        }
    }

    public void clearTextureQuads() {
        vertexTextureQuadsCount = vertexTextureQuadsPos = 0;
        bufferArrayTextureVertexQuad.clear();
    }

}