/*

これは任意のウィンドウをキャプチャするためのプログラムです。

*/

#pragma once

#include "pch.h"

namespace gd
{
	/**
	 * 任意のウィンドウをキャプチャするクラス
	 */
	class Capture
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

		// ウィンドウをキャプチャするWin32の非公開関数へのポインタ
		using DwmGetDxSharedSurface = BOOL(WINAPI*)(HWND, HANDLE*, UINT64*, DXGI_FORMAT*, DWORD*, UINT64*);

	public:
		/**
		 * キャプチャを開始します
		 * @param d3dContext Direct3Dのコンテキスト
		 * @param キャプチャするウィンドウのハンドル
		 */
		bool start(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext, HWND target)
		{
			// ID3D11Deviceを取得する
			Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
			d3dContext->GetDevice(&m_d3dDevice);

			// ウィンドウの表面への共有ハンドル
			HANDLE phSurface;

			// グラフィックスアダプタ取得
			Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice;
			Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
			DXGI_ADAPTER_DESC desc{};
			UINT64 adapterLuid;
			DX::ThrowIfFailed(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice1), &dxgiDevice));
			DX::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));
			DX::ThrowIfFailed(dxgiAdapter->GetDesc(&desc));
			adapterLuid = desc.AdapterLuid.HighPart; adapterLuid <<= 32;
			adapterLuid |= desc.AdapterLuid.LowPart;


			/*
			メモ
			dllの読み込みには通常LoadLibrary()でハンドルを取得し、使い終わったらFreeLibrary()でハンドルを解放する。
			しかし、user32.dllはウィンドウの生成に必要なので、プログラムの起動時には既にuser32.dllは読み込まれているはずである。
			そのため、ここではuser32.dllのハンドルの取得にはGetModuleHandleWを使用した。
			また、このハンドルは自分でLoadLibrary()して生成したハンドルではないため、FreeLibrary()する必要はない。
			*/

			// DwmGetDxSharedSurface関数の取得
			static HMODULE user32dll = GetModuleHandleW(L"user32.dll");
			assert(user32dll);
			static DwmGetDxSharedSurface getSurface = (DwmGetDxSharedSurface)GetProcAddress(user32dll, "DwmGetDxSharedSurface");
			if (NULL == getSurface)
			{
				Windows::error("DwmGetDxSharedSurface関数の取得に失敗しました。");
				return 1;
			}

			// DwmGetDxSharedSurface関数の実行
			DXGI_FORMAT pFmtWindow;;
			ULONG pPresentFlags;
			ULONGLONG pWin32kUpdateId;
			phSurface = nullptr;
			pPresentFlags = 0;
			pWin32kUpdateId = 0;
			if (getSurface(target, &phSurface, &adapterLuid, &pFmtWindow, &pPresentFlags, &pWin32kUpdateId) == 0)
			{
				Windows::error("DwmGetDxSharedSurface関数の呼び出しに失敗しました。");
				return 1;
			}

			// 共有リソースにアクセスできるようにする
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			DX::ThrowIfFailed(m_d3dDevice->OpenSharedResource(phSurface, IID_PPV_ARGS(&resource)));

			// 取得したリソースをテクスチャに変換
			Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTarget;
			DX::ThrowIfFailed(resource->QueryInterface(IID_PPV_ARGS(&renderTarget)));

			// シェーダリソースビューの生成
			DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(renderTarget.Get(), NULL, &shaderResourceView));

			// 処理終了
			return 0;
		}

		/**
		 * キャプチャを取得します
		 */
		inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& getImage()
		{
			return shaderResourceView;
		}
	};
}