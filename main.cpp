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
    int mouseX, mouseY;
    Capture wallpaperCapture, listviewCapture;
    std::unique_ptr<Icons> icons;
    void init(gd::Graph& graph) override
    {
        RootComponent::init(graph);
        graph.setRenderMode(
            BlendMode::AlphaBlend1,
            DepthMode::DepthNone,
            //RasterizerMode::Wireframe
            RasterizerMode::CullNone
        );
        auto desk = FakeDesktopComponent::getDesktopHwnd();
        wallpaperCapture.start(graph.getDeviceContext(), desk.value.wallpaper);
        listviewCapture.start(graph.getDeviceContext(), desk.value.listview);
        icons = std::make_unique<Icons>(desk.value.listview);
        icons->update();
    }
    void update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard) override
    {
        RootComponent::update(elapsedTime, mouse, keyboard);

        // 終了のショートカットキー
        if (
            keyboard.keys.count(VK_ESCAPE) ||  // 'Esc'キー
            keyboard.keys.count(0x51)          // 'Q'キー
        ) { closeWindow(); }

        // マウス座標
        mouseX = mouse.point.x;
        mouseY = mouse.point.y;
    }
    void render(gd::Graph& graph) override
    {
        RootComponent::render(graph);

        // 壁紙の描画
        graph.setRenderMode(BlendMode::Opaque, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.image(wallpaperCapture.getBackupImage(), { 0, 0, width, height });

        // 全アイコンの描画
        graph.setRenderMode(BlendMode::AlphaBlend2, DepthMode::DepthNone, RasterizerMode::CullNone);
        for (auto& icon : *icons)
        {
            DirectX::XMFLOAT2 position = { (float)icon.itemArea().left, (float)icon.itemArea().top };
            graph.image(listviewCapture.getImage(), icon.itemArea(), position, {0.0, 0.0}, 0.0);
        }

        float c = .5f + .5f * std::sin(t+=0.01);
        graph.setRenderMode(BlendMode::AlphaBlend1, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.fill(c, c, c, .5f);
        graph.stroke(1.f, 0.f, 0.f, 0.5f);
        graph.ellipse(mouseX, mouseY, 100.f, 10.f, 32);

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
            graph.rect(icon.itemArea().left, icon.itemArea().top, icon.itemArea().right, icon.itemArea().bottom, 1.f);
            graph.rect(icon.iconArea().left, icon.iconArea().top, icon.iconArea().right, icon.iconArea().bottom, 1.f);
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
