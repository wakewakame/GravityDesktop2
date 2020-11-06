#include "window.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    int ret;

    gd::Windows windows{ hInstance, nCmdShow };

    ret = windows.create(640, 480, "hoge", WS_OVERLAPPEDWINDOW, true);
    if (ret) return 1;

    ret = windows.create(640, 480, "hoge", WS_OVERLAPPEDWINDOW, true);
    if (ret) return 1;

    ret = windows.waitUntilExit();

    return ret;
}
