#include "main.h"
#include "block.h"
#include "chunk.h"
#include "collisionbox.h"
#include "font.h"
#include "frustum.h"
#include "player.h"
#include "tessellator.h"
#include "timers.h"
#include "world.h"
#include "worldrenderer.h"
#include "textures.h"
#include "objectselector.h"
#include "utils.h"
#include "resource.h"

const LPCWSTR MYCLASS1 = L"Cubixcraft_C++";
const LPCWSTR MYTITLE1 = L"Cubixcraft";

HINSTANCE myInstance;
HWND hMainWindow;
HWND hPanelRender;
long oldPanelRenderProc;
HDC dcPanelRender;
HICON iconTree;
HGLRC dcGL;

int glScreenWidth;
int glScreenHeight;

POINT oldMousePos;
bool mouseGrabbed = false;

//time and FPS calculation
double deltaTime = 0.0;
CTimer* timerDelta;
CTimer* timerFps;
int sceneFps = 0;
int chunkUpdatesPerSecond = 0;

CPlayer* player;
CWorld* world;
CWorldRenderer* worldRenderer;
CObjectSelector* objectSelector;
CFrustum* frustumObj;
bool bNeedToRecalculateFrustum = true;

GLuint textureFont = 0;

bool glInitialized;

std::wstring selfDir;

const LPCWSTR title = L"Cubixcraft 0.0.2-alpha | C++";





void CameraChanged() 
{
    bNeedToRecalculateFrustum = true;
}

/* CenterMouseCursor()
 * Если вращение камеры управляется мышью,
 * то необходимо каждый раз возвращать курсор в центр экрана.
 */
void CenterMouseCursor() 
{
    RECT r;
    if (GetClientRect(hPanelRender, &r)) {
        oldMousePos.x = r.right / 2;
        oldMousePos.y = r.bottom / 2;
    }
    else {
        oldMousePos.x = glScreenWidth / 2;
        oldMousePos.y = glScreenHeight / 2;
    }
    ClientToScreen(hPanelRender, &oldMousePos);
    SetCursorPos(oldMousePos.x, oldMousePos.y);
}

/*
 * ResizeGlWindow() 
 * Выполняется при изменении размеров главного окна.
 */
void ResizeGlWindow(int newWidth, int newHeight) 
{
    glViewport(0, 0, newWidth, newHeight);
    std::cout << "OpenGL viewport: " << newWidth << "x" << newHeight << std::endl;
    glScreenWidth = newWidth;
    glScreenHeight = newHeight;
    CameraChanged();
}

/*
 * DrawGlText()
 * Выводит текст поверх игрового экрана.
 * Для правильного вывода, необходимо отключить GL_LIGHTING и GL_DEPTH_TEST.
 */
void DrawGlText() 
{
    glTextStart();
    std::string t = FormatStr((char*)"FPS: %d", sceneFps);
    int x = glScreenWidth - t.length() * (int)symbolWidth - 6;
    DrawTextBkg(x, 0, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(x, 0, t.c_str(), 0x00FF00);
    
    t = FormatStr((char*)"Delta time: %f", deltaTime);
    x = glScreenWidth - t.length() * (int)symbolWidth - 6;
    DrawTextBkg(x, 20, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(x, 20, t.c_str(), 0xFFFFFF);

    t = FormatStr((char*)"Chunk updates: %d", chunkUpdatesPerSecond);
    x = glScreenWidth - t.length() * (int)symbolWidth - 6;
    DrawTextBkg(x, 40, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(x, 40, t.c_str(), 0xFFFFFF);

    t = FormatStr((char*)"Position: [%.2f %.2f %.2f]", player->positionX, player->positionY, player->positionZ);
    DrawTextBkg(0, 0, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 0, (char*)(t.c_str()), 0xFFFFFF);

    t = FormatStr((char*)"Rotation: [%.2f %.2f]", player->rotationYaw, player->rotationPitch);
    DrawTextBkg(0, 20, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 20, (char*)(t.c_str()), 0xFFFFFF);

    std::string s = WideStringToString(world->GetName());
    t = FormatStr((char*)"World: %s [%d,%d,%d]", s.c_str(), world->sizeX, world->sizeZ, world->sizeY);
    DrawTextBkg(0, 40, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 40, t.c_str(), 0xFFFFFF);
    
    t = FormatStr((char*)"Chunks rendered: %d / %d", worldRenderer->GetTotalChunksRendered(), worldRenderer->chunksCount);
    DrawTextBkg(0, 60, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 60, t.c_str(), 0xFFFFFF);

    t = FormatStr((char*)"Blocks rendered: %d / %d", worldRenderer->GetTotalBlocksRendered(), worldRenderer->GetTotalBlocksPossible());
    DrawTextBkg(0, 80, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 80, (char*)(t.c_str()), 0xFFFFFF);

    t = FormatStr((char*)"Quads rendered: %d / %d", worldRenderer->GetTotalQuadsRendered(), worldRenderer->GetTotalQuadsPossible());
    DrawTextBkg(0, 100, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 100, (char*)(t.c_str()), 0xFFFFFF);

    t = FormatStr((char*)"Vertexes rendered: %d / %d", worldRenderer->GetTotalQuadsRendered() * 4, worldRenderer->GetTotalQuadsPossible() * 4);
    DrawTextBkg(0, 120, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 120, (char*)(t.c_str()), 0xFFFFFF);

    if (objectSelector->GetHitsCount()) 
    {
        std::string s = objectSelector->SideIdToString(objectSelector->hitResult->blockSideId);
        std::string worldName = WideStringToString(world->GetBlock(objectSelector->hitResult->blockId)->GetName());
        t = FormatStr((char*)"Looks at: [%d,%d,%d] %s, %s side",
            objectSelector->hitResult->x, objectSelector->hitResult->y, objectSelector->hitResult->z,
            worldName.c_str(), s.c_str());
        DrawTextBkg(0, 140, t.length() * (int)symbolWidth + 4, 0x0);
        glTextOut(0, 140, t.c_str(), 0xFFFFFF);
    }
    else 
    {
        t = FormatStr((char*)"Looks at: none");
        DrawTextBkg(0, 140, t.length() * (int)symbolWidth + 4, 0x0);
        glTextOut(0, 140, t.c_str(), 0xFFFFFF);
    }

    t = "Press R to respawn a player";
    DrawTextBkg(0, glScreenHeight - 40, t.length() * (int)symbolWidth, 0x0);
    glTextOut(0, glScreenHeight - 40, t.c_str(), 0xFFFFFF);

    t = "Press G to refresh chunks";
    DrawTextBkg(0, glScreenHeight - 20, t.length() * (int)symbolWidth, 0x0);
    glTextOut(0, glScreenHeight - 20, t.c_str(), 0xFFFFFF);

    glTextEnd();
}

/*
 * DrawHud()
 * Рисует игровой интерфейс.
 */
void DrawHud() 
{
    //Переключаемся в режим плоского экрана
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    //Последние два параметра определяют глубину (толщину) плоскости.
    //На самом деле, по факту, плоскость не является плоской.
    glOrtho(0.0, (double)glScreenWidth, 0.0, (double)glScreenHeight, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    //Рисуем в HUD блок, который держит игрок
    float blockPreviewSize = 50.0f;
    float blockPreviewPositionX = (float)glScreenWidth - blockPreviewSize;
    float blockPreviewPositionY = (float)glScreenHeight - 100.0f;

    static float blockPreviewRotationAngle = 0.0f;
    blockPreviewRotationAngle += (float)(deltaTime * 100.0);
    while (blockPreviewRotationAngle >= 360.0f)
    {
        blockPreviewRotationAngle -= 360.0f;
    }

    //Вращение блока в HUD
    glTranslatef(blockPreviewPositionX, blockPreviewPositionY, blockPreviewSize / 2.0f);
    glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(blockPreviewRotationAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(-blockPreviewPositionX, -blockPreviewPositionY, -(blockPreviewSize / 2.0f));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    CTessellator* t = new CTessellator();

    //Для того чтобы получить координаты текстуры блока, необходимо создать его экземпляр
    CBlock* b = world->CreateBlock(player->blockIdInHand);
    for (int i = 0; i < 6; i++) 
    {
        RenderCubeSide2D(b, t, blockPreviewPositionX - blockPreviewSize / 2.0f, 
            blockPreviewPositionY - blockPreviewSize / 2.0f, 0.0f, blockPreviewSize, i);
    }
    t->FlushTextureQuads();
    delete t;
    delete b;

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glLoadIdentity();

    float screenCenterX = glScreenWidth / 2.0f;
    float screenCenterY = glScreenHeight / 2.0f;

    //рисуем перекрестие в центре экрана
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(screenCenterX - 10.0f, screenCenterY);
    glVertex2f(screenCenterX + 10.0f, screenCenterY);
    glVertex2f(screenCenterX,         screenCenterY - 10.0f);
    glVertex2f(screenCenterX,         screenCenterY + 10.0f);
    glEnd();

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glLoadIdentity();
}

/*
 * RenderHitResult()
 * Подсветка блока, на который смотрит игрок.
 */
void RenderHitResult()
{
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //плавная пульсация альфа-канала для подсветки стороны блока
    double a = (sin(GetTickCount() / 200.0) * 0.6 + 0.65) * 0.5;

    //Здесь можно задать цвет RGB для подсветки
    glColor4d(1.0, 1.0, 1.0, a);

    CTessellator* t = new CTessellator();
    objectSelector->RenderHitCubeSide(t);
    t->FlushQuads();

    /*
     * Если позиция, на которую игрок может поставить новый блок,
     * не занята самим игроком, то рисуем на этом месте блок-призрак.
     * Для того чтобы нарисовать блок с нужной текстурой,
     * придётся создать дополнительный его экземпляр.
     */
    CBlock* b = world->CreateBlock(player->blockIdInHand);
    b->SetPosition(objectSelector->hitResult->xNear,
        objectSelector->hitResult->yNear,
        objectSelector->hitResult->zNear);
    if (!b->collisionBox->Intersects(player->collisionBox)) 
    {
        //Делаем так, чтобы блок-призрак пульсировал медленнее
        //и немного менее прозрачно, чем подсветка стороны блока
        a = (sin(GetTickCount() / 300.0) * 0.6 + 0.65) * 0.6;
        glColor4d(1.0, 1.0, 1.0, a);

        glEnable(GL_TEXTURE_2D);
        b->Render(t);
        t->FlushTextureQuads();
        glDisable(GL_TEXTURE_2D);
    }
    delete b;
    delete t;

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
}

/*
 * RenderScene()
 * Отрисовка всего и вся.
 */
void RenderScene() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //определяем, куда смотрит игрок
    int hits = objectSelector->PickObject(3.0f);

    //устанавливаем игровую камеру
    player->SetupCamera();

    //рендерим мир
    worldRenderer->Render();

    //если игрок смотрит на блок - подсвечиваем его
    if (hits) 
    {
        RenderHitResult();
    }

    //рисуем игровой интерфейс
    DrawHud();

    //если текстура шрифта загружена, то выводим текст на экран
    if (textureFont) 
    {
        DrawGlText();
    }
}







void MouseWheelHandler(MOUSE_WHEEL_DIRECTION wheelDirection)
{
    //при вращении колеса мыши, меняем блок в руках игрока
    switch (wheelDirection)
    {
        case MOUSE_WHEEL_DIRECTION::MW_UP:
            player->blockIdInHand--;
            if (player->blockIdInHand < 1)
            {
                player->blockIdInHand = 2;
            }
            break;

        case MOUSE_WHEEL_DIRECTION::MW_DOWN:
            player->blockIdInHand++;
            if (player->blockIdInHand > 2)
            {
                player->blockIdInHand = 1;
            }
            break;
    }
}


/*
 * EventHandler_BlockPlaced()
 * Уведомление об изменении блока в мире.
 * Приходит, когда игрок ставит новый блок.
 */
void EventHandler_BlockPlaced(CBlock* newBlock)
{
    int x = newBlock->GetPositionX();
    int y = newBlock->GetPositionY();
    int z = newBlock->GetPositionZ();
    std::wcout << L"Placed a " << newBlock->GetName() << L" block to [" <<
        x << L"," << y << L"," << z << L"]" << std::endl;
}

/*
 * EventHandler_BlockDestroy()
 * Уведомление об удалении блока из мира.
 * Приходит, когда игрок ломает блок.
 */
void EventHandler_BlockDestroy(CBlock* wreckedBlock)
{    
    int x = wreckedBlock->GetPositionX();
    int y = wreckedBlock->GetPositionY();
    int z = wreckedBlock->GetPositionZ();
    std::wcout << L"Wrecked a " << wreckedBlock->GetName() << " block at [" <<
        x << L"," << y << L"," << z << L"]" << std::endl;
}

/*
 * EventHandler_BlockChanged()
 * Уведомление об изменении блока в мире.
 * Приходит, когда в мире меняется блок,
 * если игрок его не построил и не разрушил.
 * Например, это происходит, если поставить
 * какой-нибудь блок на блок травы.
 * Тогда блок травы меняется на блок земли.
 */
void EventHandler_BlockChanged(CBlock* oldBlock)
{
    int x = oldBlock->GetPositionX();
    int y = oldBlock->GetPositionY();
    int z = oldBlock->GetPositionZ();
    std::wcout << L"Changed a block at [" << x << L"," << y << L"," << z << L"] from " <<
        oldBlock->GetName() << L" to " << world->GetBlock(x, y, z)->GetName() << std::endl;
}











void LifeCicle()
{
    deltaTime = timerDelta->ElapsedTime();
    timerDelta->Reset();

    //определяем FPS (frames per second)
    //и количество изменившихся чанков за секунду.
    static int frames = 0;
    frames++;
    if (timerFps->ElapsedTime() >= 1.0)
    {
        sceneFps = frames;
        chunkUpdatesPerSecond = totalChunkUpdates;

        //сбрасываем счётчики
        totalChunkUpdates = 0;
        frames = 0;
        timerFps->Reset();
    }

    //Продвигаем жизненный цикл игрока.
    //В текущей версии обрабатывается только управление.
    player->Tick();

    //если размер окна не равен нулю, то рендерим сцену и выводим ее на экран
    if (glScreenWidth && glScreenHeight)
    {
        RenderScene();

        //Проверяем, не было ли ошибок при рендеринге.
        //Если они были, то в консоль будет выведено сообщение.
        CheckOpenGlError("render scene");

        SwapBuffers(dcPanelRender);
    }
}







bool SetDCPixelFormat(HDC dc) 
{
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int n = ChoosePixelFormat(dc, &pfd);
    return SetPixelFormat(dc, n, &pfd);
}

bool InitGl() 
{
    if (!SetDCPixelFormat(dcPanelRender)) 
    {
        std::cout << "ERROR: Setting the pixel format failed!" << std::endl;
        return false;
    }
    dcGL = wglCreateContext(dcPanelRender);
    if (dcGL) 
    {
        wglMakeCurrent(dcPanelRender, dcGL);

        glClearColor(0.5f, 0.8f, 1.0f, 1.0f);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        return true;
    }
    
    return false;
}

LRESULT CALLBACK PanRenderProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) 
    {
        case WM_MOUSEWHEEL:
        {
            int wheel = GET_WHEEL_DELTA_WPARAM(wParam);
            MouseWheelHandler(wheel >= 0 ? MOUSE_WHEEL_DIRECTION::MW_UP : MOUSE_WHEEL_DIRECTION::MW_DOWN);
            break;
        }

        //нажатие левой кнопки мыши
        case WM_LBUTTONDOWN: 
        {
            if (mouseGrabbed) 
            {
                /*
                 * Если игрок смотрит на блок, то как минимум одна сторона 
                 * этого блока всегда свободна. С этой стороны игрок может
                 * поставить новый блок. Но сначала необходимо проверить,
                 * не стоит ли на этом месте сам игрок, чтобы не дать 
                 * поставить блок на самого себя.
                 */
                if (objectSelector->hitResult->blockId >= 0) 
                {
                    CCollisionBox* box = new CCollisionBox(
                        (float)objectSelector->hitResult->xNear,
                        (float)objectSelector->hitResult->yNear,
                        (float)objectSelector->hitResult->zNear,
                        objectSelector->hitResult->xNear + world->blockWidth,
                        objectSelector->hitResult->yNear + world->blockHeight,
                        objectSelector->hitResult->zNear + world->blockWidth);
                    if (!player->collisionBox->Intersects(box)) 
                    {
                        world->SetBlock(objectSelector->hitResult->xNear,
                            objectSelector->hitResult->yNear,
                            objectSelector->hitResult->zNear,
                            world->CreateBlock(player->blockIdInHand));
                    }
                    else
                    {
                        std::cout << "Position [" << objectSelector->HitResultNearToString() << 
                            "] is blocked by a player." << std::endl;
                    }
                    delete box;
                }
            }
            else 
            {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                std::cout << "Mouse left click: [" << x << "," << y << "]" << std::endl;
                ShowCursor(false);
                CenterMouseCursor();
                mouseGrabbed = true;
                std::cout << "Mouse is grabbed." << std::endl;
            }
            break;
        }
        
        //нажатие правой кнопки мыши
        case WM_RBUTTONDOWN:
            if (mouseGrabbed) 
            {
                if (objectSelector->hitResult->blockId >= 0) 
                {
                    world->DestroyBlock(objectSelector->hitResult->x, objectSelector->hitResult->y, objectSelector->hitResult->z);
                }
            }
            break;

        default:
            return CallWindowProc((WNDPROC)oldPanelRenderProc, wnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
        case WM_SIZE: 
        {
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            MoveWindow(hPanelRender, 0, 0, w, h, false);
            std::cout << "Window size: " << w << "x" << h << std::endl;
            ResizeGlWindow(w, h);
            break;
        }

        case WM_KILLFOCUS:
            if (mouseGrabbed) 
            {
                mouseGrabbed = false;
                ShowCursor(true);
                std::cout << "Window focus was lost! Mouse is released." << std::endl;
            }
            break;

        case WM_KEYDOWN:
            switch (wParam) 
            {
                case VK_ESCAPE:
                case VK_RETURN:
                    if (mouseGrabbed) 
                    {
                        mouseGrabbed = false;
                        ShowCursor(true);
                        std::cout << "Mouse is released." << std::endl;
                    }
                    else 
                    {
                        DestroyWindow(wnd);
                    }
                    break;

                case VK_G:
                    std::cout << "Refreshing chunks..." << std::endl;
                    worldRenderer->SetAllChunksExpired();
                    break;

                case VK_R:
                    player->Reset();
                    std::cout << "Player is respawned." << std::endl;
                    break;
            }
            break;

        case WM_CREATE:
            iconTree = (HICON)LoadIcon(myInstance, MAKEINTRESOURCE(IDI_MY_ICON_TREE));
            SendMessage(wnd, WM_SETICON, ICON_SMALL, (LPARAM)iconTree);
            SendMessage(wnd, WM_SETICON, ICON_BIG, (LPARAM)iconTree); 

            hPanelRender = CreateWindow(L"Static", NULL,
                WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, 0, 0, 0, 0,
                wnd, NULL, NULL, NULL);
            oldPanelRenderProc = (long)SetWindowLongPtr(hPanelRender, GWLP_WNDPROC, (long)&PanRenderProc);
            dcPanelRender = GetDC(hPanelRender);

            glInitialized = InitGl();
            if (!glInitialized) 
            {
                std::cout << "InitGl() failed!" << std::endl;
                ReleaseDC(hPanelRender, dcPanelRender);
                MessageBox(0, L"InitGl() failed!", MYTITLE1, MB_ICONERROR);
                DestroyWindow(wnd);
                return 0;
            }

            WCHAR p[256];
            GetModuleFileName(0, p, 256);
            selfDir = ExtractDirectoryName(p);
            
            LoadGameTextures();

            textureFont = BuildTexturedFont(WideStringToString(selfDir) + "\\textures\\font.png");
            if (!textureFont) 
            {
                std::cout << "Can't load font.png!" << std::endl;
                MessageBox(wnd, L"Can't load font.png!", MYCLASS1, MB_ICONERROR);
            }

            world = new CWorld(256, 256, 64, L"world1");
            world->SetSavingDirectory(selfDir + L"\\saves");
            world->onBlockPlaced = &EventHandler_BlockPlaced;
            world->onBlockChanged = &EventHandler_BlockChanged;
            world->onBlockDestroyed = &EventHandler_BlockDestroy;
            if (!world->Load()) 
            {
                world->GenerateMap();
            }
            worldRenderer = new CWorldRenderer(world);
            player = new CPlayer(world, 0.0f, (float)(world->sizeZ + 2.0f), 0.0f, 136.0f, 0.0f, 0.0f);
            player->onCameraChanged = CameraChanged;
            objectSelector = new CObjectSelector(player);
            frustumObj = new CFrustum();
            timerFps = new CTimer();
            timerDelta = new CTimer();
            break;

        case WM_DESTROY:
            if (glInitialized) 
            {
                delete timerDelta;
                delete timerFps;
                delete player;
                delete world;
                delete worldRenderer;
                delete objectSelector;
                delete frustumObj;

                KillGameTextures();
                if (textureFont) 
                {
                    KillFont();
                }

                wglMakeCurrent(dcPanelRender, 0);
                wglDeleteContext(dcGL);

                ReleaseDC(hPanelRender, dcPanelRender);
            }
            DestroyIcon(iconTree);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(wnd, message, wParam, lParam);
    }
    return 0;
}

int main() 
{
    std::cout << "Starting Cubixcraft second alpha... " <<
        "| Version 0.0.2-alpha | 16.05.2020 is the date when dirt came to the Earth!" << std::endl;

    myInstance = GetModuleHandle(NULL);

    HBRUSH brush1 = CreateSolidBrush(0);

    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = myInstance;
    wc.hIcon         = NULL;
    wc.hIconSm       = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = brush1;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = MYCLASS1;
    if (!RegisterClassEx(&wc)) 
    {
        DeleteObject(brush1);
        std::cout << "ERROR: Can't create main window!" << std::endl;
        MessageBox(0, L"Can't create main window!", MYTITLE1, MB_ICONERROR);
        return 0;
    }

    int nLeft   = 0;
    int nTop    = 0;
    int nWidth  = 640;
    int nHeight = 480;
    RECT r;
    if (GetWindowRect(GetDesktopWindow(), &r)) 
    {
        nLeft = r.right / 2 - nWidth / 2;
        nTop = r.bottom / 2 - nHeight / 2;
    }

    hMainWindow = CreateWindow(wc.lpszClassName, title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        nLeft, nTop, nWidth, nHeight,
        NULL, NULL, myInstance, NULL);

    MSG msg;
    while (true) 
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
        {
            if (msg.message == WM_QUIT) 
            {
                break;
            }
            else 
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else 
        {
            LifeCicle();
        }
    }

    if (!UnregisterClass(wc.lpszClassName, wc.hInstance)) 
    {
        DeleteObject(brush1);
        std::cout << "ERROR: Can't unregister main window!" << std::endl;
        MessageBox(0, L"Unregistering main window failed!", MYTITLE1, MB_ICONERROR);
    }

    return 0;
}
