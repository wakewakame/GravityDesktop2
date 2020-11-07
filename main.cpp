#include "dx_windows.h"
#include "component.h"

class CustomComponent : public gd::RootComponent
{
public:
    void init(gd::Graph& graph) {}
    void update(float a) override {}
    void render(gd::Graph& graph) override {}
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    int ret;

    gd::Windows windows{ hInstance, nCmdShow };

    CustomComponent component;
    ret = windows.create<CustomComponent>();
    if (ret) return 1;

    ret = windows.waitUntilExit();

    return ret;
}
