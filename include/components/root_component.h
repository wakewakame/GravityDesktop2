#pragma once

#include "components/component.h"

namespace gd
{
	class Window;

	enum class WM_APP_LIST : UINT
	{
		EXIT = WM_APP + 0x0000
	};

	class RootComponent : public Component
	{
	public:
		RootComponent();
		virtual ~RootComponent();

		virtual void init(gd::Graph& graph) override;
		virtual void render(gd::Graph& graph, gd::Mouse& mouse) override;
		virtual void exit(Graph& graph) override;
		virtual void resize(int width, int height) override;

		virtual void setInstance(HINSTANCE hInstance) final;
		virtual HINSTANCE getInstance() final;
		virtual void setHwnd(HWND hWnd) final;
		virtual HWND getHwnd() final;
		virtual std::string getTitle() const override;
		virtual SIZE getDefaultSize() const;
		virtual DWORD getWindowStyle() const;
		virtual UINT getWindowClassStyle() const;
		virtual HICON getIcon() const;
		virtual void setGDWindow(Window* gdWindow) final;
		virtual void setSize(int width, int height) final;
		virtual void closeWindow() final;

	protected:
		HINSTANCE hInstance = nullptr;
		HWND hWnd = nullptr;
		Window* gdWindow = nullptr;
	};
}