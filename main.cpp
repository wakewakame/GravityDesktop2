#include "window.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    int ret;

    ret = gd::Window::create(hInstance, nCmdShow, 640, 480, "hoge", WS_OVERLAPPEDWINDOW, true);
    if (ret) return 1;

    ret = gd::Window::waitUntilExit();

    gd::Window::destroy();

    return ret;
}
