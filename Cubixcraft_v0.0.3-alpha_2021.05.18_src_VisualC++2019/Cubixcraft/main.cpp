#include "main.h"
#include "block.h"
#include "chunk.h"
#include "CollisionBox.h"
#include "font.h"
#include "frustum.h"
#include "player.h"
#include "tessellator.h"
#include "timers.h"
#include "world.h"
#include "WorldRenderer.h"
#include "textures.h"
#include "ObjectSelector.h"
#include "utils.h"
//файл ресурсов
#include "resource.h"

const LPCWSTR MYCLASS = L"Cubixcraft_C++";
const LPCWSTR MYTITLE = L"Cubixcraft";

HINSTANCE myInstance;
HWND hMainWindow;
HWND hPanelRender;
HDC dcPanelRender;
HICON iconTree;
HGLRC dcGL;

int viewportWidth;
int viewportHeight;

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
CObjectSelector* objectSelector;
CFrustum* frustumObj;
bool bNeedToRecalculateFrustum = true;

GLuint textureFont = 0;

bool glInitialized;

std::wstring selfDir;

const LPCWSTR title = L"Cubixcraft 0.0.3-alpha | C++";





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
    if (GetClientRect(hPanelRender, &r)) 
    {
        oldMousePos.x = r.right / 2;
        oldMousePos.y = r.bottom / 2;
    }
    else 
    {
        oldMousePos.x = viewportWidth / 2;
        oldMousePos.y = viewportHeight / 2;
    }
    if (ClientToScreen(hPanelRender, &oldMousePos))
        SetCursorPos(oldMousePos.x, oldMousePos.y);
}

/*
 * ResizeViewport() 
 * Выполняется при изменении размера главного окна.
 */
void ResizeViewport(int newWidth, int newHeight)
{
    glViewport(0, 0, newWidth, newHeight);
    std::cout << "OpenGL viewport: " << newWidth << "x" << newHeight << std::endl;
    
    viewportWidth = newWidth;
    viewportHeight = newHeight;

    CameraChanged();
}

/*
 * DrawHudText()
 * Выводит текст поверх игрового экрана.
 * Для правильного вывода, необходимо отключить GL_LIGHTING и GL_DEPTH_TEST.
 */
void DrawHudText()
{
    glTextStart();

    std::string t = FormatString((char*)"FPS: %d", sceneFps);
    float x = viewportWidth - GetStringWidth(t);
    DrawTextBkg(x, 0.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), x, 0.0f, 0x00FF00);
    
    t = FormatString((char*)"Delta time: %f", deltaTime);
    x = viewportWidth - GetStringWidth(t);
    DrawTextBkg(x, 20.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), x, 20.0f, 0xFFFFFF);

    t = FormatString((char*)"Chunk updates: %d", chunkUpdatesPerSecond);
    x = viewportWidth - GetStringWidth(t);
    DrawTextBkg(x, 40.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), x, 40.0f, 0xFFFFFF);

    t = FormatString((char*)"Position: [%.2f %.2f %.2f]", player->positionX, player->positionY, player->positionZ);
    DrawTextBkg(0.0f, 0.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), 0.0f, 0.0f, 0xFFFFFF);

    t = FormatString((char*)"Rotation: [%.2f %.2f]", player->rotationYaw, player->rotationPitch);
    DrawTextBkg(0.0f, 20.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), 0.0f, 20.0f, 0xFFFFFF);

    std::string s = WideStringToString(world->GetName());
    t = FormatString((char*)"World: %s [%d,%d,%d]", s.c_str(), world->sizeX, world->sizeZ, world->sizeY);
    DrawTextBkg(0.0f, 40.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), 0.0f, 40.0f, 0xFFFFFF);
    
    CWorldRenderer* worldRenderer = world->GetRenderer();
    t = FormatString((char*)"Chunks rendered: %d / %d", worldRenderer->GetTotalChunksRendered(), worldRenderer->chunksCount);
    DrawTextBkg(0.0f, 60.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), 0.0f, 60.0f, 0xFFFFFF);

    t = FormatString((char*)"Blocks rendered: %d / %d", worldRenderer->GetTotalBlocksRendered(), worldRenderer->GetTotalBlocksPossible());
    DrawTextBkg(0.0f, 80.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), 0.0f, 80.0f, 0xFFFFFF);

    t = FormatString((char*)"Quads rendered: %d / %d", worldRenderer->GetTotalQuadsRendered(), worldRenderer->GetTotalQuadsPossible());
    DrawTextBkg(0.0f, 100.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), 0.0f, 100.0f, 0xFFFFFF);

    t = FormatString((char*)"Vertexes rendered: %d / %d", worldRenderer->GetTotalQuadsRendered() * 4, worldRenderer->GetTotalQuadsPossible() * 4);
    DrawTextBkg(0.0f, 120.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), 0.0f, 120.0f, 0xFFFFFF);

    if (objectSelector->GetHitsCount()) 
    {
        CBlock* block = world->GetBlock(objectSelector->hitResult->blockId);
        std::string blockName = WideStringToString(block->GetName());
        delete block;
        std::string blockSideName = CObjectSelector::SideIdToString(objectSelector->hitResult->blockSideId);
        t = FormatString((char*)"Looking at: [%d,%d,%d] %s, %s side",
            objectSelector->hitResult->x, objectSelector->hitResult->y, objectSelector->hitResult->z,
            blockName.c_str(), blockSideName.c_str());
        DrawTextBkg(0.0f, 140.0f, GetStringWidth(t), 0x0);
        glTextOut((char*)(t.c_str()), 0.0f, 140.0f, 0xFFFFFF);
    }
    else 
    {
        t = FormatString((char*)"Looking at: none");
        DrawTextBkg(0.0f, 140.0f, GetStringWidth(t), 0x0);
        glTextOut((char*)(t.c_str()), 0.0f, 140.0f, 0xFFFFFF);
    }

    t = "Press R to respawn a player";
    DrawTextBkg(0.0f, viewportHeight - 40.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), 0.0f, viewportHeight - 40.0f, 0xFFFFFF);

    t = "Press G to refresh chunks";
    DrawTextBkg(0.0f, viewportHeight - 20.0f, GetStringWidth(t), 0x0);
    glTextOut((char*)(t.c_str()), 0.0f, viewportHeight - 20.0f, 0xFFFFFF);

    glTextEnd();
}

/*
 * DrawHud()
 * Рисует игровой интерфейс.
 */
void DrawHud() 
{
    //Переключаемся в режим плоского экрана (ортогональная проекция)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //Последние два параметра определяют глубину (толщину) плоскости.
    //На самом деле, по факту, плоскость не является плоской.
    glOrtho(0.0, (double)viewportWidth, 0.0, (double)viewportHeight, -100.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Рисуем в HUD блок, который держит игрок.
    //Для этого необходимо, чтобы главный текстурный атлас
    //заранее был прикреплён вызовом функции glBindTexture().
    
    float blockPreviewSize = 50.0f;
    float blockPreviewPositionX = (float)viewportWidth - blockPreviewSize;
    float blockPreviewPositionY = (float)viewportHeight - 100.0f;

    //делаем так, чтобы блок вращался
    static float blockPreviewRotationAngle = 0.0f;
    blockPreviewRotationAngle += (float)(deltaTime * 100.0);
    while (blockPreviewRotationAngle >= 360.0f)
    {
        blockPreviewRotationAngle -= 360.0f;
    }

    //собственно, вращение блока
    glTranslatef(blockPreviewPositionX, blockPreviewPositionY, blockPreviewSize / 2.0f);
    glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(blockPreviewRotationAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(-blockPreviewPositionX, -blockPreviewPositionY, -(blockPreviewSize / 2.0f));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    CTessellator* t = new CTessellator();

    //Для того чтобы получить координаты текстуры блока, необходимо создать его экземпляр
    CBlock* block = world->CreateBlock(player->GetBlockIdInHand());
    for (int i = 0; i < 6; i++) 
    {
        RenderCubeSideHud(block, t, blockPreviewPositionX - blockPreviewSize / 2.0f,
            blockPreviewPositionY - blockPreviewSize / 2.0f, 0.0f, blockPreviewSize, i);
    }
    t->FlushTextureQuads();
    delete t;
    delete block;

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glLoadIdentity();

    //рисуем перекрестие в центре экрана
    float screenCenterX = (float)viewportWidth / 2.0f;
    float screenCenterY = (float)viewportHeight / 2.0f;

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(screenCenterX - 10.0f, screenCenterY        );
    glVertex2f(screenCenterX + 10.0f, screenCenterY        );
    glVertex2f(screenCenterX,         screenCenterY - 10.0f);
    glVertex2f(screenCenterX,         screenCenterY + 10.0f);
    glEnd();

    glEnable(GL_DEPTH_TEST);
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
    double alpha = (sin(GetTickCount64() / 200.0) * 0.6 + 0.65) * 0.5;

    //Здесь можно задать цвет RGB для подсветки
    glColor4d(1.0, 1.0, 1.0, alpha);

    CTessellator* t = new CTessellator();
    objectSelector->RenderHitCubeSide(t);
    t->FlushQuads();

    /*
     * Если позиция, на которую игрок может поставить новый блок,
     * не занята самим игроком, то рисуем на этом месте блок-призрак.
     * Для того чтобы нарисовать блок с нужной текстурой,
     * придётся создать дополнительный его экземпляр.
     */
    CBlock* block = world->CreateBlock(player->GetBlockIdInHand());
    block->SetPosition(objectSelector->hitResult->xNear,
        objectSelector->hitResult->yNear,
        objectSelector->hitResult->zNear);
    if (!block->collisionBox->Intersects(player->collisionBox))
    {
        //Делаем так, чтобы блок-призрак пульсировал медленнее
        //и немного менее прозрачно, чем подсветка стороны блока
        alpha = (sin(GetTickCount64() / 300.0) * 0.6 + 0.65) * 0.6;
        glColor4d(1.0, 1.0, 1.0, alpha);

        glEnable(GL_TEXTURE_2D);
        block->Render(t);
        t->FlushTextureQuads();
        glDisable(GL_TEXTURE_2D);
    }
    delete block;
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
    world->GetRenderer()->Render();

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
        DrawHudText();
    }
}







void MouseWheelHandler(MOUSE_WHEEL_DIRECTION wheelDirection)
{
    //при вращении колеса мыши, меняем блок в руках игрока
    switch (wheelDirection)
    {
        case MOUSE_WHEEL_DIRECTION::MW_UP:
        {
            char blockId = player->GetBlockIdInHand() - (char)1;
            if (blockId < (char)1)
            {
                blockId = (char)2;
            }
            player->SetBlockIdInHand(blockId);
            break;
        }

        case MOUSE_WHEEL_DIRECTION::MW_DOWN:
        {
            char blockId = player->GetBlockIdInHand() + (char)1;
            if (blockId > (char)2)
            {
                blockId = (char)1;
            }
            player->SetBlockIdInHand(blockId);
            break;
        }
    }
}


/*
 * EventHandler_BlockPlaced()
 * Уведомление об изменении блока в мире.
 * Приходит, когда игрок ставит новый блок.
 */
void EventHandler_BlockPlaced(int x, int y, int z, char newBlockId)
{
    CBlock* newBlock = world->CreateBlock(newBlockId);

    std::wcout << L"Placed a " << newBlock->GetName() << L" block to [" <<
        x << L"," << y << L"," << z << L"]" << std::endl;

    delete newBlock;
}

/*
 * EventHandler_BlockDestroyed()
 * Уведомление об удалении блока из мира.
 * Приходит, когда игрок ломает блок.
 */
void EventHandler_BlockDestroyed(int x, int y, int z, char wreckedBlockId)
{    
    CBlock* wreckedBlock = world->CreateBlock(wreckedBlockId);

    std::wcout << L"Wrecked a " << wreckedBlock->GetName() << " block at [" <<
        x << L"," << y << L"," << z << L"]" << std::endl;

    delete wreckedBlock;
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
void EventHandler_BlockChanged(int x, int y, int z, char newBlockId, char oldBlockId)
{
    CBlock* oldBlock = world->CreateBlock(oldBlockId);
    CBlock* newBlock = world->CreateBlock(newBlockId);
    
    std::wcout << L"Changed a block at [" << x << L"," << y << L"," << z << L"] from " <<
        oldBlock->GetName() << L" to " << newBlock->GetName() << std::endl;

    delete oldBlock;
    delete newBlock;
}











void LifeCycle()
{
    deltaTime = timerDelta->ElapsedTime();
    timerDelta->Reset();

    //определяем FPS (frames per second)
    //и количество изменившихся чанков в секунду.
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
    if (viewportWidth && viewportHeight)
    {
        RenderScene();

        //Проверяем, не было ли ошибок при рендеринге.
        //Если они были, то в консоль будет выведено сообщение.
        CheckOpenGlError("Render scene");

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

    int format = ChoosePixelFormat(dc, &pfd);
    return format ? SetPixelFormat(dc, format, &pfd) : false;
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
                            player->GetBlockIdInHand());
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
            if (mouseGrabbed && objectSelector->hitResult->blockId >= 0) 
            {
                world->DestroyBlock(objectSelector->hitResult->x, objectSelector->hitResult->y, objectSelector->hitResult->z);
            }
            break;

    }    
    return DefWindowProc(wnd, message, wParam, lParam);
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

            ResizeViewport(w, h);
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
                    world->GetRenderer()->SetAllChunksExpired();
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
            SetWindowLongPtr(hPanelRender, GWLP_WNDPROC, (long)&PanRenderProc);
            dcPanelRender = GetDC(hPanelRender);

            glInitialized = InitGl();
            if (!glInitialized)
            {
                std::cout << "InitGl() failed!" << std::endl;
                ReleaseDC(hPanelRender, dcPanelRender);
                MessageBox(0, L"InitGl() failed!", MYCLASS, MB_ICONERROR);
                DestroyWindow(wnd);
                return 0;
            }

            //получаем путь к директории, в которой находится программа.
            WCHAR p[256];
            GetModuleFileName(0, p, 256);
            selfDir = ExtractDirectoryName(p);

            LoadGameTextures();

            textureFont = BuildTexturedFont(WideStringToString(selfDir) + "\\textures\\font.png");
            if (!textureFont)
            {
                std::cout << "Can't load font.png!" << std::endl;
                MessageBox(wnd, L"Can't load font.png!", MYCLASS, MB_ICONERROR);
            }

            world = new CWorld(256, 256, 256, L"world1");
            world->SetSavingDirectory(selfDir + L"\\saves");

            //обработчики событий, происходящих в мире.
            world->onBlockPlaced = &EventHandler_BlockPlaced;
            world->onBlockChanged = &EventHandler_BlockChanged;
            world->onBlockDestroyed = &EventHandler_BlockDestroyed;

            if (!world->Load())
            {
                world->GenerateMap();
            }
            player = new CPlayer(world, 0.0f, (float)world->sizeY + 2.0f, 0.0f, 136.0f, 0.0f, 0.0f);
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
                delete objectSelector;
                delete frustumObj;

                FreeGameTextures();

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
    }
    return DefWindowProc(wnd, message, wParam, lParam);
}

int main() 
{
    std::cout << "Starting Cubixcraft third alpha... | Version 0.0.3-alpha" <<
        " | 2021.05.18 is the date of the 367 days anniversary of code refactoring!" << std::endl;

    myInstance = GetModuleHandle(NULL);

    HBRUSH brushBkg = CreateSolidBrush(0);

    WNDCLASSEX wndClassEx;
    ZeroMemory(&wndClassEx, sizeof(WNDCLASSEX));

    wndClassEx.cbSize        = sizeof(WNDCLASSEX);
    wndClassEx.style         = CS_HREDRAW | CS_VREDRAW;
    wndClassEx.lpfnWndProc   = WndProc;
    wndClassEx.hInstance     = myInstance;
    wndClassEx.hbrBackground = brushBkg;
    wndClassEx.lpszClassName = MYCLASS;

    if (!RegisterClassEx(&wndClassEx))
    {
        DeleteObject(brushBkg);
        std::cout << "ERROR: Can't create main window!" << std::endl;
        MessageBox(0, L"Can't create main window!", MYTITLE, MB_ICONERROR);
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

    hMainWindow = CreateWindow(wndClassEx.lpszClassName, title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        nLeft, nTop, nWidth, nHeight,
        NULL, NULL, myInstance, NULL);

    MSG msg;
    while (true) 
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
        {
            if (msg.message == WM_QUIT) 
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else 
        {
            LifeCycle();
        }
    }

    if (!UnregisterClass(wndClassEx.lpszClassName, wndClassEx.hInstance))
    {
        DeleteObject(brushBkg);
        std::cout << "ERROR: Can't unregister main window!" << std::endl;
        MessageBox(0, L"Unregistering main window failed!", MYTITLE, MB_ICONERROR);
    }

    return 0;
}
