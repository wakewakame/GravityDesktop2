/*

これはウィンドウを生成するためのプログラムです。

*/

#pragma once

#include "utils/pch.h"
#include "components/dx_window.h"
#include "components/root_component.h"

namespace gd
{
	class RootComponent;

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
		 * @param T RootComponentの派生クラス
		 * @param args... Tのコンストラクタ引数に渡す値
		 * @return 成功すると0が返され、失敗すると1以上の値が返される
		 */
		template<class T, class... Args>
		int create(Args&&... args)
		{
			static_assert(
				std::is_base_of<RootComponent, T>::value,
				"テンプレート引数に指定された型がRootComponentクラスを継承していません"
			);

			// RootComponentのインスタンスを生成する
			std::unique_ptr<RootComponent> rootComponent{
				static_cast<RootComponent*>(new T(std::forward<Args>(args)...))
			};

			// ウィンドウを作成する
			return create(std::move(rootComponent));
		}

		/**
		 * 描画するためにループ処理を行います。
		 * ウィンドウが閉じられるまで、この関数が返ることはありません。
		 * @return 成功すると0が返され、失敗すると1以上の値が返される
		 */
		int waitUntilExit();

		/**
		 * エラー用のダイアログを表示します。
		 * @param description エラーの説明文
		 */
		static void error(LPCWSTR description);

	private:
		HINSTANCE hInstance;
		int nCmdShow;
		std::map<HWND, std::unique_ptr<Window>> windows;
		static size_t createCount;

		int create(std::unique_ptr<RootComponent>&& rootComponent);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT SubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}