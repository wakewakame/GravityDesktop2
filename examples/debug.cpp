#include "components/dx_windows.h"
#include "components/component.h"
#include "desktop/capture.h"
#include "desktop/fake_desktop.h"
#include "desktop/icons.h"

#include <SpriteBatch.h>
#include <box2d/box2d.h>

#include <memory>
#include <cmath>

using namespace gd;

class PhysicsObj;
class PhysicsWorld;

using gdVec1 = float;
using gdVec2 = DirectX::SimpleMath::Vector2;
using gdVec3 = DirectX::SimpleMath::Vector3;
using gdVec4 = DirectX::SimpleMath::Vector4;

class gdBox {
private:
	gdVec4 rect_var = gdVec4(0.0f, 0.0f, 0.0f, 0.0f);
	gdVec2 position_var = gdVec2(0.0f, 0.0f);
	gdVec1 angle_var = 0.0f;

public:
	gdBox() {}
	gdBox(gdVec2 set_size, gdVec2 set_position = gdVec2(0.0f, 0.0f), gdVec1 set_angle = 0.0f) {
		size(set_size.x, set_size.y);
		position(set_position.x, set_position.y);
		angle(set_angle);
	}
	gdBox(gdVec4 set_rect, gdVec1 set_angle = 0.0f) {
		rect(set_rect.x, set_rect.y, set_rect.z, set_rect.w);
		angle(set_angle);
	}
	// rect
	inline void rect(gdVec4 set_rect) {
		rect_var = set_rect;
		position_var.x = (rect_var.z + rect_var.x) / 2.0f;
		position_var.y = (rect_var.w + rect_var.y) / 2.0f;
	}
	inline void rect(gdVec1 x, gdVec1 y, gdVec1 z, gdVec1 w) {
		rect(gdVec4(x, y, z, w));
	}
	inline gdVec4 rect() {
		return rect_var;
	}
	// position
	inline void position(gdVec2 set_pos) {
		rect_var.x += (set_pos.x - position_var.x);
		rect_var.y += (set_pos.y - position_var.y);
		rect_var.z += (set_pos.x - position_var.x);
		rect_var.w += (set_pos.y - position_var.y);
		position_var = set_pos;
	}
	inline void position(gdVec1 x, gdVec1 y) {
		position(gdVec2(x, y));
	}
	inline gdVec2 position() {
		return position_var;
	}
	// size
	inline void size(gdVec2 set_size) {
		rect_var.z = rect_var.x + set_size.x;
		rect_var.w = rect_var.y + set_size.y;
		position_var.x = (rect_var.z + rect_var.x) / 2.0f;
		position_var.y = (rect_var.w + rect_var.y) / 2.0f;
	}
	inline void size(gdVec1 x, gdVec1 y) {
		size(gdVec2(x, y));
	}
	inline gdVec2 size() {
		return gdVec2(rect_var.z - rect_var.x, rect_var.w - rect_var.y);
	}
	// angle
	inline void angle(gdVec1 x) {
		angle_var = x;
	}
	inline gdVec1 angle() {
		return angle_var;
	}
	// for Box2d
	inline void b2position(b2Vec2 set_position) { position(set_position.x, set_position.y); }
	inline void b2size(b2Vec2 set_size) { size(set_size.x, set_size.y); }
	inline b2Vec2 b2position() { return b2Vec2(position_var.x, position_var.y); }
	inline b2Vec2 b2size() { return b2Vec2(size().x, size().x); }
};

class PhysicsObj {
private:
	float rate = 1.0f;
	PhysicsWorld *gd_world = nullptr;
	b2World *world = nullptr;
	b2BodyDef bodyDef;
	b2PolygonShape bodySize;
	b2FixtureDef fixtureDef;
	b2Body *body = nullptr;
	b2Vec2 position = b2Vec2(0.0f, 0.0f);
	float angle = 0.0f;
	gdBox size;

public:
	void init(PhysicsWorld* set_world, gdBox set_size, bool dynamic);
	~PhysicsObj() {}
	void come(float x, float y) {
		// (x,y)に等速で近づく
		body->SetLinearVelocity(b2Vec2(
			(float)(x - size.position().x) * rate * 10.0f,
			(float)(y - size.position().y) * rate * 10.0f
		));
	}
	void spring(float x, float y) {
		// (x,y)にバネのように近づく
		// 変数初期化
		b2Vec2 move_ivec;
		float move_length;
		// 計算
		move_ivec.x = (x - size.position().x) * rate;
		move_ivec.y = (y - size.position().y) * rate;
		move_length = move_ivec.Length();
		move_ivec *= (1.0f / move_length);
		move_ivec = 100.0f * move_length * body->GetMass() * move_ivec;
		// 重力無効化
		move_ivec -= (body->GetMass() * world->GetGravity());
		// 速度減衰率変更
		body->SetLinearDamping(5.0f);
		// 力の適応
		body->ApplyForce(move_ivec, body->GetPosition(), 1);
	}
	void update() {
		const double pi = std::acos(-1.0);
		const double pi2 = 2.0 * pi;

		// 速度減衰率リセット
		body->SetLinearDamping(0.0f);
		// Box2D座標、角度取得
		size.b2position((1 / rate)*(body->GetPosition()));
		size.angle(body->GetAngle());
		size.angle(size.angle() - (gdVec1)pi2*(gdVec1)std::floor((size.angle() + pi) / pi2)); // (-pi<=body[i].agl<pi)に変換
	}
	void release() {
		if (body != nullptr) world->DestroyBody(body);
	}
	inline gdBox getBox() { 
		return size; 
	}
};

class PhysicsWorld {
private:
	float max_size = 5.0f;    // Box2Dの最大空間サイズ
	float rate = 1.0f;        // Box2D空間単位/ユーザー空間単位
	DWORD now_time = 0;       // フレームレート取得用の現在時刻(ms単位)
	DWORD before_time = 0;    // フレームレート取得用の1フレーム前時刻(ms単位)
	double fps = 60.0;        // 現在のフレームレート
	b2World *world = nullptr; // Box2D空間
	gdBox size;               // ユーザー空間サイズ
	b2Vec2 gravity = b2Vec2(0.0f, 9.8f); // 重力設定
	std::unique_ptr<PhysicsObj> left_wall;    // 左壁
	std::unique_ptr<PhysicsObj> top_wall;     // 上壁
	std::unique_ptr<PhysicsObj> right_wall;   // 右壁
	std::unique_ptr<PhysicsObj> bottom_wall;  // 下壁

public:
	bool init(float size_x, float size_y) {
		// 代入
		size.size(size_x, size_y);
		rate = max_size / size.size().y; // Box2Dの空間の縦軸最大値をmax_sizeに設定					
		gravity = b2Vec2(0.0, 9.8f); // 重力設定
		// 空間生成
		world = new b2World(gravity);
		// 壁生成
		left_wall = std::unique_ptr<PhysicsObj>(new PhysicsObj);
		top_wall = std::unique_ptr<PhysicsObj>(new PhysicsObj);
		right_wall = std::unique_ptr<PhysicsObj>(new PhysicsObj);
		bottom_wall = std::unique_ptr<PhysicsObj>(new PhysicsObj);
		left_wall->init(this, gdBox({ 10.0f,size.size().y }, { 0.0f - 5.0f, size.size().y / 2.0f }), false);
		top_wall->init(this, gdBox({ size.size().x,10.0f }, { size.size().x / 2.0f, 0.0f - 5.0f }), false);
		right_wall->init(this, gdBox({ 10.0f,size.size().y }, { size.size().x + 5.0f, size.size().y / 2.0f }), false);
		bottom_wall->init(this, gdBox({ size.size().x,10.0f }, { size.size().x / 2.0f, size.size().y + 5.0f }), false);
		return 0;
	}
	bool init(HWND hwnd) {
		RECT rc;
		GetWindowRect(hwnd, &rc);
		return init(rc.right - rc.left, rc.bottom - rc.top);
	}
	void update(bool stop) {
		// Box2D更新
		if (!stop) {
			world->Step(
				(float)(1.0 / 60.0),
				(int32)(60.0),
				2
			);
		}
	}
	void setGravity(float x, float y) {
		world->SetGravity(rate * b2Vec2(x, y));
	}
	void resetGravity() {
		world->SetGravity(gravity);
	}
	inline b2World* getWorld() {
		return world;
	}
	inline float getRate() { return rate; }
	inline double getFps() { return fps; }
	inline gdBox getBox() { return size; }
	inline b2Vec2 getDefaultGravity() { return gravity; };
};

void PhysicsObj::init(PhysicsWorld *set_world, gdBox set_size, bool dynamic) {
	// 初期化
	gd_world = set_world;
	world = gd_world->getWorld();
	rate = gd_world->getRate();
	size = set_size;
	// 定義
	bodyDef.type = dynamic ? b2_dynamicBody : b2_staticBody;
	bodyDef.allowSleep = false;
	bodyDef.linearDamping = 0.0f;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.8f;
	fixtureDef.restitution = 0.45f;
	bodyDef.position.Set(
		size.position().x * rate,
		size.position().y * rate
	);
	bodySize.SetAsBox(
		size.size().x * rate / 2.0f,
		size.size().y * rate / 2.0f
	);
	fixtureDef.shape = &bodySize;
	// 生成
	body = world->CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
	// 更新
	update();
}

void rect(gd::Graph& g, gdBox& box) {
	float r = box.angle();
	float x = box.size().x * 0.5;
	float y = box.size().y * 0.5;
	gdVec2 p1 = box.position() + gdVec2(-std::cos(r) * x + std::sin(r) * y, -std::sin(r) * x - std::cos(r) * y);
	gdVec2 p2 = box.position() + gdVec2(+std::cos(r) * x + std::sin(r) * y, +std::sin(r) * x - std::cos(r) * y);
	gdVec2 p3 = box.position() + gdVec2(+std::cos(r) * x - std::sin(r) * y, +std::sin(r) * x + std::cos(r) * y);
	gdVec2 p4 = box.position() + gdVec2(-std::cos(r) * x - std::sin(r) * y, -std::sin(r) * x + std::cos(r) * y);
    g.beginShape(true, 0.f);
    g.vertex(p1.x, p1.y);
    g.vertex(p2.x, p2.y);
    g.vertex(p3.x, p3.y);
    g.vertex(p4.x, p4.y);
    g.endShape(true);
}

class CustomComponent : public RootComponent {
public:
	PhysicsWorld world;
	std::vector<PhysicsObj> obj;
    int width, height;
    void resize(int width, int height) override { this->width = width; this->height = height; }
    void init(gd::Graph& graph) override {
        RootComponent::init(graph);

		world.init(width, height);
		for (int i = 0; i < 10; i++) {
			auto& o = obj.emplace_back();
			o.init(&world, gdBox{ gdVec2{ 10.0f, 10.0f }, gdVec2{ 10.0f * i, 10.0f } }, true);
		}
    }
    void render(gd::Graph& graph, const gd::Mouse& mouse, const gd::Keyboard& keyboard) override {
        RootComponent::render(graph, mouse, keyboard);
        if (keyboard.keys.count(VK_ESCAPE)) { closeWindow(); }

        graph.setRenderMode(BlendMode::Opaque, DepthMode::DepthNone, RasterizerMode::CullNone);

        graph.fill(0xFFFFFF, 0xFF);
        graph.rect(0, 0, width, height, 0.0f);

        graph.fill(0x000000, 0xFF);
		world.update(false);
		for (auto & o : obj) {
			o.update();
			auto box = o.getBox();
			//graph.rect(box.position().x, box.position().y, box.position().x + 10, box.position().y + 10);
			rect(graph, box);
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
