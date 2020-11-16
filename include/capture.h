/*
#pragma once

#include "pch.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Details;

namespace gd
{
	class Capture
	{
	protected:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTarget;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBackup;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewBackup;
		std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
		SIZE size{};

		// ウィンドウをキャプチャする関数
		using DwmGetDxSharedSurface = BOOL(WINAPI*)(HWND, HANDLE*, UINT64*, DXGI_FORMAT*, DWORD*, UINT64*);

	public:
		bool init(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext, HWND target)
		{
			// ID3D11DeviceContextのポインタを持っておく
			// (ComPtrの代入演算子は参照カウントをインクリメントして、ポインタの所有権を共有する)
			m_d3dContext = d3dContext;

			// ID3D11Deviceを取得する
			Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
			m_d3dContext->GetDevice(&m_d3dDevice);

			// ウィンドウの表面への共有ハンドル
			HANDLE phSurface;

			// ウィンドウサイズ取得
			RECT rc;
			GetWindowRect(target, &rc);
			size.cx = rc.right - rc.left;
			size.cy = rc.bottom - rc.top;

			// グラフィックスアダプタ取得
			ComPtr<IDXGIDevice1> dxgiDevice;
			ComPtr<IDXGIAdapter> dxgiAdapter;
			DXGI_ADAPTER_DESC desc{};
			UINT64 adapterLuid;
			DX::ThrowIfFailed(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice1), &dxgiDevice));
			DX::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));
			DX::ThrowIfFailed(dxgiAdapter->GetDesc(&desc));
			adapterLuid = desc.AdapterLuid.HighPart; adapterLuid <<= 32;
			adapterLuid |= desc.AdapterLuid.LowPart;

			// DwmGetDxSharedSurface関数の取得
			HMODULE user32dll;
			DwmGetDxSharedSurface getSurface;
			user32dll = LoadLibrary(TEXT("user32.dll"));
			if (user32dll == NULL)
			{
				Windows::error("user32.dllの読み込みに失敗しました。");
				return 1;
			}
			getSurface = (DwmGetDxSharedSurface)GetProcAddress(user32dll, "DwmGetDxSharedSurface");
			if (getSurface == NULL)
			{
				Windows::error("DwmGetDxSharedSurface関数の取得に失敗しました。");
				FreeLibrary(user32dll);
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
				FreeLibrary(user32dll);
				return 1;
			}

			// dll開放
			FreeLibrary(user32dll);

			// 共有リソースにアクセスできるようにする
			DX::ThrowIfFailed(m_d3dDevice->OpenSharedResource(phSurface, IID_ID3D11Resource, (void**)(resource.ReleaseAndGetAddressOf())));

			// 取得したリソースをテクスチャに変換
			DX::ThrowIfFailed(resource->QueryInterface(IID_ID3D11Texture2D, (void**)(renderTarget.GetAddressOf())));

			// テクスチャのバックアップ生成
			D3D11_TEXTURE2D_DESC renderTargetDesk;
			renderTarget->GetDesc(&renderTargetDesk);
			m_d3dDevice->CreateTexture2D(&renderTargetDesk, NULL, renderTargetBackup.ReleaseAndGetAddressOf());
			m_d3dContext->CopyResource(*renderTargetBackup.GetAddressOf(), *renderTarget.GetAddressOf());

			// シェーダリソースビューの生成
			DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(*renderTarget.GetAddressOf(), NULL, shaderResourceView.ReleaseAndGetAddressOf()));

			// バックアップシェーダリソースビューの生成
			DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(*renderTargetBackup.GetAddressOf(), NULL, shaderResourceViewBackup.ReleaseAndGetAddressOf()));

			// スプライトの生成
			spriteBatch = std::make_unique<DirectX::SpriteBatch>(*m_d3dContext.GetAddressOf());

			// 処理終了
			return 0;
		}
	};
}
*/