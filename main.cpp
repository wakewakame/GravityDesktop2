#include "components/dx_windows.h"
#include "components/component.h"
#include "desktop/capture.h"
#include "desktop/fake_desktop.h"
#include "desktop/icons.h"

#include <SpriteBatch.h>

using namespace gd;

class CustomComponent : public RootComponent
{
public:
    float t = 0.f;
    Capture capture1, capture2;
    std::unique_ptr<Icons> icons;
    void init(gd::Graph& graph)
    {
        RootComponent::init(graph);
        graph.setRenderMode(
            BlendMode::AlphaBlend1,
            DepthMode::DepthNone,
            //RasterizerMode::Wireframe
            RasterizerMode::CullNone
        );
        auto desk = FakeDesktopComponent::getDesktopHwnd();
        capture1.start(graph.getDeviceContext(), desk.value.wallpaper);
        capture2.start(graph.getDeviceContext(), desk.value.listview);
        icons = std::make_unique<Icons>(desk.value.listview);
    }
    void render(gd::Graph& graph, gd::Mouse& mouse) override
    {
        RootComponent::render(graph, mouse);

        //if (t > 5.0f) { closeWindow(); };

        auto p1 = mouse.point;
        float c = .5f + .5f * std::sin(t+=0.01);

        graph.setRenderMode(BlendMode::AlphaBlend2, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.image(capture1.getBackupImage());
        graph.image(capture2.getImage());

        graph.setRenderMode(BlendMode::AlphaBlend1, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.fill(c, c, c, .5f);
        graph.stroke(1.f, 0.f, 0.f, 0.5f);
        graph.ellipse(p1.x, p1.y, 100.f, 10.f, 32);

        graph.fill(1.f, 0.f, 0.f, .0f);
        graph.stroke(1.f, 0.f, 0.f, 0.5f);
        icons->update();
        for (auto& icon : *icons)
        {
            graph.rect(icon.itemArea.left, icon.itemArea.top, icon.itemArea.right, icon.itemArea.bottom, 1.f);
            graph.rect(icon.iconArea.left, icon.iconArea.top, icon.iconArea.right, icon.iconArea.bottom, 1.f);
        }
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
    //ret = windows.create<CustomComponent>();
    if (ret) return 1;

    ret = windows.waitUntilExit();

    return ret;
}
