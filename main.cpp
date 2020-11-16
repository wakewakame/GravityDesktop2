#include "dx_windows.h"
#include "component.h"
#include "capture.h"

#include <SpriteBatch.h>

using namespace gd;

class CustomComponent : public gd::RootComponent
{
public:
    float t = 0.f;
    Capture capture;
    void init(gd::Graph& graph) {
        graph.setRenderMode(
            BlendMode::AlphaBlend,
            DepthMode::DepthNone,
            //RasterizerMode::Wireframe
            RasterizerMode::CullNone
        );
        capture.start(graph.getDeviceContext(), hWnd);
    }
    void render(gd::Graph& graph, gd::Mouse& mouse) override
    {
        graph.image(capture.getImage());

        auto p1 = mouse.point;
        float c = .5f + .5f * std::sin(t+=0.01);

        graph.fill(0x000000, 128);
        graph.rect(50, 250, 150, 350, 5.f);

        graph.fill(c, c, c, .0f);
        graph.ellipse(p1.x - 100.f, p1.y, 100.f, 10.f, 32);

        graph.fill(c, c, c, .5f);
        graph.ellipse(p1.x, p1.y, 100.f, 10.f, 32);

        graph.fill(c, c, c, 1.f);
        graph.ellipse(p1.x + 100.f, p1.y, 100.f, 10.f, 32);
    }
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // 現在のスレッドでCOMライブラリを使用することを宣言する
    Microsoft::WRL::Wrappers::RoInitializeWrapper initWrapper{RO_INIT_SINGLETHREADED};
    if (!SUCCEEDED(initWrapper)) return 3;

    int ret;

    gd::Windows windows{ hInstance, nCmdShow };

    CustomComponent component;
    ret = windows.create<CustomComponent>();
    ret = windows.create<CustomComponent>();
    if (ret) return 1;

    ret = windows.waitUntilExit();

    return ret;
}
