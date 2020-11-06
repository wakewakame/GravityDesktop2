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
	 * �E�B���h�E�𐶐����邽�߂̊֐����܂Ƃ߂��N���X�ł��B
	 */
	class Windows
	{
	public:
		/**
		 * �R���X�g���N�^
		 * @param hInstance �ʏ��wWinMain�̈����ɓn�����l���g�p����
		 * @param nCmdShow �ʏ��wWinMain�̈����ɓn�����l���g�p����
		 */
		Windows(HINSTANCE hInstance, int nCmdShow);

		virtual ~Windows();

		/**
		 * �E�B���h�E�𐶐����܂��B
		 * @param windowTitle �E�B���h�E�̃^�C�g���o�[�ɕ\������镶������w�肷��
		 * @param windowStyle WinUser.h�Œ�`����Ă���WS_����n�܂�萔���w�肷��
		 * @param enableDoubleClick false���w�肷��ƃ_�u���N���b�N���P�Ȃ�2�񕪂̃N���b�N�Ƃ��Ĉ�����
		 * @return ���������0���Ԃ���܂��B���s�����1�ȏ�̒l���Ԃ����
		 */
		int create(
			const std::string& windowTitle = "window",
			DWORD windowStyle = WS_OVERLAPPEDWINDOW, bool enableDoubleClick = true
		);

		/**
		 * �`�悷�邽�߂Ƀ��[�v�������s���܂��B
		 * �E�B���h�E��������܂ŁA���̊֐����Ԃ邱�Ƃ͂���܂���B
		 */
		int waitUntilExit();

		/**
		 * �S�ẴE�B���h�E���I�������܂��B
		 */
		void exit();

		/**
		 * �G���[�p�̃_�C�A���O��\�����܂��B
		 * @param description �G���[�̐�����
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