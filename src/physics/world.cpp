#include "physics/world.h"

using namespace gd;

PhysicsObj_::PhysicsObj_(std::shared_ptr<b2World> world, b2Vec2 position, b2Vec2 size, PhysicsObjType type, float pixel_per_meter)
	: world(world), size(size), pixel_per_meter(pixel_per_meter)
{
	// 矩形の生成
	b2BodyDef bodyDef;
	bodyDef.type = (PhysicsObjType::DYNAMIC == type) ? b2_dynamicBody : b2_staticBody;
	bodyDef.position = (1.0f / pixel_per_meter) * (position + (0.5 * size));
	bodyDef.angularDamping = 0.1f;
	b2PolygonShape bodyShape;
	bodyShape.SetAsBox(size.x * 0.5 / pixel_per_meter, size.y * 0.5 / pixel_per_meter);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &bodyShape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.8f;
	fixtureDef.restitution = 0.45f;
	body = world->CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
}

PhysicsObj_::~PhysicsObj_()
{
	destroy();  // 矩形の削除
}

void PhysicsObj_::destroy()
{
	if (!body) { return; }
	world->DestroyBody(body);
	body = nullptr;
}

void PhysicsObj_::spring(float x, float y, float stiffness, float damping)
{
	// オブジェクトの座標から移動の目標点までのベクトル
	b2Vec2 acceleration = b2Vec2{ x / pixel_per_meter, y / pixel_per_meter } - body->GetPosition();
	acceleration *= stiffness;                            // バネによる推進力
	acceleration -= damping * body->GetLinearVelocity();  // ダンパーによる抵抗力
	acceleration -= body->GetWorld()->GetGravity();       // 重力の影響を無視
	// 力の適応
	body->ApplyForce(body->GetMass() * acceleration, body->GetPosition(), true);
}

b2Vec2 PhysicsObj_::getPosition() const { return pixel_per_meter * body->GetPosition(); }

b2Vec2 PhysicsObj_::getSize() const { return size; }

float PhysicsObj_::getAngle() const { return body->GetAngle(); }

bool PhysicsObj_::isHit(float x, float y) const {
	const float r = -getAngle();
	const b2Mat22 rotate{
		std::cos(r), -std::sin(r),
		std::sin(r),  std::cos(r)
	};
	const b2Vec2 local_position = b2Mul(rotate, b2Vec2{ x, y } - getPosition());
	return (
		std::abs(local_position.x) <= (size.x * 0.5) &&
		std::abs(local_position.y) <= (size.y * 0.5)
	);
}

PhysicsWorld::PhysicsWorld(const float pixel_per_meter)
	: pixel_per_meter(pixel_per_meter), world(std::make_shared<b2World>(b2Vec2{ 0.0f, 0.0f }))
{
	// 画面サイズを設定
	resizeWorld(640.0f, 480.0f);
}

void PhysicsWorld::resizeWorld(float width, float height)
{
	// 既存の壁を削除
	walls.clear();

	// 全ての動的オブジェクトがリサイズ後の範囲内に収まるように再配置
	b2Body* b = world->GetBodyList();
	b2Vec2 size{ width / pixel_per_meter, height / pixel_per_meter };
	while (b) {
		if (b2_dynamicBody == b->GetType()) {
			b2Vec2 position = b->GetPosition();
			if (position.x >= size.x || position.y >= size.y) {
				position.x = std::min(position.x, size.x - b2_epsilon);
				position.y = std::min(position.y, size.y - b2_epsilon);
			}
			b->SetTransform(position, b->GetAngle());
		}
		b = b->GetNext();
	}

	// 左右上下の壁の生成
	const float wallThickness = 10.0f;                      // 壁の厚み
	const PhysicsObjType objType = PhysicsObjType::STATIC;  // オブジェクトのタイプ
	walls.emplace_back(createObj(-wallThickness, 0.0f          , wallThickness, height        , objType));
	walls.emplace_back(createObj(width         , 0.0f          , wallThickness, height        , objType)); 
	walls.emplace_back(createObj(0.0f          , -wallThickness, width        , wallThickness , objType)); 
	walls.emplace_back(createObj(0.0f          , height        , width        , wallThickness , objType)); 

	// 地球と同じ重力を設定
	setEarthGravity();
}

void PhysicsWorld::update(float fps, int32_t velocityIterations, int32_t positionIterations)
{
	world->Step(1.0f / fps, velocityIterations, positionIterations);
}

PhysicsObj PhysicsWorld::createObj(float x, float y, float width, float height, PhysicsObjType type)
{
	return std::make_shared<PhysicsObj_>(world, b2Vec2{ x, y }, b2Vec2{ width, height }, type, pixel_per_meter);
}

void PhysicsWorld::setGravity(float x, float y) { world->SetGravity(b2Vec2{ x / pixel_per_meter, y / pixel_per_meter }); }

void PhysicsWorld::setEarthGravity() { setGravity(0.0f, 9.81f * pixel_per_meter); }
