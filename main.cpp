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
        
        graph.beginShape(true, 10.f);
        if (mouse.zDelta) div += mouse.zDelta / 120;
        if (div < 2) { div = 2; }
        if (div > 128) { div = 128; }
        t += 0.1f;
        for (int i = 0; i < div; i++)
        {
            float p = (float)i / (float)div;
            float theta = t + DirectX::XM_2PI * p;
            float x = std::cos(theta) * 100.0f + (float)p1.x;
            float y = std::sin(theta) * 100.0f + (float)p1.y;
            graph.fill(color, 128);
            graph.stroke(color2, 128);
            graph.vertext(x, y);
        }
        graph.endShape(true);
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
