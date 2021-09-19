#include "components/dx_windows.h"
#include "components/component.h"
#include "desktop/capture.h"
#include "desktop/fake_desktop.h"
#include "desktop/icons.h"

#include <SpriteBatch.h>

using namespace gd;

// FakeDesktopComponent�ł͂Ȃ�FakeDesktopComponent���p�����邱�Ƃ�
// �f�X�N�g�b�v�ɋU�������E�B���h�E�𐶐��ł���
class CustomComponent : public FakeDesktopComponent
{
public:
    // �E�B���h�E�̕`��͈�
    int width, height;

    // ���������ɌĂяo����郁�\�b�h
    void init(gd::Graph& graph) override
    {
        FakeDesktopComponent::init(graph);
    }

    // ��ʂɕ`�悷�鏈�����s�����\�b�h
    void render(gd::Graph& graph, const gd::Mouse& mouse, const gd::Keyboard& keyboard) override
    {
        FakeDesktopComponent::render(graph, mouse, keyboard);

        // �I���̃V���[�g�J�b�g�L�[
        if (
            keyboard.keys.count(VK_ESCAPE) ||  // 'Esc'�L�[
            keyboard.keys.count(0x51)          // 'Q'�L�[
        ) { closeWindow(); }

        // �������[�h�Ȃǂ̎w��
        graph.setRenderMode(BlendMode::Opaque, DepthMode::DepthNone, RasterizerMode::CullNone);

        // �w�i�����Z�b�g
        graph.fill(0xFFFFFF, 0xFF);
        graph.rect(0, 0, width, height, 0.0f);

        // �}�E�X���W�ɐ}�`��`��
        graph.fill(0xFF00FF, 0xFF);
        graph.stroke(0x000000, 0x7F);
        graph.ellipse(mouse.point.x, mouse.point.y, 100.f, 10.f, 32);
    }

	// �E�B���h�E�̍쐬���A���T�C�Y���ɌĂяo����郁�\�b�h
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
    ret = windows.create<CustomComponent>(); if (ret) return 1;
    ret = windows.waitUntilExit();

    return ret;
}
