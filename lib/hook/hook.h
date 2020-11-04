#pragma once

#include <Windows.h>

#ifdef EXPORT_
#define EXPORT_API_ __declspec(dllexport)
#else
#define EXPORT_API_ __declspec(dllimport)
#endif 

/**
 * マウスとキーボードのウィンドウメッセージをフックし、別のウィンドウに転送するようにします。
 * この関数を実行した場合は必ず最後に removeHook を実行してください。
 * @param targetHwnd フックするウィンドウハンドル
 * @param forwardHwnd 転送先のウィンドウハンドル
 * @return 成功すると0、失敗すると1が返される
 */
EXPORT_API_ bool insertHook(HWND targetHwnd, HWND forwardHwnd);

/**
 * insertHook で設定したフックを解除します。
 */
EXPORT_API_ void removeHook();

// フックにより呼び出されるキーイベントを処理するコールバック関数
EXPORT_API_ LRESULT CALLBACK KeyHookProc(int, WPARAM, LPARAM);

// フックにより呼び出されるマウスイベントを処理するコールバック関数
EXPORT_API_ LRESULT CALLBACK MouseHookProc(int, WPARAM, LPARAM);