#include "dx_windows.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    int ret;

    gd::Windows windows{ hInstance, nCmdShow };

    ret = windows.create(100, 100, "hoge");
    if (ret) return 1;

    ret = windows.create(200, 200, "hoge");
    if (ret) return 1;

    ret = windows.waitUntilExit();

    return ret;
}
