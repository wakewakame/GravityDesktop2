/*

これはキーイベントを抽象化するためのプログラムです。

*/

#pragma once

#include "utils/pch.h"


namespace gd
{
	struct Keyboard
	{
		std::set<uint8_t> keys;   // 現在のフレームのキー入力
		std::set<uint8_t> keys_;  // 1フレーム前のフレームのキー入力
	};

	class KeyboardProcess
	{
	public:
		KeyboardProcess();
		virtual ~KeyboardProcess();
		void OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		/**
		 * 現在のフレームでのキーボードの状態を更新する
		 */
		void nextFrame();

		/**
		 * 現在のフレームにおけるキーボードの状態を取得する
		 * @return 現在のフレームにおけるキーボードの状態
		 */
		Keyboard getKeyboardStatus() const;

	private:
		std::set<uint8_t> aKeys;  // 最新のキー入力
		std::set<uint8_t> bKeys;  // 現在のフレームのキー入力
		std::set<uint8_t> cKeys;  // 1フレーム前のフレームのキー入力
	};
}