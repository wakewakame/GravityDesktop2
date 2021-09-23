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

// �A�C�R���ƕ����I�u�W�F�N�g���܂Ƃ߂ĊǗ�����N���X
class PhysicsIcons {
private:
    std::unique_ptr<Icons> icons;
    PhysicsWorld& world;
	std::list<PhysicsObj> objs;
	std::list<PhysicsObj> taskbars;

public:
    PhysicsIcons(PhysicsWorld& world, gd::FakeDesktopComponent::DesktopHwnds desk) : world(world)
    {
        // �A�C�R���̎擾
        icons = std::make_unique<Icons>(desk.listview);
        icons->update();

		// �A�C�R���I�u�W�F�N�g�̐���
        for (Icon& icon : *icons) {
            RECT area = icon->itemArea();
			objs.emplace_back(world.createObj(
				area.left, area.top, area.right - area.left, area.bottom - area.top,
                PhysicsObjType::DYNAMIC
			));
		}

        // �^�X�N�o�[�̕ǂ𐶐�
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
            �f�X�N�g�b�v�̃A�C�R�����X�V���ꂽ�Ƃ���
            �^�X�N�o�[�̈ʒu���ύX���ꂽ�Ƃ��Ȃǂ�
            �I�u�W�F�N�g�̍��W���X�V����
        */
    }

    // �S�ẴA�C�R���I�u�W�F�N�g���g��for�Ń��[�v���邽�߂̎���
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

    // �S�Ă�I��
    void allselect() { icons->allselect(); }

    // �S�Ă̑I��������
    void unselect() { icons->unselect(); }

    // �S�Ẵn�C���C�g������
    void unhot() { icons->unhot(); }

    // �w�肳�ꂽ���W�̒����ɂ���A�C�R���̎擾
    std::optional<IconObj> hitTest(float x, float y) {
        std::list<PhysicsObj>::iterator objIter = objs.begin();
        for (gd::Icon icon : *icons) {
            if (objIter == objs.end()) break;
            if ((*objIter)->isHit(x, y)) return IconObj(icon, *objIter);
            objIter++;
        }
        return std::nullopt;
    }

    // �S�ẴA�C�R���ɑ΂��ă��[�v
    void forEach(std::function<void(Icon, PhysicsObj)> callback) {
        std::list<PhysicsObj>::iterator objIter = objs.begin();
        for (gd::Icon icon : *icons) {
            if (objIter == objs.end()) break;
			callback(icon, *objIter);
            objIter++;
        }
    }

    // �I�����ꂽ�A�C�R���ɑ΂��ă��[�v
    void forEachSelected(std::function<void(Icon, PhysicsObj)> callback) {
        std::list<PhysicsObj>::iterator objIter = objs.begin();
        for (gd::Icon& icon : *icons) {
            if (objIter == objs.end()) break;
            if (icon->isSelect()) callback(icon, *objIter);
            objIter++;
        }
    }

    // �I������Ă��Ȃ��A�C�R���ɑ΂��ă��[�v
    void forEachUnselected(std::function<void(Icon, PhysicsObj)> callback) {
        std::list<PhysicsObj>::iterator objIter = objs.begin();
        for (gd::Icon& icon : *icons) {
            if (objIter == objs.end()) break;
            if (!icon->isSelect()) callback(icon, *objIter);
            objIter++;
        }
    }

    // �����ʒu�ɃX���[�Y�Ɉړ�����
    // �ړ������������true���A��
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

    // �I�����ꂽ�A�C�R���݂̂��񂹏W�߂�
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

        // �f�X�N�g�b�v�̃E�B���h�E�n���h���擾
        auto desk = FakeDesktopComponent::getDesktopHwnd();

        // �A�C�R���̎擾
        iconObjs = std::make_unique<PhysicsIcons>(world, desk.value);

        // �Ώۂ̃E�B���h�E�n���h�����L���v�`���J�n
        wallpaperCapture.start(graph.getDeviceContext(), desk.value.wallpaper);
        listviewCapture.start(graph.getDeviceContext(), desk.value.listview);
    }
    void update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard) {
        ParentComponent::update(elapsedTime, mouse, keyboard);

        // �e�L�[�̏�Ԏ擾
        bool ShiftKey = keyboard.keys.count(16);
        bool Ctrl = keyboard.keys.count(17);
        bool EscKey = keyboard.keys.count(VK_ESCAPE);
        bool AKey = keyboard.keys.count(65);
        bool GKey = keyboard.keys.count(71);
        bool GKey_ = keyboard.keys_.count(71);
        bool QKey = keyboard.keys.count(81);
        bool UKey = keyboard.keys.count(85);
        bool UKey_ = keyboard.keys_.count(85);

        // ���݂̃t���[�����[�g��PhysicsWorld�ɓ`����
        world.setFps(getFps());

        // �A�C�R�����̍X�V
        iconObjs->update();

        // �J�[�\�������̃A�C�R���擾
		auto iconObj = iconObjs->hitTest(mouse.point.x, mouse.point.y);

        // �I�����̃A�j���[�V����
        exit_flag = exit_flag || EscKey || QKey;
        if (exit_flag && iconObjs->transformDefault(exit_flag)) closeWindow();

        // Ctrl-A�őS�I��
        if (Ctrl && AKey) iconObjs->allselect();

        // U�L�[�ŏ����ʒu�Ɉړ�
        if (UKey) iconObjs->transformDefault(true);
        else if (UKey_) iconObjs->transformDefault(false);

        // G�L�[�ŏd�͂̑���
        if (GKey) {
            b2Vec2 gravity = 4.0f * b2Vec2{
                (mouse.point.x - 0.5f * static_cast<float>(width)),
                (mouse.point.y - 0.5f * static_cast<float>(height))
            };
            world.setGravity(gravity.x, gravity.y);
        }
        else if (GKey_) world.setEarthGravity();

		// �I�𒆂̃I�u�W�F�N�g���E�N���b�N�ŏW�߂�
        if (mouse.rPressed) iconObjs->spring(mouse.point.x, mouse.point.y);

        // �J�[�\�������ɃA�C�R��������΃n�C���C�g
        if (iconObj) iconObj->icon->hot();
        else iconObjs->unhot();

        // �N���b�N���ɃJ�[�\�������ɃA�C�R��������ΑI��
        if (mouse.lClick()) {
            if (iconObj) {
                if (!ShiftKey) iconObjs->forEachSelected([iconObj](Icon icon, PhysicsObj) {
                    if (icon != iconObj->icon) icon->select(false);
                    });
                iconObj->icon->select(!ShiftKey || !iconObj->icon->isSelect());
            }
			else if (!ShiftKey) iconObjs->unselect();
        }

        // ���N���b�N�Ńh���b�O�̊J�n
        if (mouse.lClick() && !ShiftKey && iconObj) {
            drag_picker = world.createPicker(iconObj->obj, mouse.point.x, mouse.point.y);
        }
		if (drag_picker) drag_picker->setPosition(mouse.point.x, mouse.point.y);
		if (mouse.lRelease()) drag_picker.reset();
        
		// �������Z
		world.update();
    }
    void render(gd::Graph& graph) override
    {
        ParentComponent::render(graph);
        graph.setRenderMode(BlendMode::AlphaBlend1, DepthMode::DepthNone, RasterizerMode::CullNone);

        // �ǎ��̕`��
        graph.setRenderMode(BlendMode::Opaque, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.image(wallpaperCapture.getBackupImage());

        // �S�A�C�R���̕`��
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
    // ���݂̃X���b�h��COM���C�u�������g�p���邱�Ƃ�錾����
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
