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
#include <iostream>

class CustomComponent : public RootComponent {
public:
    void init(gd::Graph& graph) override {
        RootComponent::init(graph);
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        std::cout << "hello" << std::endl;
    }
    void update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard) {
        RootComponent::update(elapsedTime, mouse, keyboard);
        if (keyboard.keys.count(VK_ESCAPE)) { closeWindow(); }
    }
    void render(gd::Graph& graph) override {
        RootComponent::render(graph);
    }
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
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
