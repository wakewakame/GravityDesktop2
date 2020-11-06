#pragma once

#include "pch.h"
#include "StepTimer.h"
#include "graph.h"

namespace gd
{
	class Window
	{
	public:

		Window() noexcept;
		~Window() = default;

		Window(Window&&) = default;
		Window& operator= (Window&&) = default;

		Window(Window const&) = delete;
		Window& operator= (Window const&) = delete;

		// Initialization and management
		void Initialize(HWND window, int width, int height);

		// Basic game loop
		void Tick();

		// Messages
		void GetMessage(UINT message, WPARAM wParam, LPARAM lParam);
		void OnActivated();
		void OnDeactivated();
		void OnSuspending();
		void OnResuming();
		void OnWindowSizeChanged(int width, int height);

		// Properties
		void GetDefaultSize(int& width, int& height) const noexcept;

	private:

		void Update(DX::StepTimer const& timer);
		void Render();

		void Clear();
		void Present();

		void CreateDevice();
		void CreateResources();

		void OnDeviceLost();

		// Device resources.
		HWND                                            m_window;
		int                                             m_outputWidth;
		int                                             m_outputHeight;

		D3D_FEATURE_LEVEL                               m_featureLevel;
		Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

		Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

		// Rendering loop timer.
		DX::StepTimer                                   m_timer;

		bool s_in_sizemove = false;
		bool s_in_suspend = false;
		bool s_minimized = false;

		Graph graph;
	};
}