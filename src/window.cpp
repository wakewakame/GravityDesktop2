#include "window.h"
#include "pch.h"
#include "Game.h"

#include <vector>
#include <codecvt>
#include <locale>

using namespace DirectX;

extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

gd::Windows::Windows(HINSTANCE hInstance, int nCmdShow) : hInstance(hInstance), nCmdShow(nCmdShow), createCount(0)
{
    // DirectXMathライブラリが使用可能かを確認する
    if (!XMVerifyCPUSupport()) { error(L"このPCではDirectXMathが利用できません。"); }

    // 現在のスレッドでCOMライブラリを初期化する
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr)) { error(L"COMライブラリの初期化に失敗しました。"); }
}

gd::Windows::~Windows()
{
    /*
    次のようなプログラムはイテレータの破壊を起こします。
    
    for (auto gameItr = games.begin(); gameItr != games.end(); ++gameItr)
    {
        DestroyWindow(gameItr->first);
    }
    
    これはDestroyWindowの中で処理がWinProcにディスパッチされ、その中でgames.eraseが実行されるためです。
    これを避けるため、ウィンドウハンドルの配列を作成してからウィンドウを破棄します。
    */

    // 全てのウィンドウハンドルを取得する
    std::vector<HWND> hwndArray;
    hwndArray.reserve(games.size());
    for (auto gameItr = games.begin(); gameItr != games.end(); ++gameItr)
    {
        hwndArray.push_back(gameItr->first);
    }

    // 全てのウィンドウを破棄する
    for (HWND hwnd : hwndArray) DestroyWindow(hwnd);
    assert(0 == games.size());

    // COMライブラリを閉じる
    CoUninitialize();
}

int gd::Windows::create(
    const int32_t width, const int32_t height, const std::string& windowTitle,
    DWORD windowStyle, bool enableDoubleClick
)
{
    // ウィンドウクラスの名前の重複を避けるために語尾に数字を追加する
    std::string windowClass = windowTitle + std::to_string(createCount++);

    // std::stringからLPCWSTRに変換する
    using convert_t = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_t, wchar_t> strconverter;
    std::wstring wWindowTitle_ = strconverter.from_bytes(windowTitle);
    std::wstring wWindowClass_ = strconverter.from_bytes(windowClass);
    LPCWSTR wWindowTitle = wWindowTitle_.c_str();
    LPCWSTR wWindowClass = wWindowClass_.c_str();

	// Gameインスタンスを生成する
    std::unique_ptr<Game> game = std::make_unique<Game>();

    // ウィンドウクラスの登録とウィンドウを生成する
    {
        // POD型では空の初期化子リストは0に初期化される
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);

        // サイズが変わったとき，ウインドウ全体を再描画する
        wcex.style |= CS_HREDRAW | CS_VREDRAW;

        // ダブルクリックを有効化する
        if (enableDoubleClick) wcex.style |= CS_DBLCLKS;

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
        if (!RegisterClassExW(&wcex)) { error(L"ウィンドウクラスの登録に失敗しました。"); return 1; }

        // 生成するウィンドウサイズを取得する
        int w, h;
        game->GetDefaultSize(w, h);
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

        // ウィンドウのGWLP_USERDATA属性にgameのポインタを設定する
        // これはWndProcからgameへアクセスできるようにするためである
        //SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(game.get()));
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&games));

        // 生成したウィンドウのクライアント領域を取得する
        GetClientRect(hwnd, &rc);

        // gameを初期化する
        game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);

        // gameをgamesに登録する
        assert(0 == games.count(hwnd));
        games.insert(std::make_pair(hwnd, std::move(game)));
    }

	return 0;
}

int gd::Windows::waitUntilExit()
{
    // POD型では空の初期化子リストは0に初期化される
    MSG msg = {};

    // 全てのウィンドウが終了するまでループする
    while (games.size() > 0)
    {
        // ウィンドウメッセージが送られてきた場合はそれを処理する
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // WndProcにメッセージをディスパッチする
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // WM_QUITが送られてきたら終了する
            if (WM_QUIT != msg.message) break;
        }
        // ウィンドウを更新する
        else
        {
            for (auto gameItr = games.begin(); gameItr != games.end(); ++gameItr)
            {
                gameItr->second->Tick();
            }
        }
    }

    return 0;
}

void gd::Windows::exit()
{
    // スレッドのメッセージキューにWM_QUITを送る
    PostQuitMessage(0);
}

void gd::Windows::error(const std::string& description)
{
    // std::stringからLPCWSTRに変換する
    using convert_t = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_t, wchar_t> strconverter;
    std::wstring wDescription_ = strconverter.from_bytes(description);
    LPCWSTR wDescription = wDescription_.c_str();
    error(wDescription);
}

void gd::Windows::error(LPCWSTR description)
{
    // エラーダイアログを表示する
    MessageBoxW(nullptr, description, L"error", MB_OK | MB_ICONERROR);
}

LRESULT CALLBACK gd::Windows::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // GWLP_USERDATA属性に設定したgameのインスタンスを取得する
    auto games = reinterpret_cast<Games*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if ((nullptr == games) || (0 == games->count(hWnd)))
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    std::unique_ptr<Game>& game = games->at(hWnd);

    // 送られてきたウィンドウメッセージをgameにも送る
    game->GetMessage(hWnd, message, wParam, lParam);

    // WM_DESTROYが送られてきたらgamesからウィンドウハンドルを削除する
    if (WM_DESTROY == message)
    {
        games->erase(hWnd);
    }

    // そのほかのメッセージはデフォルトの処理を行う
    return DefWindowProc(hWnd, message, wParam, lParam);
}