#pragma once

#include "utils/pch.h"

namespace gd
{
	enum class PhysicsObjType
	{
		STATIC,
		DYNAMIC,
		KINEMATIC
	};

	struct PhysicsWorldStatus_ {
		float fps;
		const float pixel_per_meter;
		PhysicsWorldStatus_(float fps, float pixel_per_meter);
	};
	using PhysicsWorldStatus = std::shared_ptr<PhysicsWorldStatus_>;

	class PhysicsObj_
	{
	private:
		std::shared_ptr<b2World> world;  // Box2D空間
		b2Body *body = nullptr;          // 矩形 (ポインタの解放はbox2dが担う)
		b2Vec2 size;                     // 矩形のサイズ
		PhysicsWorldStatus status;       // PhysicsWorldの情報

	public:
		PhysicsObj_(
			std::shared_ptr<b2World> world,
			b2Vec2 position, b2Vec2 size, PhysicsObjType type,
			PhysicsWorldStatus status,
			uint16_t categoryBits = 0x0001, uint16_t maskBits = 0xFFFF
		);
		virtual ~PhysicsObj_();
		void destroy();
		void spring(float x, float y, float stiffness = 160.0f, float damping = 5.0f);
		b2Vec2 getPosition() const;
		b2Vec2 getSize() const;
		float getAngle() const;
		void setPosition(float x, float y);
		b2Vec2 getLocalPosition(float x, float y) const;
		void setTransform(float x, float y, float angle);
		void setEnabled(bool flag);
		bool isHit(float x, float y) const;
		b2Body* getb2Body();
	};
	using PhysicsObj = std::shared_ptr<PhysicsObj_>;

	class PhysicsPicker_ {
	private:
		PhysicsObj obj;            // つままれるオブジェクト
		PhysicsObj targetObj;      // 移動の目標となるオブジェクト
		b2Joint *joint = nullptr;  // 結合 (ポインタの解放はbox2dが担う)

	public:
		PhysicsPicker_(std::shared_ptr<b2World> world, PhysicsObj obj, b2Vec2 anchor, PhysicsWorldStatus status);
		virtual ~PhysicsPicker_();
		void destroy();
		void setPosition(float x, float y);
		PhysicsObj getObj();
	};
	using PhysicsPicker = std::shared_ptr<PhysicsPicker_>;

	class PhysicsWorld
	{
	private:
		std::shared_ptr<b2World> world;  // Box2D空間
		PhysicsWorldStatus status;       // PhysicsWorldの情報
		std::vector<PhysicsObj> walls;   // 壁

		/*
			メモ
			Box2Dは0.1〜10メートルの移動形状でうまく機能するように調整されている。
			そのため、ピクセル単位の座標をそのままBox2Dでも使用することは推奨されない。
			よって、画面とBox2Dのスケールをpixel_per_meterによって変換する。
		*/

	public:
		PhysicsWorld(float fps = 60.0f, float pixel_per_meter = 200.0f);
		void resizeWorld(float width, float height);
		void update(int32_t velocityIterations = 6, int32_t positionIterations = 2);
		PhysicsObj createObj(
			float x, float y, float width, float height, PhysicsObjType type,
			uint16_t categoryBits = 0x0001, uint16_t maskBits = 0xFFFF
		);
		void setFps(float fps);
		float getFps();
		void setGravity(float x, float y);
		void setEarthGravity();
		PhysicsPicker createPicker(PhysicsObj& obj, float x, float y);
		void wakeUpAll();
	};
}
