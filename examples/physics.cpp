#include "components/dx_windows.h"
#include "components/component.h"
#include "desktop/capture.h"
#include "desktop/fake_desktop.h"
#include "desktop/icons.h"
#include "physics/world.h"

#include <SpriteBatch.h>

using namespace gd;

class CustomComponent : public RootComponent {
public:
	// 物理演算の空間
	PhysicsWorld world;

	// 空間中のオブジェクト
	std::list<PhysicsObj> obj;

	// ドラッグ中のオブジェクト
	PhysicsObj drag_obj;

	// ウィンドウリサイズ時
    int width, height;
    void resize(int width, int height) override {
		this->width = width; this->height = height;
		world.resizeWorld(width, height);
	}

    void init(gd::Graph& graph) override {
        RootComponent::init(graph);

		// オブジェクトの生成
		for (int i = 0; i < 9; i++) {
			auto& o = obj.emplace_back(world.createObj(70.0f * i, 10.0f, 60.0f, 100.0f, PhysicsObjType::DYNAMIC));
		}
    }

    void render(gd::Graph& graph, const gd::Mouse& mouse, const gd::Keyboard& keyboard) override {
        RootComponent::render(graph, mouse, keyboard);
        if (keyboard.keys.count(VK_ESCAPE)) { closeWindow(); }
        graph.setRenderMode(BlendMode::AlphaBlend1, DepthMode::DepthNone, RasterizerMode::CullNone);
        graph.fill(0xF0F0F0, 0xFF);
        graph.rect(0, 0, width, height, 0.0f);

		// ドラッグ処理
		if (mouse.lRelease()) drag_obj.reset();
		if (mouse.lClick()) for (auto& o : obj) {
			if (o->isHit(mouse.point.x, mouse.point.y)) {
				drag_obj = o;
				break;
			}
		}
		if (drag_obj) drag_obj->spring(mouse.point.x, mouse.point.y);

		// 物理演算
		world.update(60.0f);

		// オブジェクト数分ループ
		for (auto& o : obj) {
			// 色の指定
			if (o == drag_obj) graph.fill(0xFF00FF, 0x7F);
			else if (o->isHit(mouse.point.x, mouse.point.y)) graph.fill(0xFF00FF, 0xFF);
			else graph.fill(0x404040, 0xFF);

			// 四角形の描画
			float r = o->getAngle();
			float x = o->getSize().x * 0.5;
			float y = o->getSize().y * 0.5;
			b2Vec2 p1 = o->getPosition() + b2Vec2(-std::cos(r) * x + std::sin(r) * y, -std::sin(r) * x - std::cos(r) * y);
			b2Vec2 p2 = o->getPosition() + b2Vec2(+std::cos(r) * x + std::sin(r) * y, +std::sin(r) * x - std::cos(r) * y);
			b2Vec2 p3 = o->getPosition() + b2Vec2(+std::cos(r) * x - std::sin(r) * y, +std::sin(r) * x + std::cos(r) * y);
			b2Vec2 p4 = o->getPosition() + b2Vec2(-std::cos(r) * x - std::sin(r) * y, -std::sin(r) * x + std::cos(r) * y);
			graph.beginShape(true, 0.f);
			graph.vertex(p1.x, p1.y);
			graph.vertex(p2.x, p2.y);
			graph.vertex(p3.x, p3.y);
			graph.vertex(p4.x, p4.y);
			graph.endShape(true);
		}
    }
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    Microsoft::WRL::Wrappers::RoInitializeWrapper initWrapper{RO_INIT_SINGLETHREADED};
    if (!SUCCEEDED(initWrapper)) return 3;
    int ret; gd::Windows windows{ hInstance, nCmdShow };
    ret = windows.create<CustomComponent>(); if (ret) return 1;
    ret = windows.waitUntilExit(); return ret;
}
