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

	/**
	 * ウィンドウを生成するための関数をまとめたクラスです。
	 */
	class Windows
	{
	public:
		/**
		 * コンストラクタ
		 * @param hInstance 通常はwWinMainの引数に渡される値を使用する
		 * @param nCmdShow 通常はwWinMainの引数に渡される値を使用する
		 */
		Windows(HINSTANCE hInstance, int nCmdShow);

		virtual ~Windows();

		/**
		 * ウィンドウを生成します。
		 * @param windowTitle ウィンドウのタイトルバーに表示される文字列を指定する
		 * @param windowStyle WinUser.hで定義されているWS_から始まる定数を指定する
		 * @param enableDoubleClick falseを指定するとダブルクリックが単なる2回分のクリックとして扱われる
		 * @return 成功すると0が返されます。失敗すると1以上の値が返される
		 */
		int create(
			const std::string& windowTitle = "window",
			DWORD windowStyle = WS_OVERLAPPEDWINDOW, bool enableDoubleClick = true
		);

		/**
		 * 描画するためにループ処理を行います。
		 * ウィンドウが閉じられるまで、この関数が返ることはありません。
		 */
		int waitUntilExit();

		/**
		 * 全てのウィンドウを終了させます。
		 */
		void exit();

		/**
		 * エラー用のダイアログを表示します。
		 * @param description エラーの説明文
		 */
		static void error(const std::string& description);

	private:
		HINSTANCE hInstance;

		int nCmdShow;

		static size_t createCount;

		std::map<HWND, std::unique_ptr<Window>> windows;

		static void error(LPCWSTR description);

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}