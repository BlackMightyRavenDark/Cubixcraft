#include "utils.h"
#include "block.h"
#include "tessellator.h"

/*
 * RenderCubeSide2D()
 * ������ ��������� ������� ����� � HUD.
 * ��� ����������� ������, ���������� ��������� GL_LIGHTING � �������� GL_DEPTH_TEST.
 */
int RenderCubeSide2D(CBlock* b, CTessellator* t, float x, float y, float z, float cubeSize, int cubeSide)
{
    int sidesRendered = 0;
    POINT pt = b->GetTextureCoordinates(cubeSide);
 
    /*
     * u0, v0 - ���������� x,y ����� ������� ����� �������� ������� ������� ����� � ���������� ������.
     * u1, v1 - ���������� x,y ������ ������ ����� �������� ������� ������� ����� � ���������� ������.
     * �� ����������� ������ ����� �������� ������� ����� ����� �������
     * � �������� �� ������� ������� ����� � �������� �� ��������.
     * 16 - ��� ���������� ������� � ������ �� ��������� � �����������.
     * ���������� ����� ������������ ��� ���������� ���������� ������������
     * ����� ���������� �, ��� ���������, ��������� ���������� � ��������� FPS.
     */
    float u0 = pt.x / 16.0f;
    float v0 = pt.y / 16.0f;
    float u1 = u0 + 1.0f / 16.0f;
    float v1 = v0 + 1.0f / 16.0f;
    switch (cubeSide)
    {
        case BLOCK_SIDE_BACK:
            t->AddTextureVertexQuad(u0, v1, x,            y,            z           );
            t->AddTextureVertexQuad(u1, v1, x + cubeSize, y,            z           );
            t->AddTextureVertexQuad(u1, v0, x + cubeSize, y + cubeSize, z           );
            t->AddTextureVertexQuad(u0, v0, x,            y + cubeSize, z           );
            sidesRendered++;
            break;
        case BLOCK_SIDE_FRONT:
            t->AddTextureVertexQuad(u0, v1, x,            y,            z + cubeSize);
            t->AddTextureVertexQuad(u1, v1, x + cubeSize, y,            z + cubeSize);
            t->AddTextureVertexQuad(u1, v0, x + cubeSize, y + cubeSize, z + cubeSize);
            t->AddTextureVertexQuad(u0, v0, x,            y + cubeSize, z + cubeSize);
            sidesRendered++;
            break;
        case BLOCK_SIDE_LEFT:
            t->AddTextureVertexQuad(u0, v1, x,            y,            z           );
            t->AddTextureVertexQuad(u1, v1, x,            y,            z + cubeSize);
            t->AddTextureVertexQuad(u1, v0, x,            y + cubeSize, z + cubeSize);
            t->AddTextureVertexQuad(u0, v0, x,            y + cubeSize, z           );
            sidesRendered++;
            break;
        case BLOCK_SIDE_RIGHT:
            t->AddTextureVertexQuad(u0, v1, x + cubeSize, y,            z           );
            t->AddTextureVertexQuad(u1, v1, x + cubeSize, y,            z + cubeSize);
            t->AddTextureVertexQuad(u1, v0, x + cubeSize, y + cubeSize, z + cubeSize);
            t->AddTextureVertexQuad(u0, v0, x + cubeSize, y + cubeSize, z           );
            sidesRendered++;
            break;
        case BLOCK_SIDE_BOTTOM:
            t->AddTextureVertexQuad(u0, v1, x,            y,            z           );
            t->AddTextureVertexQuad(u1, v1, x + cubeSize, y,            z           );
            t->AddTextureVertexQuad(u1, v0, x + cubeSize, y,            z + cubeSize);
            t->AddTextureVertexQuad(u0, v0, x,            y,            z + cubeSize);
            sidesRendered++;
            break;
        case BLOCK_SIDE_TOP:
            t->AddTextureVertexQuad(u0, v1, x,            y + cubeSize, z           );
            t->AddTextureVertexQuad(u1, v1, x + cubeSize, y + cubeSize, z           );
            t->AddTextureVertexQuad(u1, v0, x + cubeSize, y + cubeSize, z + cubeSize);
            t->AddTextureVertexQuad(u0, v0, x,            y + cubeSize, z + cubeSize);
            sidesRendered++;
            break;
    }
    return sidesRendered;
}

int CheckOpenGlError(std::string t)
{
    int errorCode = glGetError();
    switch (errorCode)
    {
        case GL_INVALID_ENUM:
            std::cout << "!!!OpenGL error in " << t << ": GL_INVALID_ENUM" << std::endl;
            break;
        case GL_INVALID_VALUE:
            std::cout << "!!!OpenGL error in " << t << ": GL_INVALID_VALUE" << std::endl;
            break;
        case GL_INVALID_OPERATION:
            std::cout << "!!!OpenGL error in " << t << ": GL_INVALID_OPERATION" << std::endl;
            break;
        case GL_STACK_OVERFLOW:
            std::cout << "!!!OpenGL error in " << t << ": GL_STACK_OVERFLOW" << std::endl;
            break;
        case GL_STACK_UNDERFLOW:
            std::cout << "!!!OpenGL error in " << t << ": GL_STACK_UNDERFLOW" << std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cout << "!!!OpenGL error in " << t << ": GL_OUT_OF_MEMORY" << std::endl;
            break;
    }
    return errorCode;
}

std::string WideStringToString(std::wstring wideString)
{
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    std::string res = converterX.to_bytes(wideString);
    return res;
}

std::wstring ExtractDirectoryName(std::wstring s)
{
    int n = s.find_last_of('\\');
    return n > 0 ? s.substr(0, n) : L"";
}

bool KeyPressed(int vKey)
{
    short n = GetKeyState(vKey);
    return n < 0;
}
