/*

����̓f�X�N�g�b�v�̃A�C�R�������擾����v���O�����ł��B

*/

#pragma once

#include "utils/pch.h"
#include "icons.h"
#include "physics/world.h"

namespace gd
{
	// �A�C�R���ƕ����I�u�W�F�N�g���܂Ƃ߂ĊǗ�����N���X
	class PhysicsIcons {
	private:
		std::unique_ptr<Icons> icons;
		PhysicsWorld& world;
		std::list<PhysicsObj> objs;
		std::list<PhysicsObj> taskbars;

	public:
		PhysicsIcons(PhysicsWorld& world, gd::FakeDesktopComponent::DesktopHwnds desk) : world(world)
		{
			// �A�C�R���̎擾
			icons = std::make_unique<Icons>(desk.listview);
			icons->update();

			// �A�C�R���I�u�W�F�N�g�̐���
			for (Icon& icon : *icons) {
				RECT area = icon->itemArea();
				objs.emplace_back(world.createObj(
					area.left, area.top, area.right - area.left, area.bottom - area.top,
					PhysicsObjType::DYNAMIC
				));
			}

			// �^�X�N�o�[�̕ǂ𐶐�
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
				�f�X�N�g�b�v�̃A�C�R�����X�V���ꂽ�Ƃ���
				�^�X�N�o�[�̈ʒu���ύX���ꂽ�Ƃ��Ȃǂ�
				�I�u�W�F�N�g�̍��W���X�V����
			*/
			//icons->update();
		}

		// �S�Ă�I��
		void allselect() { icons->allselect(); }

		// �S�Ă̑I��������
		void unselect() { icons->unselect(); }

		// �S�Ẵn�C���C�g������
		void unhot() { icons->unhot(); }

		struct IconObj {
			gd::Icon icon; PhysicsObj obj;
			IconObj() : icon(nullptr), obj(nullptr) {}
			IconObj(gd::Icon icon, PhysicsObj obj) : icon(icon), obj(obj) {}
		};

		// �S�ẴA�C�R���ɑ΂��ă��[�v
		void forEach(std::function<void(Icon, PhysicsObj)> callback) {
			std::list<PhysicsObj>::iterator objIter = objs.begin();
			for (gd::Icon icon : *icons) {
				if (objIter == objs.end()) break;
				callback(icon, *objIter);
				objIter++;
			}
		}

		// �I�����ꂽ�A�C�R���ɑ΂��ă��[�v
		void forEachSelected(std::function<void(Icon, PhysicsObj)> callback) {
			std::list<PhysicsObj>::iterator objIter = objs.begin();
			for (gd::Icon& icon : *icons) {
				if (objIter == objs.end()) break;
				if (icon->isSelect()) callback(icon, *objIter);
				objIter++;
			}
		}

		// �I������Ă��Ȃ��A�C�R���ɑ΂��ă��[�v
		void forEachUnselected(std::function<void(Icon, PhysicsObj)> callback) {
			std::list<PhysicsObj>::iterator objIter = objs.begin();
			for (gd::Icon& icon : *icons) {
				if (objIter == objs.end()) break;
				if (!icon->isSelect()) callback(icon, *objIter);
				objIter++;
			}
		}

		// �w�肳�ꂽ���W�̒����ɂ���A�C�R���̎擾
		std::optional<IconObj> hitTest(float x, float y) {
			std::list<PhysicsObj>::iterator objIter = objs.begin();
			for (gd::Icon icon : *icons) {
				if (objIter == objs.end()) break;
				if ((*objIter)->isHit(x, y)) return IconObj(icon, *objIter);
				objIter++;
			}
			return std::nullopt;
		}

		// �����ʒu�ɃX���[�Y�Ɉړ�����
		// �ړ������������true���A��
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
