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
    int width, height;
    uint32_t backgroundColor = 0x000000;
    Capture wallpaperCapture, listviewCapture;
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
        backgroundColor = desk.value.backgroundColor;
        wallpaperCapture.start(graph.getDeviceContext(), desk.value.wallpaper);
        listviewCapture.start(graph.getDeviceContext(), desk.value.listview);
        icons = std::make_unique<Icons>(desk.value.listview);
        icons->update();
    }
    void render(gd::Graph& graph, const gd::Mouse& mouse, const gd::Keyboard& keyboard) override
    {
        RootComponent::render(graph, mouse, keyboard);
        graph.fill(backgroundColor, 0xff);
        graph.stroke(0x000000, 0x00);
        graph.rect(0, 0, width, height);

        // 終了のショートカットキー
        if (
            keyboard.keys.count(VK_ESCAPE) ||  // 'Esc'キー
            keyboard.keys.count(0x51)          // 'Q'キー
        ) { closeWindow(); }

        auto p1 = mouse.point;
        float c = .5f + .5f * std::sin(t+=0.01);

        graph.setRenderMode(BlendMode::AlphaBlend1, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.image(wallpaperCapture.getBackupImage());
        graph.image(listviewCapture.getImage());

        graph.setRenderMode(BlendMode::AlphaBlend1, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.fill(c, c, c, .5f);
        graph.stroke(1.f, 0.f, 0.f, 0.5f);
        graph.ellipse(p1.x, p1.y, 100.f, 10.f, 32);

        graph.fill(1.f, 0.f, 0.f, .0f);
        graph.stroke(1.f, 0.f, 0.f, 0.5f);
        static size_t count = 0, t = 0;
        t = (t + 1) % 3;
        if (0 == t) { count = (count + 1) % icons->size(); }
        else { return; }
        icons->unselect();
        for (auto& icon : *icons)
        {
            if (icon.index == count) { icon.select(LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED); }
            graph.rect(icon.itemArea.left, icon.itemArea.top, icon.itemArea.right, icon.itemArea.bottom, 1.f);
            graph.rect(icon.iconArea.left, icon.iconArea.top, icon.iconArea.right, icon.iconArea.bottom, 1.f);
        }
    }
    void resize(int width, int height) override {
        this->width = width;
        this->height = height;
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
