#include "components/dx_windows.h"
#include "components/component.h"
#include "desktop/capture.h"
#include "desktop/fake_desktop.h"
#include "components/dx_windows.h"
#include "components/component.h"
#include "desktop/capture.h"
#include "desktop/fake_desktop.h"
#include "desktop/icons.h"
#include "physics/world.h"

#include <SpriteBatch.h>

using namespace gd;

class CustomComponent : public RootComponent
{
public:
    PhysicsWorld world;
	std::list<PhysicsObj> obj;
	PhysicsPicker drag_picker;

    int width, height;
    void resize(int width, int height) override {
        this->width = width; this->height = height;
		world.resizeWorld(width, height);
    }

    Capture wallpaperCapture, listviewCapture;
    std::unique_ptr<Icons> icons;
    void init(gd::Graph& graph) override
    {
        RootComponent::init(graph);

        // デスクトップのウィンドウハンドル取得
        auto desk = FakeDesktopComponent::getDesktopHwnd();

        // 対象のウィンドウハンドルをキャプチャ開始
        wallpaperCapture.start(graph.getDeviceContext(), desk.value.wallpaper);
        listviewCapture.start(graph.getDeviceContext(), desk.value.listview);

        // デスクトップのアイコン情報の取得
        icons = std::make_unique<Icons>(desk.value.listview);
        icons->update();

		// オブジェクトの生成
        for (auto& icon : *icons) {
            RECT area = icon.itemArea();
			obj.emplace_back(world.createObj(
				area.left, area.top, area.right - area.left, area.bottom - area.top,
                PhysicsObjType::DYNAMIC
			));
		}
    }
    void update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard) {
        RootComponent::update(elapsedTime, mouse, keyboard);
        if (keyboard.keys.count(VK_ESCAPE)) { closeWindow(); }

        // 現在のフレームレートをPhysicsWorldに伝える
        world.setFps(getFps());

		// ドラッグ処理
		if (mouse.lRelease()) drag_picker.reset();
        bool unhot = true;
        uint64_t count = 0;
		if (mouse.lClick() && !keyboard.keys.count(16)) icons->unselect();
		for (auto& o : obj) {
			if (o->isHit(mouse.point.x, mouse.point.y)) {
                (*icons)[count].hot();
                unhot = false;
                if (mouse.lClick()) {
					drag_picker = world.createPicker(o, mouse.point.x, mouse.point.y);
                    (*icons)[count].select(LVIS_SELECTED, LVIS_SELECTED);
                }
			}
            count++;
		}
		if (drag_picker) drag_picker->setPosition(mouse.point.x, mouse.point.y);
        if (unhot) {
            icons->unhot();
        }

		// 右クリックでオブジェクトを集める
		if (mouse.rPressed) {
			for (auto& o : obj) o->spring(mouse.point.x, mouse.point.y, 60.0f);
		}

		// 物理演算
		world.update();
    }
    void render(gd::Graph& graph) override
    {
        RootComponent::render(graph);
        graph.setRenderMode(BlendMode::AlphaBlend1, DepthMode::DepthNone, RasterizerMode::CullNone);

        // 壁紙の描画
        graph.setRenderMode(BlendMode::Opaque, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.image(wallpaperCapture.getBackupImage());

        // 全アイコンの描画
        graph.setRenderMode(BlendMode::AlphaBlend2, DepthMode::DepthNone, RasterizerMode::CullNone);
        auto o = obj.begin();
        for (auto& icon : *icons)
        {
            b2Vec2 position_ = (*o)->getPosition();
            b2Vec2 origin_ = 0.5f * (*o)->getSize();
            float angle = (*o)->getAngle();
            o++;
            DirectX::XMFLOAT2 position{ position_.x, position_.y };
            DirectX::XMFLOAT2 origin{ origin_.x, origin_.y };
            graph.image(listviewCapture.getImage(), icon.itemArea(), position, origin, angle);
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
