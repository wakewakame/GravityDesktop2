/*

これはデスクトップになりすますウィンドウを生成するコンポーネントです。

*/

#pragma once

#include "utils/types.h"
#include "components/root_component.h"
#include "desktop/capture.h"
#include "hook.h"

namespace gd
{
	class FakeDesktopComponent : public RootComponent
	{
	public:
		struct DesktopHwnds
		{
			HWND listview = NULL;        // デスクトップのアイコンを表示するウィンドウハンドル
			HWND wallpaper = NULL;       // デスクトップの壁紙を表示するウィンドウハンドル
			std::vector<HWND> taskbars;  // 各モニターのタスクバーのウィンドウハンドル
		};

		FakeDesktopComponent() {}
		virtual ~FakeDesktopComponent() {
			/*
			メモ
			以下の処理は終了時に絶対に呼ばれてほしいので、Component::exit関数に移動させてはいけない。
			*/

			// デスクトップの祖先ウィンドウを不透明にする
			if (desktopHwnds.isErr || desktopHwnds.isNone) { return; }
			show(GetAncestor(desktopHwnds.value.listview, GA_ROOTOWNER));

			// フックを外す
			removeHook();
		}

		DWORD getWindowStyle() const override final { return WS_POPUP | WS_CHILD; }

		void init(gd::Graph& graph)
		{
			RootComponent::init(graph);
			HWND hWnd = getHwnd();

			// デスクトップのウィンドウハンドルを取得する
			desktopHwnds = getDesktopHwnd();
			if (desktopHwnds.isErr || desktopHwnds.isNone)
			{
				Windows::error(desktopHwnds.description);
				closeWindow();
				return;
			}

			// 自身の親ウィンドウを壁紙のウィンドウに設定
			SetParent(hWnd, desktopHwnds.value.wallpaper);

			// 自身のウィンドウの位置とサイズを調整する
			RECT rc;
			GetWindowRect(desktopHwnds.value.wallpaper, &rc);
			SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, rc.right - rc.left, rc.bottom - rc.top, 0);
			setSize(rc.right - rc.left, rc.bottom - rc.top);

			// デスクトップアイコンを透明にする
			HWND ancestor = GetAncestor(desktopHwnds.value.listview, GA_ROOTOWNER);
			hide(ancestor);

			// 壁紙ウィンドウにキーイベントやマウスイベントが届くようにする
			EnableWindow(desktopHwnds.value.wallpaper, TRUE);

			// 壁紙ウィンドウへのマウスイベントとキーイベントのフックをする
			if (insertHook(desktopHwnds.value.wallpaper, hWnd))
			{
				Windows::error(L"デスクトップのフックに失敗しました。");
				closeWindow();
				return;
			}
		}
		void render(gd::Graph& graph, gd::Mouse& mouse) override
		{
			RootComponent::render(graph, mouse);
		}
		void exit(gd::Graph& graph) override
		{
			RootComponent::exit(graph);
		}
		void resize(int width, int height) override
		{
			RootComponent::resize(width, height);
		}

		// デスクトップのウィンドウハンドルを取得する
		static Result<DesktopHwnds> getDesktopHwnd()
		{
			DesktopHwnds result;

			/*
			メモ
			Windowsのデスクトップは「壁紙」と「アイコン」がそれぞれ別のウィンドウ
			に表示されるとき(状態Aとする)と、1つのウィンドウにまとめて表示される
			とき(状態Bとする)の2パターンある。今回の重力デスクトップでは、アイコン
			はアイコンでキャプチャし、壁紙は壁紙でキャプチャしたいので、状態Aの必
			要がある。以下は状態Bを状態Aに変更するための処理である。

			参考元: https://www.codeproject.com/articles/856020/draw-behind-desktop-icons-in-windows-plus
			*/

			HWND Progman = FindWindow(L"Progman", NULL);
			if (NULL == Progman) return Result<DesktopHwnds>::Err(L"Progmanのウィンドウハンドル取得に失敗しました。");
			HWND SHELLDLL_DefView = FindWindowEx(Progman, NULL, L"SHELLDLL_DefView", L"");
			// デスクトップの壁紙とアイコンが1つのウィンドウにまとまっている場合
			if (NULL != SHELLDLL_DefView)
			{
				// 壁紙とアイコンをそれぞれ別のウィンドウに分離させる
				if (0 == SendMessageTimeout(Progman, 0x052C, 0, 0, 0x0, 10000, NULL))
				{
					return Result<DesktopHwnds>::Err(L"壁紙とアイコンの分離に失敗しました。");
				}
			}

			// WorkerWのSysListView32ハンドル取得
			HWND WorkerW = NULL;
			SHELLDLL_DefView = NULL;
			while (true)
			{
				WorkerW = FindWindowEx(NULL, WorkerW, L"WorkerW", L"");
				if (NULL == WorkerW)
				{
					SHELLDLL_DefView = NULL;
					break;
				}
				SHELLDLL_DefView = FindWindowEx(WorkerW, NULL, L"SHELLDLL_DefView", L"");
				if (NULL != SHELLDLL_DefView)
				{
					break;
				}
			}
			if (NULL == SHELLDLL_DefView)
			{
				return Result<DesktopHwnds>::Err(L"デスクトップのウィンドウハンドルの取得に失敗しました。");
			}

			// デスクトップのアイコンを表示するListViewのハンドル取得
			result.listview = FindWindowEx(SHELLDLL_DefView, NULL, L"SysListView32", NULL);
			if (NULL == result.listview)
			{
				return Result<DesktopHwnds>::Err(L"デスクトップのListViewのハンドルの取得に失敗しました。");
			}

			// デスクトップの壁紙を表示するウィンドウハンドルの取得
			result.wallpaper = NULL;
			while (true)
			{
				result.wallpaper = FindWindowEx(NULL, result.wallpaper, L"WorkerW", L"");
				if (result.wallpaper == NULL) {
					break;
				}
				if (GetParent(result.wallpaper) == Progman) break;
			}
			if (NULL == result.wallpaper)
			{
				return Result<DesktopHwnds>::Err(L"壁紙のウィンドウハンドルの取得に失敗しました。");
			}

			// 全てのタスクバーのウィンドウハンドルを取得
			result.taskbars;
			HWND buf = NULL;
			// メインモニターのタスクバーを取得
			while (true)
			{
				buf = FindWindowEx(NULL, buf, L"Shell_TrayWnd", NULL);
				if (NULL == buf) break;
				result.taskbars.push_back(buf);
			}
			// サブモニターのタスクバーを取得
			while (true)
			{
				buf = FindWindowEx(NULL, buf, L"Shell_SecondaryTrayWnd", NULL);
				if (NULL == buf) break;
				result.taskbars.push_back(buf);
			}

			return Result<DesktopHwnds>::Ok(result);
		}

	protected:
		Result<DesktopHwnds> desktopHwnds;

		/**
		 * ウィンドウを透明にする
		 * @param 透明にするウィンドウのハンドル
		 * @return 成功するとtrue、失敗するとfalseが返る
		 */
		bool hide(HWND hwnd) {
			LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
			if (0 == style) return false;
			SetLastError(0);
			SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
			if (0 != GetLastError()) return false;
			if (0 == SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA)) return false;
			return true;
		}

		/**
		 * ウィンドウを不透明にする
		 * @param 不透明にするウィンドウのハンドル
		 * @return 成功するとtrue、失敗するとfalseが返る
		 */
		bool show(HWND hwnd) {
			LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
			if (0 == style) return false;
			SetLastError(0);
			SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
			if (0 != GetLastError()) return false;
			if (0 == SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA)) return false;
			return true;
		}
	};
}
