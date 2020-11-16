#pragma once

#include "pch.h"
#include "graph.h"
#include "mouse.h"

namespace gd
{
	enum class WM_APP_LIST : UINT
	{
		EXIT = WM_APP + 0x0000
	};

	class Component
	{
	public:
		Component() {};
		virtual ~Component() {};

		virtual void init(Graph& graph) {}
		virtual void render(Graph& graph, Mouse& mouse) {}
		virtual std::string getTitle() const { return "default component"; }
	};

	class RootComponent : public Component
	{
	public:
		RootComponent() {};
		virtual ~RootComponent() {};

		virtual void setInstance(HINSTANCE hInstance) final { this->hInstance = hInstance; }
		virtual HINSTANCE getInstance() final { return hInstance; }
		virtual void setHwnd(HWND hWnd) final { this->hWnd = hWnd; }
		virtual HWND getHwnd() final { return hWnd; }
		virtual std::string getTitle() const override { return "window component"; }
		virtual SIZE getDefaultSize() const { return SIZE{ 640, 480 }; }
		virtual DWORD getWindowStyle() const { return WS_OVERLAPPEDWINDOW; }
		virtual UINT getWindowClassStyle() const { return CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; }
		virtual HICON getIcon() const { return LoadIconW(nullptr, L"IDI_APPLICATION"); }
		virtual void closeWindow() final {
			if (hWnd) PostMessageW(hWnd, static_cast<UINT>(WM_APP_LIST::EXIT), 0, 0);
		}

	protected:
		HINSTANCE hInstance = nullptr;
		HWND hWnd = nullptr;
	};
}