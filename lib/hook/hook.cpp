#include "hook.h"

#pragma data_seg(".shareddata")

// フックするウィンドウハンドル
HWND targetHwnd_ = NULL;

// 転送先のウィンドウハンドル
HWND forwardHwnd_ = NULL;

// キーフックとマウスフック
HHOOK hKeyHook = NULL;
HHOOK hMouseHook = NULL;

#pragma data_seg()

// DLLのインスタンス
HINSTANCE hinstDLL_;

bool insertHook(HWND targetHwnd, HWND forwardHwnd)
{
	removeHook();

	// ウィンドウハンドルを記憶しておく
	targetHwnd_ = targetHwnd;
	forwardHwnd_ = forwardHwnd;

	// ターゲットのウィンドウハンドルからスレッドID取得
	DWORD threadId = GetWindowThreadProcessId(targetHwnd_, NULL);

	// ターゲットのウィンドウハンドルにキーフックとマウスフックを追加
	hKeyHook = SetWindowsHookEx(WH_KEYBOARD, KeyHookProc, hinstDLL_, threadId);
	hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseHookProc, hinstDLL_, threadId);

	// エラーチェック
	if ((NULL == hKeyHook) || (NULL == hMouseHook))
	{
		removeHook();
		return 1;
	}

	return 0;
}

void removeHook()
{
	if (NULL != hKeyHook)
	{
		UnhookWindowsHookEx(hKeyHook);
		hKeyHook = NULL;
	}

	if (NULL != hMouseHook)
	{
		UnhookWindowsHookEx(hMouseHook);
		hMouseHook = NULL;
	}

	targetHwnd_ = NULL;
	forwardHwnd_ = NULL;
}

LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wp, LPARAM lp)
{
	// 決まり事
	if (nCode < 0) { return CallNextHookEx(hKeyHook, nCode, wp, lp); }

	if ((nCode == HC_ACTION) && (forwardHwnd_ != NULL))
	{
		// forwardHwnd_ にキーボードメッセージと、キーコードの転送
		PostMessage(
			forwardHwnd_,
			(lp & 0b10000000000000000000000000000000) ? WM_KEYUP : WM_KEYDOWN,
			wp, 
			lp
		);

		// このウィンドウメッセージをここで止める
		return 0;
	}

	return CallNextHookEx(hKeyHook, nCode, wp, lp);
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wp, LPARAM lp)
{
	// 決まり事
	if (nCode < 0) { return CallNextHookEx(hMouseHook, nCode, wp, lp); }

	if ((nCode == HC_ACTION) && (forwardHwnd_ != NULL))
	{
		// forwardHwnd_ にキーボードメッセージと、キーコードの転送
		MOUSEHOOKSTRUCT *msg = (MOUSEHOOKSTRUCT*)lp;
		ScreenToClient(targetHwnd_, &msg->pt);
		PostMessage(
			forwardHwnd_,
			(UINT)wp,
			(WPARAM)msg->wHitTestCode,
			MAKELPARAM(msg->pt.x, msg->pt.y)
		);

		// このウィンドウメッセージをここで止める
		return 0;
	}

	return CallNextHookEx(hMouseHook, nCode, wp, lp);
}

// エントリーポイント
BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	if (DLL_PROCESS_ATTACH == fdwReason)
	{
		hinstDLL_ = hinstDLL;
	}

	return TRUE;
}