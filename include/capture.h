/*

����͔C�ӂ̃E�B���h�E���L���v�`�����邽�߂̃v���O�����ł��B

*/

#pragma once

#include "pch.h"

namespace gd
{
	/**
	 * �C�ӂ̃E�B���h�E���L���v�`������N���X
	 */
	class Capture
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

		// �E�B���h�E���L���v�`������Win32�̔���J�֐��ւ̃|�C���^
		using DwmGetDxSharedSurface = BOOL(WINAPI*)(HWND, HANDLE*, UINT64*, DXGI_FORMAT*, DWORD*, UINT64*);

	public:
		/**
		 * �L���v�`�����J�n���܂�
		 * @param d3dContext Direct3D�̃R���e�L�X�g
		 * @param �L���v�`������E�B���h�E�̃n���h��
		 */
		bool start(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext, HWND target)
		{
			// ID3D11Device���擾����
			Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
			d3dContext->GetDevice(&m_d3dDevice);

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


			/*
			����
			dll�̓ǂݍ��݂ɂ͒ʏ�LoadLibrary()�Ńn���h�����擾���A�g���I�������FreeLibrary()�Ńn���h�����������B
			�������Auser32.dll�̓E�B���h�E�̐����ɕK�v�Ȃ̂ŁA�v���O�����̋N�����ɂ͊���user32.dll�͓ǂݍ��܂�Ă���͂��ł���B
			���̂��߁A�����ł�user32.dll�̃n���h���̎擾�ɂ�GetModuleHandleW���g�p�����B
			�܂��A���̃n���h���͎�����LoadLibrary()���Đ��������n���h���ł͂Ȃ����߁AFreeLibrary()����K�v�͂Ȃ��B
			*/

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

			// �V�F�[�_���\�[�X�r���[�̐���
			DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(renderTarget.Get(), NULL, &shaderResourceView));

			// �����I��
			return 0;
		}

		/**
		 * �L���v�`�����擾���܂�
		 */
		inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& getImage()
		{
			return shaderResourceView;
		}
	};
}