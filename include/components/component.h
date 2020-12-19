#pragma once

#include "utils/pch.h"
#include "components/graph.h"
#include "components/mouse.h"

namespace gd
{
	class Component
	{
	public:
		Component() {};
		virtual ~Component() {};

		virtual void init(Graph& graph) {}
		virtual void render(Graph& graph, Mouse& mouse) {}
		virtual void exit(Graph& graph) {}
		virtual void resize(int width, int height) {}
		virtual std::string getTitle() const { return "default component"; }
	};
}