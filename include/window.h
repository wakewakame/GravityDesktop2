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
	 * ウィンドウを生成するための関数をまとめたクラスです。
	 * このクラスではウィンドウを2つ以上同時に生成できません。
	 */
	class Window
	{
	private:
		static void error(LPCWSTR description) { MessageBoxW(nullptr, description, L"error", MB_OK | MB_ICONERROR); }

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		static std::unique_ptr<Game> g_game;

	public:
		/**
		 * ウィンドウを生成します。
		 * ウィンドウが閉じられた後は必ずdestroy()を実行してください。
		 * @param hInstance 通常はwWinMainの引数に渡される値を使用する
		 * @param nCmdShow 通常はwWinMainの引数に渡される値を使用する
		 * @param width, height ウィンドウのサイズを指定する
		 * @param windowTitle ウィンドウのタイトルバーに表示される文字列を指定する
		 * @param windowStyle WinUser.hで定義されているWS_から始まる定数を指定する
		 * @param enableDoubleClick falseを指定するとダブルクリックが単なる2回分のクリックとして扱われる
		 * @return 成功すると0が返されます。失敗すると1以上の値が返されます。
		 */
		static int create(
			HINSTANCE hInstance, int nCmdShow,
			const int32_t width = 640, const int32_t height = 480, const std::string& windowTitle = "window",
			DWORD windowStyle = WS_POPUPWINDOW, bool enableDoubleClick = true
		);

		/**
		 * 描画するためにループ処理を行います。
		 * ウィンドウが閉じられるまで、この関数が返ることはありません。
		 */
		static int waitUntilExit();

		/**
		 * ウィンドウを終了させます。
		 */
		static void exit();

		/**
		 * ウィンドウの生成に用いたメモリなどを破棄します。
		 */
		static void destroy();
	};
}