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

// アイコンと物理オブジェクトをまとめて管理するクラス
class PhysicsIcons {
private:
    std::unique_ptr<Icons> icons;
    PhysicsWorld& world;
	std::list<PhysicsObj> objs;
	std::list<PhysicsObj> taskbars;

public:
    PhysicsIcons(PhysicsWorld& world, gd::FakeDesktopComponent::DesktopHwnds desk) : world(world)
    {
        // アイコンの取得
        icons = std::make_unique<Icons>(desk.listview);
        icons->update();

		// アイコンオブジェクトの生成
        for (Icon& icon : *icons) {
            RECT area = icon->itemArea();
			objs.emplace_back(world.createObj(
				area.left, area.top, area.right - area.left, area.bottom - area.top,
                PhysicsObjType::DYNAMIC
			));
		}

        // タスクバーの壁を生成
		RECT listview_rc; GetWindowRect(desk.listview, &listview_rc);
        for (HWND taskbar : desk.taskbars) {
            RECT taskbar_rc; GetWindowRect(taskbar, &taskbar_rc);
			taskbars.emplace_back(world.createObj(
				taskbar_rc.left - listview_rc.left, taskbar_rc.top - listview_rc.top,
                taskbar_rc.right - taskbar_rc.left, taskbar_rc.bottom - taskbar_rc.top,
                PhysicsObjType::STATIC
			));
		}
    }

    void update() {
        /*
            Todo
            デスクトップのアイコンが更新されたときや
            タスクバーの位置が変更されたときなどに
            オブジェクトの座標を更新する
        */
    }

    // 全てのアイコンオブジェクトを拡張forでループするための実装
	struct IconObj {
		gd::Icon icon; PhysicsObj obj;
		IconObj() : icon(nullptr), obj(nullptr) {}
		IconObj(gd::Icon icon, PhysicsObj obj) : icon(icon), obj(obj) {}
	};
    class Iterator {
    private:
        std::vector<gd::Icon>::iterator iconIter;
        std::list<PhysicsObj>::iterator objIter;
    public:
        IconObj iconObj;
        Iterator(
            const std::vector<gd::Icon>::iterator& iconIter,
            const std::list<PhysicsObj>::iterator& objIter
        ) : iconIter(iconIter), objIter(objIter) {}
        IconObj& operator*() { iconObj = IconObj(*iconIter, *objIter); return iconObj; }
        Iterator& operator++() { iconIter++; objIter++; return *this; }
        bool operator!=(const Iterator& v) { return (iconIter != v.iconIter) || (objIter != v.objIter); }
    };
	Iterator begin() { return Iterator(icons->begin(), objs.begin()); }
	Iterator end()   { return Iterator(icons->end()  , objs.end())  ; }

    // 全てを選択
    void allselect() { icons->allselect(); }

    // 全ての選択を解除
    void unselect() { icons->unselect(); }

    // 全てのハイライトを解除
    void unhot() { icons->unhot(); }

    // 指定された座標の直下にあるアイコンの取得
    std::optional<IconObj> hitTest(float x, float y) {
        std::list<PhysicsObj>::iterator objIter = objs.begin();
        for (gd::Icon icon : *icons) {
            if (objIter == objs.end()) break;
            if ((*objIter)->isHit(x, y)) return IconObj(icon, *objIter);
            objIter++;
        }
        return std::nullopt;
    }

    // 全てのアイコンに対してループ
    void forEach(std::function<void(Icon, PhysicsObj)> callback) {
        std::list<PhysicsObj>::iterator objIter = objs.begin();
        for (gd::Icon icon : *icons) {
            if (objIter == objs.end()) break;
			callback(icon, *objIter);
            objIter++;
        }
    }

    // 選択されたアイコンに対してループ
    void forEachSelected(std::function<void(Icon, PhysicsObj)> callback) {
        std::list<PhysicsObj>::iterator objIter = objs.begin();
        for (gd::Icon& icon : *icons) {
            if (objIter == objs.end()) break;
            if (icon->isSelect()) callback(icon, *objIter);
            objIter++;
        }
    }

    // 選択されていないアイコンに対してループ
    void forEachUnselected(std::function<void(Icon, PhysicsObj)> callback) {
        std::list<PhysicsObj>::iterator objIter = objs.begin();
        for (gd::Icon& icon : *icons) {
            if (objIter == objs.end()) break;
            if (!icon->isSelect()) callback(icon, *objIter);
            objIter++;
        }
    }

    // 初期位置にスムーズに移動する
    // 移動が完了するとtrueが帰る
    bool transformDefault(bool flag, float speed = 0.8f, float threshold = 0.5f) {
        static const float radToDeg = 180.0f / acos(-1.0f);
        float error = 0.0f;
        if (flag) {
            const float rate = std::pow(std::pow(speed, 60.0f), 1.0f / world.getFps());
            for (auto& iconObj : *this) {
                b2Vec2 target{
                    static_cast<float>(iconObj.icon->itemArea().left + iconObj.icon->itemArea().right) * 0.5f,
                    static_cast<float>(iconObj.icon->itemArea().top + iconObj.icon->itemArea().bottom) * 0.5f
                };
                iconObj.obj->setEnabled(false);
                b2Vec2 current = iconObj.obj->getPosition();
                b2Vec2 position = (1.0f - rate) * target + rate * current;
                float angle = iconObj.obj->getAngle() * rate;
                iconObj.obj->setTransform(position.x, position.y, angle);
                error += (position - target).Length() + std::abs(angle * radToDeg);
            }
            error /= objs.size();
            return error <= threshold;
        }
        else {
            for (auto& iconObj : *this) iconObj.obj->setEnabled(true);
        }
        return false;
    }

    // 選択されたアイコンのみを寄せ集める
    void spring(float x, float y) {
		forEachSelected([&](Icon, PhysicsObj obj) { obj->spring(x, y, 60.0f); });
    }
};

using ParentComponent = FakeDesktopComponent;
class CustomComponent : public ParentComponent
{
public:
    PhysicsWorld world;
    std::unique_ptr<PhysicsIcons> iconObjs;
    PhysicsPicker drag_picker;

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
        auto desk = FakeDesktopComponent::getDesktopHwnd();

        // アイコンの取得
        iconObjs = std::make_unique<PhysicsIcons>(world, desk.value);

        // 対象のウィンドウハンドルをキャプチャ開始
        wallpaperCapture.start(graph.getDeviceContext(), desk.value.wallpaper);
        listviewCapture.start(graph.getDeviceContext(), desk.value.listview);
    }
    void update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard) {
        ParentComponent::update(elapsedTime, mouse, keyboard);

        // 各キーの状態取得
        bool ShiftKey = keyboard.keys.count(16);
        bool Ctrl = keyboard.keys.count(17);
        bool EscKey = keyboard.keys.count(VK_ESCAPE);
        bool AKey = keyboard.keys.count(65);
        bool GKey = keyboard.keys.count(71);
        bool GKey_ = keyboard.keys_.count(71);
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
            b2Vec2 gravity = 4.0f * b2Vec2{
                (mouse.point.x - 0.5f * static_cast<float>(width)),
                (mouse.point.y - 0.5f * static_cast<float>(height))
            };
            world.setGravity(gravity.x, gravity.y);
        }
        else if (GKey_) world.setEarthGravity();

		// 選択中のオブジェクトを右クリックで集める
        if (mouse.rPressed) iconObjs->spring(mouse.point.x, mouse.point.y);

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
    //ret = windows.create<CustomComponent>();
    if (ret) return 1;

    ret = windows.waitUntilExit();

    return ret;
}
