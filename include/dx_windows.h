#pragma once

#include "pch.h"
#include "dx_window.h"

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