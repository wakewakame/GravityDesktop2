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
    int width, height;
    Capture wallpaperCapture, listviewCapture;
    std::unique_ptr<Icons> icons;
    void init(gd::Graph& graph) override
    {
        RootComponent::init(graph);

        // �f�X�N�g�b�v�̃E�B���h�E�n���h���擾
        auto desk = FakeDesktopComponent::getDesktopHwnd();

        // �Ώۂ̃E�B���h�E�n���h�����L���v�`���J�n
        wallpaperCapture.start(graph.getDeviceContext(), desk.value.wallpaper);
        listviewCapture.start(graph.getDeviceContext(), desk.value.listview);

        // �f�X�N�g�b�v�̃A�C�R�����̎擾
        icons = std::make_unique<Icons>(desk.value.listview);
        icons->update();
    }
    void render(gd::Graph& graph, const gd::Mouse& mouse, const gd::Keyboard& keyboard) override
    {
        RootComponent::render(graph, mouse, keyboard);

        // �ǎ��̕`��
        graph.setRenderMode(BlendMode::Opaque, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.image(wallpaperCapture.getBackupImage());

        // �S�A�C�R���̕`��
        graph.setRenderMode(BlendMode::AlphaBlend2, DepthMode::DepthNone, RasterizerMode::CullNone);
        for (auto& icon : *icons)
        {
            DirectX::XMFLOAT2 position = { (float)icon.itemArea().left, (float)icon.itemArea().top };
            graph.image(listviewCapture.getImage(), icon.itemArea(), position, {0.0, 0.0}, 0.0);
        }

        // �A�C�R����g�ň͂�
        graph.fill(1.f, 0.f, 0.f, .0f);
        graph.stroke(1.f, 0.f, 0.f, 0.5f);
        for (auto& icon : *icons)
        {
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
