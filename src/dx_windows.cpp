#include "dx_windows.h"

using namespace DirectX;

size_t gd::Windows::createCount = 0;

gd::Windows::Windows(HINSTANCE hInstance, int nCmdShow) : hInstance(hInstance), nCmdShow(nCmdShow)
{
    /*
    メモ : CoInitializeExの第二引数にCOINIT_APARTMENTTHREADEDを指定している理由について

    DirectX3D自体は簡易COMを使用しているが、Direct3D Audioなどはフルスペックの
    COMに依存したコンポーネントがある。
    CoInitializeExの第二引数にCOINITBASE_MULTITHREADEDなどを指定した状態でフ
    ルスペックのCOMに依存したコンポーネントを使用するとヒープメモリを破壊される可能
    性があるため、ここではCOINIT_APARTMENTTHREADEDを指定した。
    */

    // DirectXMathライブラリが使用可能かを確認する
    if (!XMVerifyCPUSupport()) { error(L"このPCではDirectXMathが利用できません。"); }
}

gd::Windows::~Windows()
{
    /*
    次のようなプログラムはイテレータの破壊を起こします。
    
    for (auto windowItr = windows.begin(); windowItr != windows.end(); ++windowItr)
    {
        DestroyWindow(windowItr->first);
    }
    
    これはDestroyWindowの中で処理がWinProcにディスパッチされ、その中でwindows.eraseが実行されるためです。
    これを避けるため、ウィンドウハンドルの配列を作成してからウィンドウを破棄します。
    */

    // 全てのウィンドウハンドルを取得する
    std::vector<HWND> hwndArray;
    hwndArray.reserve(windows.size());
    for (auto windowItr = windows.begin(); windowItr != windows.end(); ++windowItr)
    {
        hwndArray.push_back(windowItr->first);
    }

    // 全てのウィンドウを破棄する
    for (HWND hwnd : hwndArray) DestroyWindow(hwnd);
    assert(0 == windows.size());
}

int gd::Windows::create(std::unique_ptr<RootComponent>&& rootComponent)
{
    assert(static_cast<bool>(rootComponent));

    // rootComponentにhInstanceを渡す
    rootComponent->setInstance(hInstance);

    // ウィンドウクラスの名前の重複を避けるために語尾に数字を追加する
    std::string windowClass = rootComponent->getTitle() + std::to_string(createCount++);

    // std::stringからLPCWSTRに変換する
    using convert_t = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_t, wchar_t> strconverter;
    std::wstring wWindowTitle_ = strconverter.from_bytes(rootComponent->getTitle());
    std::wstring wWindowClass_ = strconverter.from_bytes(windowClass);
    LPCWSTR wWindowTitle = wWindowTitle_.c_str();
    LPCWSTR wWindowClass = wWindowClass_.c_str();

    // ウィンドウクラスの登録とウィンドウを生成する
    {
        // POD型では空の初期化子リストは0に初期化される
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);

        // ウィンドウクラスのスタイルを指定する
        wcex.style = rootComponent->getWindowClassStyle();

        // 送られてくるウィンドウメッセージを処理するためのコールバック関数(ウィンドウプロシージャ)を指定する
        wcex.lpfnWndProc = WndProc;

        // インスタンスへのハンドルを指定する
        wcex.hInstance = hInstance;

        // アプリケーションのアイコンを指定する
        wcex.hIcon = rootComponent->getIcon();
        wcex.hIconSm = rootComponent->getIcon();

        // ウィンドウのマウスカーソルを指定する
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);

        // 背景ブラシへのハンドルを指定する　(選択した値に+1する必要がある)
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

        // ウィンドウクラス名を指定する
        wcex.lpszClassName = wWindowClass;

        // ウィンドウクラスを登録する
        if (!RegisterClassExW(&wcex)) { error(L"ウィンドウクラスの登録に失敗しました。"); return 1; }

        // 生成するウィンドウサイズを取得する
        SIZE windowSize = rootComponent->getDefaultSize();
        RECT rc = { 0, 0, windowSize.cx, windowSize.cy };

        // ウィンドウのクライアント領域のサイズから、ウィンドウ全体の領域のサイズを求める
        DWORD windowStyle = rootComponent->getWindowStyle();
        AdjustWindowRect(&rc, windowStyle, FALSE);

        // ウィンドウを生成する
        HWND hWnd = CreateWindowExW(
            0, wcex.lpszClassName, wWindowTitle, windowStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, wcex.hInstance,
            reinterpret_cast<LPVOID>(this)
        );
        if (!hWnd) { error(L"ウィンドウの生成に失敗しました。"); return 1; }

        // ウィンドウの表示状態を設定する
        ShowWindow(hWnd, nCmdShow);

        // 生成したウィンドウのクライアント領域を取得する
        GetClientRect(hWnd, &rc);

        // DxWindowインスタンスを生成する
        std::unique_ptr<Window> window = std::make_unique<Window>();

        // windowを初期化する
        window->Initialize(hWnd, rc.right - rc.left, rc.bottom - rc.top);

        // windowにrootComponentを登録する
        window->SetRootComponent(std::move(rootComponent));

        // windowをwindowsに登録する
        windows.insert(std::make_pair(hWnd, std::move(window)));
    }

	return 0;
}

int gd::Windows::waitUntilExit()
{
    // POD型では空の初期化子リストは0に初期化される
    MSG msg = {};

    // 全てのウィンドウが終了するまでループする
    while (windows.size() > 0)
    {
        // ウィンドウメッセージが送られてきた場合はそれを処理する
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // WndProcにメッセージをディスパッチする
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // WM_QUITが送られてきたら終了する
            if (WM_QUIT == msg.message) break;
        }
        // ウィンドウを更新する
        else
        {
            for (auto windowItr = windows.begin(); windowItr != windows.end(); ++windowItr)
            {
                windowItr->second->Tick();
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

LRESULT CALLBACK gd::Windows::SubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (windows.count(hWnd))
    {
        // 送られてきたウィンドウメッセージをgameにも送る
        windows[hWnd]->OnWindowMessage(uMsg, wParam, lParam);

        // ウィンドウが破棄されようとしているときはwindowを破棄する
        if (WM_DESTROY == uMsg)
        {
            windows.erase(hWnd);
        }
    }

    // そのほかのメッセージはデフォルトの処理を行う
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK gd::Windows::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // CreateWindowExWによってウィンドウが生成されるとき
    if (WM_NCCREATE == uMsg)
    {
        // ウィンドウサブクラスの関数を定義
        // uIdSubclassには自身のsubClassProcのポインタを入れる
        // dwRefDataにはgd::Windowsのインスタンスを入れる
        static SUBCLASSPROC const subClassProc = [](
            HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData
        ) -> LRESULT
        {
            assert(dwRefData);

            // gd::Windowsのメンバ関数のSubProcを呼び出す
            LRESULT const result = reinterpret_cast<Windows*>(dwRefData)->SubProc(hWnd, uMsg, wParam, lParam);

            // ウィンドウが破棄される前にサブクラスの登録を解除する
            if (WM_DESTROY == uMsg) { RemoveWindowSubclass(hWnd, reinterpret_cast<SUBCLASSPROC>(uIdSubclass), uIdSubclass); }

            return result;
        };

        // subClassProcをウィンドウサブクラスとして登録する
        CREATESTRUCT* const createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        if (!SetWindowSubclass(
            hWnd, subClassProc, reinterpret_cast<UINT_PTR>(subClassProc), reinterpret_cast<DWORD_PTR>(createStruct->lpCreateParams)
        ))
        {
            // サブクラス化が失敗したため0を返す
            // これによりCreateWindowExWでNULLが返されるようになる
            return 0;
        }
    }

    // あとはデフォルトのウィンドウプロシージャに任せる
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}