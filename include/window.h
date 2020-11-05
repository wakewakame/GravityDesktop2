#pragma once

#include <string>
#include <stdint.h>
#include <memory>

#include "pch.h"
#include "Game.h"

namespace gd
{
	/*
	class Component
	{
	public:
		Component();

		virtual ~Component();

		void init();

		void draw();

		void resizeEvent(int32_t width, int32_t height);

		void mouseEvent();

		void keyEvent();
	};
	*/

	/**
	 * �E�B���h�E�𐶐����邽�߂̊֐����܂Ƃ߂��N���X�ł��B
	 * ���̃N���X�ł̓E�B���h�E��2�ȏ㓯���ɐ����ł��܂���B
	 */
	class Window
	{
	private:
		static void error(LPCWSTR description) { MessageBoxW(nullptr, description, L"error", MB_OK | MB_ICONERROR); }

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		static std::unique_ptr<Game> g_game;

	public:
		/**
		 * �E�B���h�E�𐶐����܂��B
		 * �E�B���h�E������ꂽ��͕K��destroy()�����s���Ă��������B
		 * @param hInstance �ʏ��wWinMain�̈����ɓn�����l���g�p����
		 * @param nCmdShow �ʏ��wWinMain�̈����ɓn�����l���g�p����
		 * @param width, height �E�B���h�E�̃T�C�Y���w�肷��
		 * @param windowTitle �E�B���h�E�̃^�C�g���o�[�ɕ\������镶������w�肷��
		 * @param windowStyle WinUser.h�Œ�`����Ă���WS_����n�܂�萔���w�肷��
		 * @param enableDoubleClick false���w�肷��ƃ_�u���N���b�N���P�Ȃ�2�񕪂̃N���b�N�Ƃ��Ĉ�����
		 * @return ���������0���Ԃ���܂��B���s�����1�ȏ�̒l���Ԃ���܂��B
		 */
		static int create(
			HINSTANCE hInstance, int nCmdShow,
			const int32_t width = 640, const int32_t height = 480, const std::string& windowTitle = "window",
			DWORD windowStyle = WS_POPUPWINDOW, bool enableDoubleClick = true
		);

		/**
		 * �`�悷�邽�߂Ƀ��[�v�������s���܂��B
		 * �E�B���h�E��������܂ŁA���̊֐����Ԃ邱�Ƃ͂���܂���B
		 */
		static int waitUntilExit();

		/**
		 * �E�B���h�E���I�������܂��B
		 */
		static void exit();

		/**
		 * �E�B���h�E�̐����ɗp�����������Ȃǂ�j�����܂��B
		 */
		static void destroy();
	};
}