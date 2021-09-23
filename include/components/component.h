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

		// 生成時に呼び出される
		virtual void init(Graph& graph) {}

		// updateは一定のフレームレートで呼び出される
		virtual void update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard) {}

		// renderはWM_PAINTが送られてくる度に呼び出される
		virtual void render(gd::Graph& graph) {}

		// 終了時に呼び出される
		virtual void exit(Graph& graph) {}

		// リサイズ時に呼び出される
		virtual void resize(int width, int height) {}

		// コンポーネント名を決める
		virtual std::string getTitle() const { return "default component"; }
	};
}