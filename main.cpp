#include "dx_windows.h"
#include "component.h"

class CustomComponent : public gd::RootComponent
{
public:
    bool s = false;
    bool s2 = false;
    int div = 3;
    float t = 0.0f;
    void init(gd::Graph& graph) {}
    void render(gd::Graph& graph, gd::Mouse& mouse) override
    {
        auto p1 = mouse.point;
        auto p2 = p1; p2.x += 100;
        uint32_t color = 0x000000;
        uint32_t color2 = 0xFFFFFF;
        s = mouse.lDouble ? !s : s;
        s2 = mouse.rDouble ? !s2 : s2;
        if (s) color = 0xFFFFFF;
        if (s2) color2 = 0x0000FF;
        if (mouse.lPressed) color = 0xFF0000;
        if (mouse.rPressed) color2 = 0xFF0000;
        if (mouse.mPressed) color = 0x0000FF;

        graph.fill(0x0000FF);
        graph.rect(50, 250, 150, 350, 5.f);

        graph.fill(color, 128);
        graph.stroke(color2, 128);
        
        if (mouse.zDelta) { div += (mouse.zDelta) < 0 ? -1 : 1; }
        if (div < 3) { div = 3; }
        if (div > 16) { div = 16; }

        graph.ellipse(p1.x, p1.y, 100.f, 10.f, div);

        graph.fill(0xFFFFFF);
        graph.rect(10, 200, 100, 300, 10.f);
    }
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    int ret;

    gd::Windows windows{ hInstance, nCmdShow };

    CustomComponent component;
    ret = windows.create<CustomComponent>();
    ret = windows.create<CustomComponent>();
    if (ret) return 1;

    ret = windows.waitUntilExit();

    return ret;
}
