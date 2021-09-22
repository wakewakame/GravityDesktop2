#include "physics/world.h"

using namespace gd;

PhysicsObj_::PhysicsObj_(
	std::shared_ptr<b2World> world,
	b2Vec2 position, b2Vec2 size, PhysicsObjType type,
	float pixel_per_meter,
	uint16_t categoryBits, uint16_t maskBits
)
	: world(world), size(size), pixel_per_meter(pixel_per_meter)
{
	// ��`�̐���
	b2BodyDef bodyDef;
	bodyDef.type =
		(PhysicsObjType::STATIC  == type) ? b2_staticBody   :
		(PhysicsObjType::DYNAMIC == type) ? b2_dynamicBody  :
		                                    b2_kinematicBody;
	bodyDef.position = (1.0f / pixel_per_meter) * (position + (0.5 * size));
	bodyDef.angularDamping = 5.0f;
	//bodyDef.allowSleep = false;
	b2PolygonShape bodyShape;
	bodyShape.SetAsBox(size.x * 0.5 / pixel_per_meter, size.y * 0.5 / pixel_per_meter);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &bodyShape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.8f;
	fixtureDef.restitution = 0.45f;
	fixtureDef.filter.categoryBits = categoryBits;
	fixtureDef.filter.maskBits = maskBits;
	body = world->CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
}

PhysicsObj_::~PhysicsObj_()
{
	destroy();  // ��`�̍폜
}

void PhysicsObj_::destroy()
{
	if (!body) { return; }
	world->DestroyBody(body);
	body = nullptr;
}

void PhysicsObj_::spring(float x, float y, float stiffness, float damping)
{
	// �I�u�W�F�N�g�̍��W����ړ��̖ڕW�_�܂ł̃x�N�g��
	b2Vec2 acceleration = b2Vec2{ x / pixel_per_meter, y / pixel_per_meter } - body->GetPosition();
	acceleration *= stiffness;                            // �o�l�ɂ�鐄�i��
	acceleration -= damping * body->GetLinearVelocity();  // �_���p�[�ɂ���R��
	acceleration -= body->GetWorld()->GetGravity();       // �d�͂̉e���𖳎�
	// �͂̓K��
	body->ApplyForce(body->GetMass() * acceleration, body->GetPosition(), true);
}

b2Vec2 PhysicsObj_::getPosition() const { return pixel_per_meter * body->GetPosition(); }

b2Vec2 PhysicsObj_::getSize() const { return size; }

float PhysicsObj_::getAngle() const { return body->GetAngle(); }

void PhysicsObj_::setPosition(float x, float y) {
	body->SetAwake(true);
	body->SetLinearVelocity(60.0f * (b2Vec2{ x / pixel_per_meter, y / pixel_per_meter } - body->GetPosition()));
}

b2Vec2 PhysicsObj_::getLocalPosition(float x, float y) const {
	const float r = -getAngle();
	const b2Mat22 rotate{
		std::cos(r), -std::sin(r),
		std::sin(r),  std::cos(r)
	};
	const b2Vec2 localPosition = b2Mul(rotate, b2Vec2{ x, y } - getPosition());
	return localPosition;
}

bool PhysicsObj_::isHit(float x, float y) const {
	const b2Vec2 local_position = getLocalPosition(x, y);
	return (
		std::abs(local_position.x) <= (size.x * 0.5) &&
		std::abs(local_position.y) <= (size.y * 0.5)
	);
}

b2Body* PhysicsObj_::getb2Body() { return body; }

PhysicsPicker_::PhysicsPicker_(
	std::shared_ptr<b2World> world,
	PhysicsObj obj, b2Vec2 anchor,
	float pixel_per_meter
) : obj(obj)
{
	targetObj = std::make_shared<PhysicsObj_>(
		world, anchor, b2Vec2{ 0.0f, 0.0f }, PhysicsObjType::KINEMATIC, pixel_per_meter,
		0x0001, 0x0000
	);
	const b2Vec2 local_anchor = (1.0f / pixel_per_meter) * obj->getLocalPosition(anchor.x, anchor.y);
	b2RevoluteJointDef jointDef;
	jointDef.bodyA = obj->getb2Body();
	jointDef.localAnchorA = local_anchor;
	jointDef.bodyB = targetObj->getb2Body();
	jointDef.localAnchorB = b2Vec2{ 0.0f, 0.0f };
	joint = world->CreateJoint(&jointDef);
}
PhysicsPicker_::~PhysicsPicker_() {
	destroy();
}
void PhysicsPicker_::destroy() {
	if (obj) obj.reset();
	if (targetObj) targetObj.reset();

	/*
		����
		b2Joint�Ɍq���Ă���b2Body�̂���1�ł�destroy������
		�q�����Ă���b2Joint�������I��destroy�����B
		Box2D�͌����B
	*/
}
void PhysicsPicker_::setPosition(float x, float y) {
	targetObj->setPosition(x, y);
}

PhysicsObj PhysicsPicker_::getObj() { return obj; }

PhysicsWorld::PhysicsWorld(const float pixel_per_meter)
	: pixel_per_meter(pixel_per_meter), world(std::make_shared<b2World>(b2Vec2{ 0.0f, 0.0f }))
{
	// ��ʃT�C�Y��ݒ�
	resizeWorld(640.0f, 480.0f);
}

void PhysicsWorld::resizeWorld(float width, float height)
{
	// �����̕ǂ��폜
	walls.clear();

	// �S�Ă̓��I�I�u�W�F�N�g�����T�C�Y��͈͓̔��Ɏ��܂�悤�ɍĔz�u
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

	// ���E�㉺�̕ǂ̐���
	const float wallThickness = 10.0f;                      // �ǂ̌���
	const PhysicsObjType objType = PhysicsObjType::STATIC;  // �I�u�W�F�N�g�̃^�C�v
	walls.emplace_back(createObj(-wallThickness, 0.0f          , wallThickness, height        , objType));
	walls.emplace_back(createObj(width         , 0.0f          , wallThickness, height        , objType)); 
	walls.emplace_back(createObj(0.0f          , -wallThickness, width        , wallThickness , objType)); 
	walls.emplace_back(createObj(0.0f          , height        , width        , wallThickness , objType)); 

	// �n���Ɠ����d�͂�ݒ�
	setEarthGravity();
}

void PhysicsWorld::update(float fps, int32_t velocityIterations, int32_t positionIterations)
{
	world->Step(1.0f / fps, velocityIterations, positionIterations);
}

PhysicsObj PhysicsWorld::createObj(
	float x, float y, float width, float height, PhysicsObjType type,
	uint16_t categoryBits, uint16_t maskBits
)
{
	return std::make_shared<PhysicsObj_>(
		world, b2Vec2{ x, y }, b2Vec2{ width, height }, type, pixel_per_meter,
		categoryBits, maskBits
	);
}

void PhysicsWorld::setGravity(float x, float y) { world->SetGravity(b2Vec2{ x / pixel_per_meter, y / pixel_per_meter }); }

void PhysicsWorld::setEarthGravity() { setGravity(0.0f, 9.81f * pixel_per_meter); }
