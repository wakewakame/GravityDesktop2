#include "components/dx_windows.h"
#include "components/component.h"
#include "desktop/capture.h"
#include "desktop/fake_desktop.h"
#include "components/dx_windows.h"
#include "components/component.h"
#include "desktop/capture.h"
#include "desktop/fake_desktop.h"
#include "desktop/physics_icons.h"
#include "physics/world.h"

#include <SpriteBatch.h>

using namespace gd;

using ParentComponent = FakeDesktopComponent;
class CustomComponent : public ParentComponent
{
public:
    FakeDesktopComponent::DesktopHwnds desk;
    PhysicsWorld world;
    std::unique_ptr<PhysicsIcons> iconObjs;
    PhysicsPicker drag_picker;
    POINTS gravity_origin;

    bool exit_flag = false;

    int width, height;
    void resize(int width, int height) override {
        this->width = width; this->height = height;
        world.resizeWorld(width, height);
    }

    Capture wallpaperCapture, listviewCapture;
    void init(gd::Graph& graph) override
    {
        ParentComponent::init(graph);

        // デスクトップのウィンドウハンドル取得
        desk = FakeDesktopComponent::getDesktopHwnd().value;

        // アイコンの取得
        iconObjs = std::make_unique<PhysicsIcons>(world, desk);

        // 対象のウィンドウハンドルをキャプチャ開始
        wallpaperCapture.start(graph.getDeviceContext(), desk.wallpaper);
        listviewCapture.start(graph.getDeviceContext(), desk.listview);
    }
    void update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard) {
        ParentComponent::update(elapsedTime, mouse, keyboard);

        // 各キーの状態取得
        bool EnterKey = keyboard.keys.count(13);
        bool ShiftKey = keyboard.keys.count(16);
        bool Ctrl = keyboard.keys.count(17);
        bool EscKey = keyboard.keys.count(VK_ESCAPE);
        bool AKey = keyboard.keys.count(65);
        bool GKey = keyboard.keys.count(71);
        bool GKey_ = keyboard.keys_.count(71);
        bool MKey = keyboard.keys.count(77);
        bool QKey = keyboard.keys.count(81);
        bool UKey = keyboard.keys.count(85);
        bool UKey_ = keyboard.keys_.count(85);

        // 現在のフレームレートをPhysicsWorldに伝える
        world.setFps(getFps());

        // アイコン情報の更新
        iconObjs->update();

        // カーソル直下のアイコン取得
		auto iconObj = iconObjs->hitTest(mouse.point.x, mouse.point.y);

        // 終了時のアニメーション
        exit_flag = exit_flag || EscKey || QKey;
        if (exit_flag && iconObjs->transformDefault(exit_flag)) closeWindow();

        // Ctrl-Aで全選択
        if (Ctrl && AKey) iconObjs->allselect();

        // Uキーで初期位置に移動
        if (UKey) iconObjs->transformDefault(true);
        else if (UKey_) iconObjs->transformDefault(false);

        // Gキーで重力の操作
        if (GKey) {
            if (!GKey_) { gravity_origin = mouse.point; }
            b2Vec2 gravity = 4.0f * b2Vec2{
                static_cast<float>(mouse.point.x - gravity_origin.x),
                static_cast<float>(mouse.point.y - gravity_origin.y)
            };
            world.setGravity(gravity.x, gravity.y);
        }
        else if (GKey_) world.setEarthGravity();

		// Mキーで全てのオブジェクトを集める
        if (MKey) iconObjs->forEach([&](Icon, PhysicsObj obj) {
            obj->spring(mouse.point.x, mouse.point.y, 60.0f);
        });

		// 選択中のオブジェクトを右クリックで集める
        if (mouse.rPressed) iconObjs->forEachSelected([&](Icon, PhysicsObj obj) {
            obj->spring(mouse.point.x, mouse.point.y, 60.0f);
        });

        // カーソル直下にアイコンがあればハイライト
        if (iconObj) iconObj->icon->hot();
        else iconObjs->unhot();

        // クリック時にカーソル直下にアイコンがあれば選択
        if (mouse.lClick()) {
            if (iconObj) {
                if (!ShiftKey) iconObjs->forEachSelected([iconObj](Icon icon, PhysicsObj) {
                    if (icon != iconObj->icon) icon->select(false);
                    });
                iconObj->icon->select(!ShiftKey || !iconObj->icon->isSelect());
            }
			else if (!ShiftKey) iconObjs->unselect();
        }

        // 左クリックでドラッグの開始
        if (mouse.lClick() && !ShiftKey && iconObj) {
            drag_picker = world.createPicker(iconObj->obj, mouse.point.x, mouse.point.y);
        }
		if (drag_picker) drag_picker->setPosition(mouse.point.x, mouse.point.y);
		if (mouse.lRelease()) drag_picker.reset();

        // ダブルクリックでファイルの実行
        if (mouse.lDouble && iconObj) iconObj->icon->double_click();
        
		// 物理演算
		world.update();
    }
    void render(gd::Graph& graph) override
    {
        ParentComponent::render(graph);
        graph.setRenderMode(BlendMode::AlphaBlend1, DepthMode::DepthNone, RasterizerMode::CullNone);

        // 壁紙の描画
        graph.setRenderMode(BlendMode::Opaque, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.image(wallpaperCapture.getBackupImage());

        // 全アイコンの描画
        graph.setRenderMode(BlendMode::AlphaBlend2, DepthMode::DepthNone, RasterizerMode::CullNone);
        iconObjs->forEach([&](Icon icon, PhysicsObj obj) {
            b2Vec2 position_ = obj->getPosition();
            b2Vec2 origin_ = 0.5f * obj->getSize();
            float angle = obj->getAngle();
            DirectX::XMFLOAT2 position{ position_.x, position_.y };
            DirectX::XMFLOAT2 origin{ origin_.x, origin_.y };
            graph.image(listviewCapture.getImage(), icon->itemArea(), position, origin, angle);
        });
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
    if (ret) return 1;

    ret = windows.waitUntilExit();

    return ret;
}
