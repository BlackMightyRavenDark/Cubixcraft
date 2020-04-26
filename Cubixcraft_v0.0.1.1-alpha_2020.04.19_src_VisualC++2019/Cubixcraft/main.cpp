#include "main.h"
#include "collisionbox.h"
#include "font.h"
#include "frustum.h"
#include "player.h"
#include "tesselator.h"
#include "timers.h"
#include "world.h"
#include "worldrenderer.h"
#include "textures.h"
#include "objectselector.h"
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
double deltaTime = 1.0;
Timer* timerDelta;
Timer* timerFps;
int sceneFps = 0; 

Player* player;
World* world;
WorldRenderer* worldRenderer;
ObjectSelector* objectSelector;
CFrustum* frustumObj;
bool bNeedToRecalculateFrustum = true;

GLuint textureFont = 0;

bool glInitialized;

const LPCWSTR title = L"Cubixcraft 0.0.1.1-alpha | C++";





void CameraChanged() {
    bNeedToRecalculateFrustum = true;
}

/* CenterMouseCursor()
 * Если вращение камеры управляется мышью,
 * то необходимо каждый раз возвращать курсор в центр экрана.
 */
void CenterMouseCursor() {
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
void ResizeGlWindow(int newWidth, int newHeight) {
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
void DrawGlText() {
    glTextStart();
    std::string t = FormatStr((char*)"FPS: %d", sceneFps);
    int x = glScreenWidth - t.length() * (int)symbolWidth - 6;
    DrawTextBkg(x, 0, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(x, 0, t.c_str(), 0x00FF00);
    
    t = FormatStr((char*)"Delta time: %f", deltaTime);
    x = glScreenWidth - t.length() * (int)symbolWidth - 6;
    DrawTextBkg(x, 20, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(x, 20, t.c_str(), 0xFFFFFF);

    t = FormatStr((char*)"Position: [%.2f %.2f %.2f]", player->positionX, player->positionY, player->positionZ);
    DrawTextBkg(0, 0, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 0, (char*)(t.c_str()), 0xFFFFFF);

    t = FormatStr((char*)"Rotation: [%.2f %.2f]", player->rotationYaw, player->rotationPitch);
    DrawTextBkg(0, 20, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 20, (char*)(t.c_str()), 0xFFFFFF);

    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    std::string s = converterX.to_bytes(world->GetName());
    t = FormatStr((char*)"World: %s [%d,%d,%d]", s.c_str(), world->sizeX, world->sizeZ, world->sizeY);
    DrawTextBkg(0, 40, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 40, t.c_str(), 0xFFFFFF);

    t = FormatStr((char*)"Chunks rendered: %d / %d", worldRenderer->chunksRendered, worldRenderer->chunksCount);
    DrawTextBkg(0, 60, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 60, t.c_str(), 0xFFFFFF);

    t = FormatStr((char*)"Blocks rendered: %d / %d", worldRenderer->totalBlocksRendered, worldRenderer->totalBlocksPossible);
    DrawTextBkg(0, 80, t.length() * (int)symbolWidth + 4, 0x0);
    glTextOut(0, 80, (char*)(t.c_str()), 0xFFFFFF);

    if (objectSelector->GetHitsCount()) {
        std::string s = objectSelector->SideIdToString(objectSelector->hitResult->blockSideId);
        t = FormatStr((char*)"Looks at: [%d,%d,%d] %s",
            objectSelector->hitResult->x,
            objectSelector->hitResult->y,
            objectSelector->hitResult->z,
            s.c_str());
        DrawTextBkg(0, 100, t.length() * (int)symbolWidth + 4, 0x0);
        glTextOut(0, 100, t.c_str(), 0xFFFFFF);
    }
    else {
        t = FormatStr((char*)"Looks at: none");
        DrawTextBkg(0, 100, t.length() * (int)symbolWidth + 4, 0x0);
        glTextOut(0, 100, t.c_str(), 0xFFFFFF);
    }
    glTextEnd();
}

/*
 * DrawHud()
 * Рисует игровой интерфейс.
 */
void DrawHud() {
    //Переключаемся в режим плоского экрана
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    //Последние два параметра определяют глубину (толщину) плоскости.
    //На самом деле, по факту, плоскость не является плоской.
    glOrtho(0.0, (double)glScreenWidth, 0.0, (double)glScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    int screenCenterX = glScreenWidth / 2;
    int screenCenterY = glScreenHeight / 2;

    //рисуем перекрестие в центре экрана
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f((GLfloat)(screenCenterX - 10.0f), (GLfloat)screenCenterY);
    glVertex2f((GLfloat)(screenCenterX + 10.0f), (GLfloat)screenCenterY);
    glVertex2f((GLfloat)screenCenterX,           (GLfloat)(screenCenterY - 10.0f));
    glVertex2f((GLfloat)screenCenterX,           (GLfloat)(screenCenterY + 10.0f));
    glEnd();

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/*
 * RenderHit()
 * Подсветка блока, на который смотрит игрок.
 */
void RenderHit() {
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //плавная пульсация альфа-канала
    double a = (sin(GetTickCount64() / 200.0) * 0.6 + 0.75) * 0.2;
    glColor4d(1.0, 1.0, 1.0, a);
    Tesselator* t = new Tesselator();
    objectSelector->RenderHitCubeSide(t);
    t->FlushQuads();
    delete t;
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
}

/*
 * RenderScene()
 * Отрисовка всего и вся.
 */
void RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //определяем, куда смотрит игрок
    int hits = objectSelector->PickObject(3.0f);

    //устанавливаем игровую камеру
    player->SetupCamera();

    //рендерим мир
    worldRenderer->Render();

    //если игрок смотрит на блок - подсвечиваем его
    if (hits) {
        RenderHit();
    }

    //рисуем игровой интерфейс
    DrawHud();

    //если текстура шрифта загружена, то выводим текст на экран
    if (textureFont) {
        DrawGlText();
    }
}







void LifeCicle() {
    deltaTime = timerDelta->ElapsedTime();
    timerDelta->Reset();

    //определяем FPS (frames per second)
    static int frames = 0;
    frames++;
    if (timerFps->ElapsedTime() >= 1.0) {
        sceneFps = frames;
        frames = 0;
        timerFps->Reset();
    }

    //Продвигаем жизненный цикл игрока.
    //В текущей версии обрабатывается только управление.
    player->Tick();

    //если размер окна не равен нулю, то рендерим сцену и выводим ее на экран
    if (glScreenWidth && glScreenHeight) {
        RenderScene();
        SwapBuffers(dcPanelRender);
    }
}







bool SetDCPixelFormat(HDC dc) {
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

bool InitGl() {
    if (!SetDCPixelFormat(dcPanelRender)) {
        std::cout << "ERROR: Setting the pixel format failed!" << std::endl;
        return false;
    }
    dcGL = wglCreateContext(dcPanelRender);
    if (dcGL) {
        wglMakeCurrent(dcPanelRender, dcGL);

        glClearColor(0.5f, 0.8f, 1.0f, 1.0f);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        return true;
    }
    
    return false;
}

LRESULT CALLBACK PanRenderProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {

        //нажатие левой кнопки мыши
        case WM_LBUTTONDOWN: {
            if (mouseGrabbed) {
                /*
                 * Если игрок смотрит на блок, то как минимум одна сторона 
                 * этого блока всегда свободна. С этой стороны игрок может
                 * поставить новый блок. Но сначала необходимо проверить,
                 * не стоит ли на этом месте сам игрок, чтобы не дать 
                 * поставить блок на самого себя.
                 */
                if (objectSelector->hitResult->blockId >= 0) {
                    CollisionBox* box = new CollisionBox(
                        (float)objectSelector->hitResult->xNear,
                        (float)objectSelector->hitResult->yNear,
                        (float)objectSelector->hitResult->zNear,
                        objectSelector->hitResult->xNear + world->blockWidth,
                        objectSelector->hitResult->yNear + world->blockHeight,
                        objectSelector->hitResult->zNear + world->blockWidth);
                    if (!player->collisionBox->Intersects(*box)) {
                        if (world->SetBlock(objectSelector->hitResult->xNear,
                                            objectSelector->hitResult->yNear,
                                            objectSelector->hitResult->zNear,
                                            1)) {
                            //если всё сошлось и блок поставлен, посылаем уведомление
                            worldRenderer->BlockChanged(objectSelector->hitResult->xNear,
                                                        objectSelector->hitResult->yNear,
                                                        objectSelector->hitResult->zNear);
                            std::cout << "Placed a block to [" << objectSelector->HitResultNearToString() << "]" << std::endl;
                        }
                    }
                    else {
                        std::cout << "Position [" << objectSelector->HitResultNearToString() << "] is blocked by a player" << std::endl;
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
                std::cout << "Mouse is grabbed" << std::endl;
            }
            break;
        }

        //нажатие правой кнопки мыши
        case WM_RBUTTONDOWN:
            if (mouseGrabbed) {
                if (objectSelector->hitResult->blockId >= 0) {
                    world->SetBlock(objectSelector->hitResult->x,
                                    objectSelector->hitResult->y,
                                    objectSelector->hitResult->z,
                                    0);
                    worldRenderer->BlockChanged(objectSelector->hitResult->x,
                                                objectSelector->hitResult->y,
                                                objectSelector->hitResult->z);
                    std::cout << "Wrecked a block in [" << objectSelector->hitResult->x << "," <<
                        objectSelector->hitResult->y << "," <<
                        objectSelector->hitResult->z << "]" << std::endl;
                }
            }
            break;

        default:
            return CallWindowProc((WNDPROC)oldPanelRenderProc, wnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {

        case WM_SIZE: {
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            MoveWindow(hPanelRender, 0, 0, w, h, false);
            std::cout << "Window size: " << w << "x" << h << std::endl;
            ResizeGlWindow(w, h);
            break;
        }

        case WM_KILLFOCUS:
            if (mouseGrabbed) {
                mouseGrabbed = false;
                ShowCursor(true);
                std::cout << "Focus was lost! Mouse is released" << std::endl;
            }
            break;

        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                case VK_RETURN:
                    if (mouseGrabbed) {
                        mouseGrabbed = false;
                        ShowCursor(true);
                        std::cout << "Mouse is released" << std::endl;
                    }
                    else {
                        DestroyWindow(wnd);
                    }
                    break;

                case VK_R:
                    std::cout << "Respawn a player" << std::endl;
                    player->Reset();
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
            if (!glInitialized) {
                std::cout << "InitGl() failed!" << std::endl;
                ReleaseDC(hPanelRender, dcPanelRender);
                MessageBox(0, L"InitGl() failed!", MYTITLE1, MB_ICONERROR);
                DestroyWindow(wnd);
                return 0;
            }

            textureFont = BuildTexturedFont((char*)"textures/font.png");
            if (!textureFont) {
                std::cout << "Can't load font.png!" << std::endl;
                MessageBox(wnd, L"Can't load font.png!", MYCLASS1, MB_ICONERROR);
            }

            LoadGameTextures();

            world = new World(256, 256, 64, L"world1");
            if (!world->Load()) {
                world->GenerateMap();
            }
            worldRenderer = new WorldRenderer(world);
            player = new Player(world, 0.0f, (float)(world->sizeZ + 1.0f), 0.0f, 136.0f, 0.0f, 0.0f);
            player->onCameraChanged = CameraChanged;
            objectSelector = new ObjectSelector(player);
            frustumObj = new CFrustum();
            timerFps = new Timer();
            timerDelta = new Timer();
            break;

        case WM_DESTROY:
            if (glInitialized) {
                delete timerDelta;
                delete timerFps;
                delete player;
                delete world;
                delete worldRenderer;
                delete objectSelector;
                delete frustumObj;

                if (textureFont) {
                    KillFont();
                }
                KillGameTextures();
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

int main() {
    std::cout << "Starting Cubixcraft 1,5th fixed alpha... " <<
        "| Version 0.0.1.1-alpha | 19.04.2020 is the day the cubes were respawned! AGAIN!!!" << std::endl;

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
    if (!RegisterClassEx(&wc)) {
        DeleteObject(brush1);
        std::cout << "ERROR: Can't create main window!" << std::endl;
        MessageBox(0, L"Can't create main window!", MYTITLE1, MB_ICONERROR);
        return 0;
    }

    int nLeft   = 0;
    int nTop    = 0;
    int nWidth  = 800;
    int nHeight = 600;
    RECT r;
    if (GetWindowRect(GetDesktopWindow(), &r)) {
        nLeft = r.right / 2 - nWidth / 2;
        nTop = r.bottom / 2 - nHeight / 2;
    }

    hMainWindow = CreateWindow(wc.lpszClassName, title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        nLeft, nTop, nWidth, nHeight,
        NULL, NULL, myInstance, NULL);

    MSG msg;
    while (true) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            LifeCicle();
        }
    }

    if (!UnregisterClass(wc.lpszClassName, wc.hInstance)) {
        DeleteObject(brush1);
        std::cout << "ERROR: Can't unregister main window!" << std::endl;
        MessageBox(0, L"Unregistering main window failed!", MYTITLE1, MB_ICONERROR);
    }

    return 0;
}
