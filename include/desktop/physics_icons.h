/*

これはデスクトップのアイコン情報を取得するプログラムです。

*/

#pragma once

#include "utils/pch.h"
#include "icons.h"
#include "physics/world.h"

namespace gd
{
	// アイコンと物理オブジェクトをまとめて管理するクラス
	class PhysicsIcons {
	private:
		std::unique_ptr<Icons> icons;
		PhysicsWorld& world;
		std::list<PhysicsObj> objs;
		std::list<PhysicsObj> taskbars;

	public:
		PhysicsIcons(PhysicsWorld& world, gd::FakeDesktopComponent::DesktopHwnds desk) : world(world)
		{
			// アイコンの取得
			icons = std::make_unique<Icons>(desk.listview);
			icons->update();

			// アイコンオブジェクトの生成
			for (Icon& icon : *icons) {
				RECT area = icon->itemArea();
				objs.emplace_back(world.createObj(
					area.left, area.top, area.right - area.left, area.bottom - area.top,
					PhysicsObjType::DYNAMIC
				));
			}

			// タスクバーの壁を生成
			const UINT dpiA = 96;
			const UINT dpiB = GetDpiForWindow(desk.listview);
			RECT listview_rc; GetWindowRect(desk.listview, &listview_rc);
			const POINT origin{ listview_rc.left * dpiB / dpiA, listview_rc.top * dpiB / dpiA };
			for (HWND taskbar : desk.taskbars) {
				RECT taskbar_rc; GetWindowRect(taskbar, &taskbar_rc);
				const UINT dpiC = GetDpiForWindow(taskbar);
				taskbar_rc.left   = taskbar_rc.left    * dpiC / dpiA;
				taskbar_rc.top    = taskbar_rc.top     * dpiC / dpiA;
				taskbar_rc.right  = taskbar_rc.right   * dpiC / dpiA;
				taskbar_rc.bottom = taskbar_rc.bottom  * dpiC / dpiA;
				taskbars.emplace_back(world.createObj(
					taskbar_rc.left - origin.x, taskbar_rc.top - origin.y,
					taskbar_rc.right - taskbar_rc.left, taskbar_rc.bottom - taskbar_rc.top,
					PhysicsObjType::STATIC
				));
			}
		}

		void update() {
			/*
				Todo
				デスクトップのアイコンが更新されたときや
				タスクバーの位置が変更されたときなどに
				オブジェクトの座標を更新する
			*/
			//icons->update();
		}

		// 全てを選択
		void allselect() { icons->allselect(); }

		// 全ての選択を解除
		void unselect() { icons->unselect(); }

		// 全てのハイライトを解除
		void unhot() { icons->unhot(); }

		struct IconObj {
			gd::Icon icon; PhysicsObj obj;
			IconObj() : icon(nullptr), obj(nullptr) {}
			IconObj(gd::Icon icon, PhysicsObj obj) : icon(icon), obj(obj) {}
		};

		// 全てのアイコンに対してループ
		void forEach(std::function<void(Icon, PhysicsObj)> callback) {
			std::list<PhysicsObj>::iterator objIter = objs.begin();
			for (gd::Icon icon : *icons) {
				if (objIter == objs.end()) break;
				callback(icon, *objIter);
				objIter++;
			}
		}

		// 選択されたアイコンに対してループ
		void forEachSelected(std::function<void(Icon, PhysicsObj)> callback) {
			std::list<PhysicsObj>::iterator objIter = objs.begin();
			for (gd::Icon& icon : *icons) {
				if (objIter == objs.end()) break;
				if (icon->isSelect()) callback(icon, *objIter);
				objIter++;
			}
		}

		// 選択されていないアイコンに対してループ
		void forEachUnselected(std::function<void(Icon, PhysicsObj)> callback) {
			std::list<PhysicsObj>::iterator objIter = objs.begin();
			for (gd::Icon& icon : *icons) {
				if (objIter == objs.end()) break;
				if (!icon->isSelect()) callback(icon, *objIter);
				objIter++;
			}
		}

		// 指定された座標の直下にあるアイコンの取得
		std::optional<IconObj> hitTest(float x, float y) {
			std::list<PhysicsObj>::iterator objIter = objs.begin();
			for (gd::Icon icon : *icons) {
				if (objIter == objs.end()) break;
				if ((*objIter)->isHit(x, y)) return IconObj(icon, *objIter);
				objIter++;
			}
			return std::nullopt;
		}

		// 初期位置にスムーズに移動する
		// 移動が完了するとtrueが帰る
		bool transformDefault(bool flag, float speed = 0.8f, float threshold = 0.5f) {
			static const float radToDeg = 180.0f / acos(-1.0f);
			float error = 0.0f;
			if (flag) {
				const float rate = std::pow(std::pow(speed, 60.0f), 1.0f / world.getFps());
				forEach([&](Icon icon, PhysicsObj obj) {
					b2Vec2 target{
						static_cast<float>(icon->itemArea().left + icon->itemArea().right) * 0.5f,
						static_cast<float>(icon->itemArea().top + icon->itemArea().bottom) * 0.5f
					};
					obj->setEnabled(false);
					b2Vec2 current = obj->getPosition();
					b2Vec2 position = (1.0f - rate) * target + rate * current;
					float angle = obj->getAngle() * rate;
					obj->setTransform(position.x, position.y, angle);
					error += (position - target).Length() + std::abs(angle * radToDeg);
				});
				error /= objs.size();
				return error <= threshold;
			}
			else forEach([&](Icon icon, PhysicsObj obj) { obj->setEnabled(true); });
			return false;
		}
	};
}
