#include "dx_windows.h"
#include "component.h"
#include "math_utils.h"

using namespace gd;

class CustomComponent : public gd::RootComponent
{
public:
    bool a;
    int scale = 60;
    void init(gd::Graph& graph) {
        graph.setRenderMode(
            BlendMode::AlphaBlend,
            DepthMode::DepthNone,
            RasterizerMode::Wireframe
            //RasterizerMode::CullNone
        );
    }
    void render(gd::Graph& g, gd::Mouse& mouse) override
    {
        auto m = mouse.point;

        //if (!mouse.lPressed) { m.x = m.x < 320 ? 320 : m.x; }
        if (mouse.lDouble) { a = !a; g.setRenderMode(
            BlendMode::AlphaBlend,
            DepthMode::DepthNone,
            a ? RasterizerMode::Wireframe : RasterizerMode::CullNone
        ); }

        scale += 5 * mouse.zDelta / 120;

        Vector3 hoge;

        g.stroke(0x000000, 128);
        g.beginShape(false, scale);
        g.vertex(0, 240);
        g.vertex(320, 240);
        //g.vertex(320, 240);
        g.vertex(m.x, m.y);
        g.endShape(false);
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
