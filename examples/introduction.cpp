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
    // ウィンドウの描画範囲
    int width, height;

    // マウス座標
    int mouseX, mouseY;

    // 初期化時に呼び出されるメソッド
    void init(gd::Graph& graph) override
    {
        RootComponent::init(graph);
    }

    // 一定のフレームレートで呼び出されるメソッド
    void update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard) override
    {
        RootComponent::update(elapsedTime, mouse, keyboard);

        // 終了のショートカットキー
        if (
            keyboard.keys.count(VK_ESCAPE) ||  // 'Esc'キー
            keyboard.keys.count(0x51)          // 'Q'キー
        )
        {
            // エラーが起きたときにダイアログを出すことができる
            gd::Windows::error(L"終了します。");
            closeWindow();
        }

        // マウス座標
        mouseX = mouse.point.x;
        mouseY = mouse.point.y;
    }

    // 画面に描画する処理を行うメソッド
    void render(gd::Graph& graph) override
    {
        RootComponent::render(graph);

        // 合成モードなどの指定
        graph.setRenderMode(BlendMode::AlphaBlend1, DepthMode::DepthNone, RasterizerMode::CullNone);

        // 背景をリセット
        graph.fill(0xFFFFFF, 0xFF);
        graph.rect(0, 0, width, height, 0.0f);

        // マウス座標に図形を描画
        graph.fill(0xFF00FF, 0xFF);
        graph.stroke(0x000000, 0x7F);
        graph.ellipse(mouseX, mouseY, 100.f, 10.f, 32);
    }

	// ウィンドウの作成時、リサイズ時に呼び出されるメソッド
    void resize(int width, int height) override {
        this->width = width;
        this->height = height;
    }

    // updateを呼び出すhフレームレートを指定する
    double getFps() const override { return 60.0; }
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // 現在のスレッドでCOMライブラリを使用することを宣言する
    Microsoft::WRL::Wrappers::RoInitializeWrapper initWrapper{RO_INIT_SINGLETHREADED};
    if (!SUCCEEDED(initWrapper)) return 3;

    int ret;

    gd::Windows windows{ hInstance, nCmdShow };

    // ウィンドウを2つ作る
    ret = windows.create<CustomComponent>(); if (ret) return 1;
    ret = windows.create<CustomComponent>(); if (ret) return 1;

    // 全てのウィンドウが閉じるまで待機する
    ret = windows.waitUntilExit();

    return ret;
}
