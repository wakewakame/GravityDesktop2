#pragma once

#include "utils/pch.h"
#include "components/graph.h"
#include "components/mouse.h"
#include "components/keyboard.h"

namespace gd
{
	class Component
	{
	public:
		Component() {};
		virtual ~Component() {};

		// �������ɌĂяo�����
		virtual void init(Graph& graph) {}

		// update�͈��̃t���[�����[�g�ŌĂяo�����
		virtual void update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard) {}

		// render��WM_PAINT�������Ă���x�ɌĂяo�����
		virtual void render(gd::Graph& graph) {}

		// �I�����ɌĂяo�����
		virtual void exit(Graph& graph) {}

		// ���T�C�Y���ɌĂяo�����
		virtual void resize(int width, int height) {}

		// �R���|�[�l���g�������߂�
		virtual std::string getTitle() const { return "default component"; }
	};
}