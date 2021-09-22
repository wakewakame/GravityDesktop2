#pragma once

#include "utils/pch.h"

namespace gd
{
	enum class PhysicsObjType
	{
		STATIC,
		DYNAMIC
	};

	class PhysicsObj_
	{
	private:
		std::shared_ptr<b2World> world;  // Box2D空間
		b2Body *body = nullptr;          // 矩形 (ポインタの解放はbox2dが担う)
		b2Vec2 size;                     // 矩形のサイズ
		const float pixel_per_meter;     // 1メートルあたりのピクセル数

	public:
		PhysicsObj_(std::shared_ptr<b2World> world, b2Vec2 position, b2Vec2 size, PhysicsObjType type, float pixel_per_meter);
		virtual ~PhysicsObj_();
		void destroy();
		void spring(float x, float y, float stiffness = 160.0f, float damping = 5.0f);
		b2Vec2 getPosition() const;
		b2Vec2 getSize() const;
		float getAngle() const;
		bool isHit(float x, float y) const;
	};
	using PhysicsObj = std::shared_ptr<PhysicsObj_>;

	class PhysicsWorld
	{
	private:
		std::shared_ptr<b2World> world;  // Box2D空間
		const float pixel_per_meter;     // 1メートルあたりのピクセル数
		std::vector<PhysicsObj> walls;   // 壁

		/*
			メモ
			Box2Dは0.1〜10メートルの移動形状でうまく機能するように調整されている。
			そのため、ピクセル単位の座標をそのままBox2Dでも使用することは推奨されない。
			よって、画面とBox2Dのスケールをpixel_per_meterによって変換する。
		*/

	public:
		PhysicsWorld(const float pixel_per_meter = 200.0f);
		void resizeWorld(float width, float height);
		void update(float fps = 60.0f, int32_t velocityIterations = 6, int32_t positionIterations = 2);
		PhysicsObj createObj(float x, float y, float width, float height, PhysicsObjType type);
		void setGravity(float x, float y);
		void setEarthGravity();
	};
}
