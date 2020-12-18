/*

これは生成されたウィンドウが行うべき処理をまとめたプログラムです。

*/

#pragma once

#include "pch.h"
#include "StepTimer.h"
#include "root_component.h"
#include "graph.h"
#include "mouse.h"

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
		void OnActivated();
		void OnDeactivated();
		void OnSuspending();
		void OnResuming();
		void OnWindowSizeChanged(int width, int height);

		// この関数にウィンドウメッセージが送られてくる
		void OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam);

		// RootComponentを登録する
		void SetRootComponent(std::unique_ptr<RootComponent>&& root_component);

	private:

		void Update(DX::StepTimer const& timer);
		void Render();

		void Clear();
		void Present();

		void CreateDevice();
		void CreateResources();

		void OnDeviceLost();

		void Exit();

		// Device resources.
		HWND m_window;
		int  m_outputWidth;
		int  m_outputHeight;

		/*
		メモ
		ComPtrは参照カウンタを持つスマートポインタ
		コピーコンストラクタと代入演算子はともに参照カウンタを+1してポインタの所有権を共有する
		*/

		Microsoft::WRL::ComPtr<ID3D11Device>        m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;

		Microsoft::WRL::ComPtr<IDXGISwapChain1>        m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

		// Rendering loop timer.
		DX::StepTimer m_timer;

		bool s_in_sizemove = false;
		bool s_in_suspend  = false;
		bool s_minimized   = false;

		std::unique_ptr<RootComponent> root_component;
		Graph graph;
		MouseProcess mouseProc;
	};
}