#pragma once

#include "pch.h"
#include "dx_window.h"
#include "component.h"

namespace gd
{
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
		 * @param T RootComponent�̔h���N���X
		 * @param args... T�̃R���X�g���N�^�����ɓn���l
		 * @return ���������0���Ԃ���A���s�����1�ȏ�̒l���Ԃ����
		 */
		template<class T, class... Args>
		int create(Args&&... args)
		{
			static_assert(
				std::is_base_of<RootComponent, T>::value,
				"�e���v���[�g�����Ɏw�肳�ꂽ�^��RootComponent�N���X���p�����Ă��܂���"
			);

			// RootComponent�̃C���X�^���X�𐶐�����
			std::unique_ptr<RootComponent> rootComponent{
				static_cast<RootComponent*>(new T(std::forward<Args>(args)...))
			};

			// �E�B���h�E���쐬����
			return create(std::move(rootComponent));
		}

		/**
		 * �`�悷�邽�߂Ƀ��[�v�������s���܂��B
		 * �E�B���h�E��������܂ŁA���̊֐����Ԃ邱�Ƃ͂���܂���B
		 * @return ���������0���Ԃ���A���s�����1�ȏ�̒l���Ԃ����
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
		std::map<HWND, std::unique_ptr<Window>> windows;
		static size_t createCount;

		int create(std::unique_ptr<RootComponent>&& rootComponent);
		static void error(LPCWSTR description);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT SubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}