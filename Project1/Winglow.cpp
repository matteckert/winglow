#include "Winglow.h"

#define GL_FALSE 0
#define GL_TRUE 1

#include "wgl_common.h"
#include "leanwin.h"

#include <stdexcept>

void loadWGLExtensionsUsingFakeContext();

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static const WNDCLASSEX DEFAULT_WNDCLASSEX = {
    sizeof DEFAULT_WNDCLASSEX,
    CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
    &WindowProc,
    0,
    0,
    GetModuleHandle(NULL),
    NULL,
    LoadCursor(NULL, IDC_ARROW),
    NULL,
    NULL,
    "WinglowOpenGLWindow",
    NULL
};

static const ATOM classExAtom = RegisterClassEx(&DEFAULT_WNDCLASSEX);

static const PIXELFORMATDESCRIPTOR DEFAULT_PFD = {
    sizeof(PIXELFORMATDESCRIPTOR), 1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA, 32,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    24, 8,
    0,
    PFD_MAIN_PLANE,
    0, 0, 0, 0
};

static const int DEFAULT_PIXELFORMAT_ATTRIBUTES[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    0
};

static const int DEFAULT_CONTEXT_ATTRIBUTES[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 2,
    0
};

struct Winglow::WinglowImpl {
    HWND hWnd; HDC hDC; HGLRC hGLRC;
};

Winglow::Winglow() : pimpl(new WinglowImpl) {
    loadWGLExtensionsUsingFakeContext();

    HWND hWnd = CreateWindow("WinglowOpenGLWindow",
                             "REAL WINDOW WITH WGL EXTENSIONS LOADED",
                             WS_TILEDWINDOW,
                             CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
                             NULL, NULL, NULL, NULL);
    if (!hWnd) throw std::runtime_error("Failed to create real window.");

    HDC hDC = GetDC(hWnd);
    if (!hDC) throw std::runtime_error("Failed to get real window's device context.");

    int pixelFormat; UINT numFormats;
    if (!wglChoosePixelFormatARB(hDC, DEFAULT_PIXELFORMAT_ATTRIBUTES, NULL, 1, &pixelFormat, &numFormats)
        || numFormats == 0)
        throw std::runtime_error("Failed to choose real window's pixel format.");

    if (!SetPixelFormat(hDC, pixelFormat, &DEFAULT_PFD))
        throw std::runtime_error("Failed to set real window's pixel format.");

    HGLRC hGLRC = wglCreateContextAttribsARB(hDC, NULL, DEFAULT_CONTEXT_ATTRIBUTES);
    if (!hGLRC)
        throw std::runtime_error("Failed to create real window's GL context.");

    this->pimpl->hWnd = hWnd;
    this->pimpl->hDC = hDC;
    this->pimpl->hGLRC = hGLRC;
}

Winglow::~Winglow() {
    delete pimpl;
}

bool Winglow::isAlive() {
    return IsWindow(this->pimpl->hWnd) != 0;
}

void Winglow::makeCurrent() {
    if (!wglMakeCurrent(this->pimpl->hDC, this->pimpl->hGLRC))
        throw std::runtime_error("Failed to make real window's GL context current.");
}

void Winglow::swapBuffers() {
    SwapBuffers(this->pimpl->hDC);
}

void Winglow::show() {
    ShowWindow(this->pimpl->hWnd, SW_SHOW);
}

void Winglow::hide() {
    ShowWindow(this->pimpl->hWnd, SW_HIDE);
}

void Winglow::minimize() {
    ShowWindow(this->pimpl->hWnd, SW_MINIMIZE);
}

void Winglow::maximize() {
    ShowWindow(this->pimpl->hWnd, SW_MAXIMIZE);
}

void Winglow::restore() {
    ShowWindow(this->pimpl->hWnd, SW_RESTORE);
}

void loadWGLExtensionsUsingFakeContext() {
    HWND hWnd = CreateWindow("WinglowOpenGLWindow",
                             "FAKE WINDOW FOR CONTEXT",
                             WS_TILEDWINDOW,
                             CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
                             NULL, NULL, NULL, NULL);
    if (!hWnd) throw std::runtime_error("Failed to create fake window.");

    HDC hDC = GetDC(hWnd);
    if (!hDC)
        throw std::runtime_error("Failed to get fake window's device context.");

    int pixelFormat = ChoosePixelFormat(hDC, &DEFAULT_PFD);
    if (!pixelFormat)
        throw std::runtime_error("Failed to choose fake window's pixel format.");

    if (!SetPixelFormat(hDC, pixelFormat, &DEFAULT_PFD))
        throw std::runtime_error("Failed to set fake window's pixel format.");

    HGLRC hGLRC = wglCreateContext(hDC);
    if (!hGLRC)
        throw std::runtime_error("Failed to create fake window's GL context.");

    if (!wglMakeCurrent(hDC, hGLRC))
        throw std::runtime_error("Failed to make fake window's GL context current.");

    if (!wgl_LoadFunctions(hDC))
        throw std::runtime_error("Failed to load WGL extensions.");

    if (!wglMakeCurrent(NULL, NULL))
        throw std::runtime_error("Failed to remove fake window's current context.");

    if (!wglDeleteContext(hGLRC))
        throw std::runtime_error("Failed to delete fake window's GL context.");

    if (!ReleaseDC(hWnd, hDC))
        throw std::runtime_error("Failed to release fake window's device context.");

    if (!DestroyWindow(hWnd))
        throw std::runtime_error("Failed to destroy fake window.");
}
