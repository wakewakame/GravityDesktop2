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

		// �E�B���h�E���L���v�`������֐�
		using DwmGetDxSharedSurface = BOOL(WINAPI*)(HWND, HANDLE*, UINT64*, DXGI_FORMAT*, DWORD*, UINT64*);

	public:
		bool start(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext, HWND target)
		{
			// ID3D11DeviceContext�̃|�C���^�������Ă���
			// (ComPtr�̑�����Z�q�͎Q�ƃJ�E���g���C���N�������g���āA�|�C���^�̏��L�������L����)
			m_d3dContext = d3dContext;

			// ID3D11Device���擾����
			Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
			m_d3dContext->GetDevice(&m_d3dDevice);

			// �E�B���h�E�̕\�ʂւ̋��L�n���h��
			HANDLE phSurface;

			// �O���t�B�b�N�X�A�_�v�^�擾
			Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice;
			Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
			DXGI_ADAPTER_DESC desc{};
			UINT64 adapterLuid;
			DX::ThrowIfFailed(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice1), &dxgiDevice));
			DX::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));
			DX::ThrowIfFailed(dxgiAdapter->GetDesc(&desc));
			adapterLuid = desc.AdapterLuid.HighPart; adapterLuid <<= 32;
			adapterLuid |= desc.AdapterLuid.LowPart;

			// DwmGetDxSharedSurface�֐��̎擾
			static HMODULE user32dll = GetModuleHandleW(L"user32.dll");
			assert(user32dll);
			static DwmGetDxSharedSurface getSurface = (DwmGetDxSharedSurface)GetProcAddress(user32dll, "DwmGetDxSharedSurface");
			if (NULL == getSurface)
			{
				Windows::error("DwmGetDxSharedSurface�֐��̎擾�Ɏ��s���܂����B");
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
				return 1;
			}

			// ���L���\�[�X�ɃA�N�Z�X�ł���悤�ɂ���
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			DX::ThrowIfFailed(m_d3dDevice->OpenSharedResource(phSurface, IID_PPV_ARGS(&resource)));

			// �擾�������\�[�X���e�N�X�`���ɕϊ�
			Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTarget;
			DX::ThrowIfFailed(resource->QueryInterface(IID_PPV_ARGS(&renderTarget)));

			// �e�N�X�`���̃o�b�N�A�b�v����
			D3D11_TEXTURE2D_DESC renderTargetDesk;
			renderTarget->GetDesc(&renderTargetDesk);
			Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBackup;
			m_d3dDevice->CreateTexture2D(&renderTargetDesk, NULL, &renderTargetBackup);
			m_d3dContext->CopyResource(renderTargetBackup.Get(), renderTarget.Get());

			// �V�F�[�_���\�[�X�r���[�̐���
			DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(renderTarget.Get(), NULL, &shaderResourceView));

			// �o�b�N�A�b�v�V�F�[�_���\�[�X�r���[�̐���
			DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(renderTargetBackup.Get(), NULL, &shaderResourceViewBackup));

			// �����I��
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