#pragma once

#include "pch.h"

namespace gd
{
	class Capture
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewBackup;

		// ウィンドウをキャプチャする関数
		using DwmGetDxSharedSurface = BOOL(WINAPI*)(HWND, HANDLE*, UINT64*, DXGI_FORMAT*, DWORD*, UINT64*);

	public:
		bool start(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext, HWND target)
		{
			// ID3D11DeviceContextのポインタを持っておく
			// (ComPtrの代入演算子は参照カウントをインクリメントして、ポインタの所有権を共有する)
			m_d3dContext = d3dContext;

			// ID3D11Deviceを取得する
			Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
			m_d3dContext->GetDevice(&m_d3dDevice);

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

			// テクスチャのバックアップ生成
			D3D11_TEXTURE2D_DESC renderTargetDesk;
			renderTarget->GetDesc(&renderTargetDesk);
			Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBackup;
			m_d3dDevice->CreateTexture2D(&renderTargetDesk, NULL, &renderTargetBackup);
			m_d3dContext->CopyResource(renderTargetBackup.Get(), renderTarget.Get());

			// シェーダリソースビューの生成
			DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(renderTarget.Get(), NULL, &shaderResourceView));

			// バックアップシェーダリソースビューの生成
			DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(renderTargetBackup.Get(), NULL, &shaderResourceViewBackup));

			// 処理終了
			return 0;
		}

		inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& getImage()
		{
			return shaderResourceView;
		}

		inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& getBackupImage()
		{
			return shaderResourceViewBackup;
		}
	};
}