#pragma once

#include "components/component.h"

namespace gd
{
	class Window;

	class RootComponent : public Component
	{
	public:
		RootComponent();
		virtual ~RootComponent();

		virtual void init(gd::Graph& graph) override;
		virtual void update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard) override;
		virtual void render(gd::Graph& graph) override;
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
		virtual double getFps() const;
		virtual void closeWindow() final;

	protected:
		HINSTANCE hInstance = nullptr;
		HWND hWnd = nullptr;
		Window* gdWindow = nullptr;
	};
}