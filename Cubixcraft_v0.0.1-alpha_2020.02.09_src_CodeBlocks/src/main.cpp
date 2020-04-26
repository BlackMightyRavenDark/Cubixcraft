#include "main.h"
#include "font.h"
#include "player.h"
#include "timers.h"
#include "world.h"
#include "worldrenderer.h"
#include "frustum.h"
#include "textures.h"
#include "objectselector.h"

#define MYCLASS1 "Cubixcraft_C++"
#define MYTITLE1 "Cubixcraft"

HINSTANCE myInstance;
HWND hMainWindow;
HWND hPanelRender;
long oldPanelRenderProc;
HDC dcPanelRender;
HGLRC dcGL;

int glScreenWidth;
int glScreenHeight;

POINT oldMousePos;
bool mouseGrabbed = false;


//time and FPS calculation
double deltaTime = 0.0;
Timer* timerDelta;
Timer* timerFps;
GameTimer* gameTimer;
int sceneFps = 0;


Player* player;
World* world;
WorldRenderer* worldRenderer;
ObjectSelector* objectSelector;
CFrustum* frustumObj;
bool bNeedToRecalculateFrustum = true;

GLuint textureFont = 0;

const char* title = (char*)"Cubixcraft 0.0.1-alpha  | 09.02.2020";





void CameraChanged() {
//	std::cout << "Camera changed" << std::endl;
    bNeedToRecalculateFrustum = true;
}

void CenterMouseCursor(){
    RECT r;
    if (GetClientRect(hPanelRender, &r)){
        oldMousePos.x = r.right / 2;
        oldMousePos.y = r.bottom / 2;
    } else {
        oldMousePos.x = glScreenWidth / 2;
        oldMousePos.y = glScreenHeight / 2;
    }
    ClientToScreen(hPanelRender, &oldMousePos);
    SetCursorPos(oldMousePos.x, oldMousePos.y);
}

void ResizeGlWindow(int newWidth, int newHeight){
    glViewport(0,0, newWidth, newHeight);
    std::cout << "OpenGL viewport: " << newWidth << "x" << newHeight << std::endl;
    glScreenWidth = newWidth;
    glScreenHeight = newHeight;
    CameraChanged();
}

void DrawGlText(){
    glTextStart();
	std::string t = FormatStr((char*)"FPS: %d", sceneFps);
	int x = glScreenWidth - t.length() * symbolWidth - 6;
	DrawTextBkg(x, 0, t.length() * symbolWidth + 4, 0x0);
	glTextOut(x, 0, t.c_str(), 0x00FF00);

	t = FormatStr((char*)"Delta time: %f", deltaTime);
	x = glScreenWidth - t.length() * symbolWidth - 6;
	DrawTextBkg(x, 20, t.length() * symbolWidth + 4, 0x0);
	glTextOut(x, 20, t.c_str(), 0xFFFFFF);

	t = FormatStr((char*)"Position: [%.2f %.2f %.2f]", player->positionX, player->positionY, player->positionZ);
	DrawTextBkg(0, 0, t.length() * symbolWidth + 4, 0x0);
	glTextOut(0, 0, t.c_str(), 0xFFFFFF);

	t = FormatStr((char*)"Rotation: [%.2f %.2f]", player->rotationYaw, player->rotationPitch);
	DrawTextBkg(0, 20, t.length() * symbolWidth + 4, 0x0);
	glTextOut(0, 20, t.c_str(), 0xFFFFFF);

	t = FormatStr((char*)"World: %s [%d,%d,%d]", world->GetName().c_str(), world->sizeX, world->sizeZ, world->sizeY);
	DrawTextBkg(0, 40, t.length() * symbolWidth + 4, 0x0);
	glTextOut(0, 40, t.c_str(), 0xFFFFFF);

	t = FormatStr((char*)"Chunks rendered: %d / %d", worldRenderer->chunksRendered, worldRenderer->chunksCount);
	DrawTextBkg(0, 60, t.length() * symbolWidth + 4, 0x0);
	glTextOut(0, 60, t.c_str(), 0xFFFFFF);

	t = FormatStr((char*)"Blocks rendered: %d / %d", worldRenderer->totalBlocksRendered, worldRenderer->totalBlocksPossible);
	DrawTextBkg(0, 80, t.length() * symbolWidth + 4, 0x0);
	glTextOut(0, 80, t.c_str(), 0xFFFFFF);

	if (objectSelector->GetHitsCount()){
        std::string s = objectSelector->SideIdToString(objectSelector->hitResult->blockSideId);
        t = FormatStr((char*)"View at: [%d,%d,%d] %s",
                  objectSelector->hitResult->x,
                  objectSelector->hitResult->y,
                  objectSelector->hitResult->z,
                  s.c_str());
        DrawTextBkg(0, 100, t.length() * symbolWidth + 4, 0x0);
        glTextOut(0, 100, t.c_str(), 0xFFFFFF);
	} else {
        t = FormatStr((char*)"View at: none");
        DrawTextBkg(0, 100, t.length() * symbolWidth + 4, 0x0);
        glTextOut(0, 100, t.c_str(), 0xFFFFFF);
	}
	glTextEnd();
}

void DrawHud(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)glScreenWidth, 0.0, (double)glScreenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    int screenCenterX = glScreenWidth / 2;
    int screenCenterY = glScreenHeight / 2;

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(screenCenterX - 10.0, screenCenterY);
    glVertex2f(screenCenterX + 10.0, screenCenterY);
    glVertex2f(screenCenterX, screenCenterY - 10.0);
    glVertex2f(screenCenterX, screenCenterY + 10.0);
    glEnd();

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void RenderHit(){
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    double a = (sin(GetTickCount() / 200.0) * 0.6 + 0.65) * 0.79;
    glColor4d(1.0, 1.0, 1.0, a);
    Tesselator* t = new Tesselator();
    objectSelector->RenderHitSide(t);
    t->FlushQuads();
    delete t;
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
}

void RenderScene(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int hits = objectSelector->PickObject(3.0);

    player->SetupCamera();

    worldRenderer->Render();

    if (hits){
        RenderHit();
    }

    DrawHud();

    if (textureFont){
        DrawGlText();
    }
}






void LifeCicle(){
    deltaTime = timerDelta->ElapsedTime();

    timerDelta->Reset();

    static int frames = 0;
    frames++;
    if (timerFps->ElapsedTime() >= 1.0){
        sceneFps = frames;
        frames = 0;
        timerFps->Reset();
    }

    gameTimer->AdvanceTime();

    for (int i = 0; i < gameTimer->ticks; ++i){
        player->Tick();
    }

    if (glScreenWidth && glScreenHeight){
        RenderScene();
        SwapBuffers(dcPanelRender);
    }
}







void SetDCPixelFormat(HDC dc){
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
    SetPixelFormat(dc, n, &pfd);
}

bool InitGl(){
    SetDCPixelFormat(dcPanelRender);
    dcGL = wglCreateContext(dcPanelRender);
    if (dcGL){
        wglMakeCurrent(dcPanelRender, dcGL);

        glClearColor(0.5, 0.8, 1.0, 0.0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        return true;
    }
    return false;
}

LRESULT CALLBACK PanRenderProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
    case WM_LBUTTONDOWN:{
        if (mouseGrabbed){
            if (objectSelector->hitResult->blockId >= 0){
                CollisionBox* box = new CollisionBox(
                                 objectSelector->hitResult->xNear,
                                 objectSelector->hitResult->yNear,
                                 objectSelector->hitResult->zNear,
                                 objectSelector->hitResult->xNear + world->blockWidth,
                                 objectSelector->hitResult->yNear + world->blockHeight,
                                 objectSelector->hitResult->zNear + world->blockWidth);
                if (!player->collisionBox->Intersects(*box)){
                    if (world->SetBlock(objectSelector->hitResult->xNear,
                                        objectSelector->hitResult->yNear,
                                        objectSelector->hitResult->zNear,
                                        textureGrass)){
                        worldRenderer->BlockChanged(objectSelector->hitResult->xNear,
                                                    objectSelector->hitResult->yNear,
                                                    objectSelector->hitResult->zNear);
                        std::cout << "Placed a block to [" << objectSelector->HitResultNearToString() << "]" << std::endl;
                    }
                } else {
                    std::cout << "Position [" << objectSelector->HitResultNearToString() << "] is blocked by a player" << std::endl;
                }
                delete box;
            }

        } else {
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

    case WM_RBUTTONDOWN:
        if (mouseGrabbed){
            if (objectSelector->hitResult->blockId >= 0){
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

LRESULT CALLBACK WndProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){

        case WM_SIZE:{
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            MoveWindow(hPanelRender, 0,0, w,h, false);
            std::cout << "Window size: " << w << "x" << h << std::endl;
            ResizeGlWindow(w, h);
            break;
        }

        case WM_KILLFOCUS:
            if (mouseGrabbed){
                mouseGrabbed = false;
                ShowCursor(true);
                std::cout << "Focus was lost! Mouse is released" << std::endl;
            }
            break;

        case WM_KEYDOWN:
            switch (wParam){
                case VK_ESCAPE:
                case VK_RETURN:
                    if (mouseGrabbed){
                        mouseGrabbed = false;
                        ShowCursor(true);
                        std::cout << "Mouse is released" << std::endl;
                    } else {
                        DestroyWindow(wnd);
                    }
                    break;

                case VK_R:
                    std::cout << "Reset camera" << std::endl;
                    player->Reset();
                    break;

            }
            break;

        case WM_CREATE:
            timerDelta = new Timer();
            timerFps   = new Timer();

            hPanelRender = CreateWindow("Static", NULL,
                                WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, 0, 0, 0, 0,
                                wnd, NULL, myInstance, NULL);
            oldPanelRenderProc = (long)SetWindowLong(hPanelRender, GWL_WNDPROC, (long)&PanRenderProc);
            dcPanelRender = GetDC(hPanelRender);

            if (!InitGl()){
                std::cout << "InitGl() failed!" << std::endl;
                DestroyWindow(wnd);
                PostQuitMessage(0);
                return 0;
            }


            textureFont = BuildTexturedFont((char*)"textures/font.png");
            if (!textureFont){
                std::cout << "Can't load font.png!" << std::endl;
                MessageBox(wnd, "Can't load font.png!", MYCLASS1, MB_ICONERROR);
            }

            LoadGameTextures();

            world = new World(256, 256, 64, "world1");
            if (!world->Load()){
                world->GenerateMap();
            }
            worldRenderer = new WorldRenderer(world);
            player = new Player(world, 0.0, world->sizeZ + 1.0, 0.0, 136.0, 0.0, 0.0);
            player->onCameraChanged = CameraChanged;
            objectSelector = new ObjectSelector(player);
            gameTimer = new GameTimer(30.0);
            frustumObj = new CFrustum();
            break;

        case WM_DESTROY:
            delete timerDelta;
            delete timerFps;
            delete gameTimer;
            delete player;
            delete world;
            delete worldRenderer;
            delete objectSelector;
            delete frustumObj;

            if (textureFont){
                KillFont();
            }
            KillGameTextures();
            wglMakeCurrent(dcPanelRender, 0);
            wglDeleteContext(dcGL);

            ReleaseDC(hPanelRender, dcPanelRender);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(wnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int iCmdShow){
    std::cout << "Starting Cubixcraft first alpha... " <<
                 "| Version 0.0.1-alpha | 09.02.2020 is the date the cubes were respawned!" << std::endl;
    myInstance = hInstance;

    HBRUSH brush1 = CreateSolidBrush(0);

    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hIconSm       = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = brush1;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = MYCLASS1;
    if (!RegisterClassEx(&wc)){
        DeleteObject(brush1);
        std::cout << "ERROR: Can't create main window!" << std::endl;
        MessageBox(0, "Can't create main window!", MYTITLE1, MB_ICONERROR);
        return 0;
    }

    int nLeft = 0;
    int nTop = 0;
    int nWidth = 800;
    int nHeight = 600;
    RECT r;
    if (GetWindowRect(GetDesktopWindow(), &r)){
        nLeft = r.right / 2 - nWidth / 2;
        nTop = r.bottom / 2 - nHeight / 2;
    }

    hMainWindow = CreateWindow(wc.lpszClassName, title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        nLeft, nTop, nWidth, nHeight,
        NULL, NULL, hInstance, NULL);

    MSG msg;
    while (true) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
            if (msg.message == WM_QUIT){
                break;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            LifeCicle();
        }
   }

    if (!UnregisterClass(wc.lpszClassName, wc.hInstance)){
        DeleteObject(brush1);
        std::cout << "ERROR: Can't unregister main window!" << std::endl;
        MessageBox(0, "Unregistering main window failed!", MYTITLE1, MB_ICONERROR);
    }

    return 0;
}
