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

		// �E�B���h�E���L���v�`������֐�
		using DwmGetDxSharedSurface = BOOL(WINAPI*)(HWND, HANDLE*, UINT64*, DXGI_FORMAT*, DWORD*, UINT64*);

	public:
		bool init(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext, HWND target)
		{
			// ID3D11DeviceContext�̃|�C���^�������Ă���
			// (ComPtr�̑�����Z�q�͎Q�ƃJ�E���g���C���N�������g���āA�|�C���^�̏��L�������L����)
			m_d3dContext = d3dContext;

			// ID3D11Device���擾����
			Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
			m_d3dContext->GetDevice(&m_d3dDevice);

			// �E�B���h�E�̕\�ʂւ̋��L�n���h��
			HANDLE phSurface;

			// �E�B���h�E�T�C�Y�擾
			RECT rc;
			GetWindowRect(target, &rc);
			size.cx = rc.right - rc.left;
			size.cy = rc.bottom - rc.top;

			// �O���t�B�b�N�X�A�_�v�^�擾
			ComPtr<IDXGIDevice1> dxgiDevice;
			ComPtr<IDXGIAdapter> dxgiAdapter;
			DXGI_ADAPTER_DESC desc{};
			UINT64 adapterLuid;
			DX::ThrowIfFailed(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice1), &dxgiDevice));
			DX::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));
			DX::ThrowIfFailed(dxgiAdapter->GetDesc(&desc));
			adapterLuid = desc.AdapterLuid.HighPart; adapterLuid <<= 32;
			adapterLuid |= desc.AdapterLuid.LowPart;

			// DwmGetDxSharedSurface�֐��̎擾
			HMODULE user32dll;
			DwmGetDxSharedSurface getSurface;
			user32dll = LoadLibrary(TEXT("user32.dll"));
			if (user32dll == NULL)
			{
				Windows::error("user32.dll�̓ǂݍ��݂Ɏ��s���܂����B");
				return 1;
			}
			getSurface = (DwmGetDxSharedSurface)GetProcAddress(user32dll, "DwmGetDxSharedSurface");
			if (getSurface == NULL)
			{
				Windows::error("DwmGetDxSharedSurface�֐��̎擾�Ɏ��s���܂����B");
				FreeLibrary(user32dll);
				return 1;
			}

			// DwmGetDxSharedSurface�֐��̎��s
			DXGI_FORMAT pFmtWindow;;
			ULONG pPresentFlags;
			ULONGLONG pWin32kUpdateId;
			phSurface = nullptr;
			pPresentFlags = 0;
			pWin32kUpdateId = 0;
			if (getSurface(target, &phSurface, &adapterLuid, &pFmtWindow, &pPresentFlags, &pWin32kUpdateId) == 0)
			{
				Windows::error("DwmGetDxSharedSurface�֐��̌Ăяo���Ɏ��s���܂����B");
				FreeLibrary(user32dll);
				return 1;
			}

			// dll�J��
			FreeLibrary(user32dll);

			// ���L���\�[�X�ɃA�N�Z�X�ł���悤�ɂ���
			DX::ThrowIfFailed(m_d3dDevice->OpenSharedResource(phSurface, IID_ID3D11Resource, (void**)(resource.ReleaseAndGetAddressOf())));

			// �擾�������\�[�X���e�N�X�`���ɕϊ�
			DX::ThrowIfFailed(resource->QueryInterface(IID_ID3D11Texture2D, (void**)(renderTarget.GetAddressOf())));

			// �e�N�X�`���̃o�b�N�A�b�v����
			D3D11_TEXTURE2D_DESC renderTargetDesk;
			renderTarget->GetDesc(&renderTargetDesk);
			m_d3dDevice->CreateTexture2D(&renderTargetDesk, NULL, renderTargetBackup.ReleaseAndGetAddressOf());
			m_d3dContext->CopyResource(*renderTargetBackup.GetAddressOf(), *renderTarget.GetAddressOf());

			// �V�F�[�_���\�[�X�r���[�̐���
			DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(*renderTarget.GetAddressOf(), NULL, shaderResourceView.ReleaseAndGetAddressOf()));

			// �o�b�N�A�b�v�V�F�[�_���\�[�X�r���[�̐���
			DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(*renderTargetBackup.GetAddressOf(), NULL, shaderResourceViewBackup.ReleaseAndGetAddressOf()));

			// �X�v���C�g�̐���
			spriteBatch = std::make_unique<DirectX::SpriteBatch>(*m_d3dContext.GetAddressOf());

			// �����I��
			return 0;
		}
	};
}
*/