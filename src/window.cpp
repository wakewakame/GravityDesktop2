#include "window.h"
#include "pch.h"
#include "Game.h"

#include <codecvt>
#include <locale>

using namespace DirectX;

std::unique_ptr<Game> gd::Window::g_game;

extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int gd::Window::create(
    HINSTANCE hInstance, int nCmdShow,
    const int32_t width, const int32_t height, const std::string& windowTitle,
    DWORD windowStyle, bool enableDoubleClick
)
{
	// DirectXMathライブラリが使用可能かを確認する
	if (!XMVerifyCPUSupport()) { error(L"このPCではDirectXMathが利用できません。"); return 1; }

	// 現在のスレッドでCOMライブラリを初期化する
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr)) { error(L"COMライブラリの初期化に失敗しました。"); return 1; }

	// Gameインスタンスを生成する
	g_game = std::make_unique<Game>();

	// ウィンドウタイトルをstd::stringからLPCWSTRに変換する
	using convert_t = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_t, wchar_t> strconverter;
	std::wstring wWindowTitle_ = strconverter.from_bytes(windowTitle);
	std::wstring wWindowClass_ = strconverter.from_bytes(windowTitle + " class");
    LPCWSTR wWindowTitle = wWindowTitle_.c_str();
    LPCWSTR wWindowClass = wWindowClass_.c_str();

    // ウィンドウクラスの登録とウィンドウを生成する
    {
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);

        // CS_HREDRAW: 横サイズが変わったとき，ウインドウ全体を再描画する
        // CS_VREDRAW: 縦サイズが変わったとき，ウインドウ全体を再描画する
        // CS_DBLCLKS: ダブルクリックを有効化する
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

        // 送られてくるウィンドウメッセージを処理するためのコールバック関数(ウィンドウプロシージャ)を指定する
        wcex.lpfnWndProc = WndProc;

        // インスタンスへのハンドルを指定する
        wcex.hInstance = hInstance;

        // アプリケーションのアイコンを指定する
        wcex.hIcon = LoadIconW(nullptr, L"IDI_APPLICATION");
        wcex.hIconSm = LoadIconW(nullptr, L"IDI_APPLICATION");

        // ウィンドウのマウスカーソルを指定する
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);

        // 背景ブラシへのハンドルを指定する　(選択した値に+1する必要がある)
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

        // ウィンドウクラス名を指定する
        wcex.lpszClassName = wWindowClass;

        // ウィンドウクラスを登録する
        RegisterClassExW(&wcex);

        // 生成するウィンドウサイズを取得する
        int w, h;
        g_game->GetDefaultSize(w, h);
        RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };

        // ウィンドウのクライアント領域のサイズから、ウィンドウ全体の領域のサイズを求める
        AdjustWindowRect(&rc, windowStyle, FALSE);

        // ウィンドウを生成する
        HWND hwnd = CreateWindowExW(
            0, wcex.lpszClassName, wWindowTitle, windowStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, wcex.hInstance,
            nullptr
        );
        if (!hwnd) { error(L"ウィンドウの生成に失敗しました。"); return 1; }

        // ウィンドウの表示状態を設定する
        ShowWindow(hwnd, nCmdShow);

        // ウィンドウのGWLP_USERDATA属性にg_gameのポインタを設定する
        // これはWndProcからg_gameへアクセスできるようにするためである
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()));

        // 生成したウィンドウのクライアント領域を取得する
        GetClientRect(hwnd, &rc);

        // g_gameを初期化する
        g_game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);
    }

	return 0;
}

int gd::Window::waitUntilExit()
{
    // POD型では空の初期化子リストは0に初期化される
    MSG msg = {};

    // 終了するまで無限ループする
    while (WM_QUIT != msg.message)
    {
        // ウィンドウメッセージが送られてきた場合はそれを処理する
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // ウィンドウを更新する
        else
        {
            g_game->Tick();
        }
    }

    // wWinMain関数が返すべき値を返す
    return static_cast<int>(msg.wParam);
}

void gd::Window::exit()
{
    // スレッドのメッセージキューにWM_QUITを送る
    PostQuitMessage(0);
}

void gd::Window::destroy()
{
	// Gameインスタンスをリセットする
	g_game.reset();

	// 現在のスレッドでCOMライブラリを閉じる
	CoUninitialize();
}

LRESULT CALLBACK gd::Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;
    static bool s_fullscreen = false;
    // TODO: Set s_fullscreen to true if defaulting to fullscreen.

    auto game = reinterpret_cast<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_PAINT:
        if (s_in_sizemove && game)
        {
            game->Tick();
        }
        else
        {
            PAINTSTRUCT ps;
            (void)BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                if (!s_in_suspend && game)
                    game->OnSuspending();
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            if (s_in_suspend && game)
                game->OnResuming();
            s_in_suspend = false;
        }
        else if (!s_in_sizemove && game)
        {
            game->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
        if (game)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);

            game->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
        }
        break;

    case WM_GETMINMAXINFO:
        if (lParam)
        {
            auto info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
        break;

    case WM_ACTIVATEAPP:
        if (game)
        {
            if (wParam)
            {
                game->OnActivated();
            }
            else
            {
                game->OnDeactivated();
            }
        }
        break;

    case WM_POWERBROADCAST:
        switch (wParam)
        {
        case PBT_APMQUERYSUSPEND:
            if (!s_in_suspend && game)
                game->OnSuspending();
            s_in_suspend = true;
            return TRUE;

        case PBT_APMRESUMESUSPEND:
            if (!s_minimized)
            {
                if (s_in_suspend && game)
                    game->OnResuming();
                s_in_suspend = false;
            }
            return TRUE;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            if (s_fullscreen)
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

                int width = 800;
                int height = 600;
                if (game)
                    game->GetDefaultSize(width, height);

                ShowWindow(hWnd, SW_SHOWNORMAL);

                SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
            else
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, 0);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                ShowWindow(hWnd, SW_SHOWMAXIMIZED);
            }

            s_fullscreen = !s_fullscreen;
        }
        break;

    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}