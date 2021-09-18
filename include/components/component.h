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

		virtual void init(Graph& graph) {}
		virtual void render(gd::Graph& graph, const gd::Mouse& mouse, const gd::Keyboard& keyboard) {}
		virtual void exit(Graph& graph) {}
		virtual void resize(int width, int height) {}
		virtual std::string getTitle() const { return "default component"; }
	};
}