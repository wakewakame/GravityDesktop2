#pragma once

#include <string>
#include <map>
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
	 */
	class Windows
	{
	public:
		/**
		 * コンストラクタ
		 * @param hInstance 通常はwWinMainの引数に渡される値を使用する
		 * @param nCmdShow 通常はwWinMainの引数に渡される値を使用する
		 */
		Windows(HINSTANCE hInstance, int nCmdShow);

		virtual ~Windows();

		/**
		 * ウィンドウを生成します。
		 * @param windowTitle ウィンドウのタイトルバーに表示される文字列を指定する
		 * @param windowStyle WinUser.hで定義されているWS_から始まる定数を指定する
		 * @param enableDoubleClick falseを指定するとダブルクリックが単なる2回分のクリックとして扱われる
		 * @return 成功すると0が返されます。失敗すると1以上の値が返される
		 */
		int create(
			const std::string& windowTitle = "window",
			DWORD windowStyle = WS_OVERLAPPEDWINDOW, bool enableDoubleClick = true
		);

		/**
		 * 描画するためにループ処理を行います。
		 * ウィンドウが閉じられるまで、この関数が返ることはありません。
		 */
		int waitUntilExit();

		/**
		 * 全てのウィンドウを終了させます。
		 */
		void exit();

		/**
		 * エラー用のダイアログを表示します。
		 * @param description エラーの説明文
		 */
		static void error(const std::string& description);

	private:
		HINSTANCE hInstance;

		int nCmdShow;

		static size_t createCount;

		using Games = std::map<HWND, std::unique_ptr<Game>>;
		Games games;

		static void error(LPCWSTR description);

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}