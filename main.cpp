#include "dx_windows.h"
#include "component.h"

class CustomComponent : public gd::RootComponent
{
public:
    void init(gd::Graph& graph) {}
    void render(gd::Graph& graph, gd::Mouse& mouse) override
    {
        auto p1 = mouse.point;
        auto p2 = p1; p2.x += 100;
        auto color = gd::Color(0x000000);
        if (mouse.lPressed) color = gd::Color(0xFF0000);
        if (mouse.rPressed) color = gd::Color(0x00FF00);
        if (mouse.mPressed) color = gd::Color(0x0000FF);
        for (int i = 0; i < 100; i++)
        {
            p1.y += 1; p2.y += 1;
            graph.Line(p1, p2, color);
        }
    }
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
