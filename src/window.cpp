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
	// DirectXMath���C�u�������g�p�\�����m�F����
	if (!XMVerifyCPUSupport()) { error(L"����PC�ł�DirectXMath�����p�ł��܂���B"); return 1; }

	// ���݂̃X���b�h��COM���C�u����������������
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr)) { error(L"COM���C�u�����̏������Ɏ��s���܂����B"); return 1; }

	// Game�C���X�^���X�𐶐�����
	g_game = std::make_unique<Game>();

	// �E�B���h�E�^�C�g����std::string����LPCWSTR�ɕϊ�����
	using convert_t = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_t, wchar_t> strconverter;
	std::wstring wWindowTitle_ = strconverter.from_bytes(windowTitle);
	std::wstring wWindowClass_ = strconverter.from_bytes(windowTitle + " class");
    LPCWSTR wWindowTitle = wWindowTitle_.c_str();
    LPCWSTR wWindowClass = wWindowClass_.c_str();

    // �E�B���h�E�N���X�̓o�^�ƃE�B���h�E�𐶐�����
    {
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);

        // CS_HREDRAW: ���T�C�Y���ς�����Ƃ��C�E�C���h�E�S�̂��ĕ`�悷��
        // CS_VREDRAW: �c�T�C�Y���ς�����Ƃ��C�E�C���h�E�S�̂��ĕ`�悷��
        // CS_DBLCLKS: �_�u���N���b�N��L��������
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

        // �����Ă���E�B���h�E���b�Z�[�W���������邽�߂̃R�[���o�b�N�֐�(�E�B���h�E�v���V�[�W��)���w�肷��
        wcex.lpfnWndProc = WndProc;

        // �C���X�^���X�ւ̃n���h�����w�肷��
        wcex.hInstance = hInstance;

        // �A�v���P�[�V�����̃A�C�R�����w�肷��
        wcex.hIcon = LoadIconW(nullptr, L"IDI_APPLICATION");
        wcex.hIconSm = LoadIconW(nullptr, L"IDI_APPLICATION");

        // �E�B���h�E�̃}�E�X�J�[�\�����w�肷��
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);

        // �w�i�u���V�ւ̃n���h�����w�肷��@(�I�������l��+1����K�v������)
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

        // �E�B���h�E�N���X�����w�肷��
        wcex.lpszClassName = wWindowClass;

        // �E�B���h�E�N���X��o�^����
        RegisterClassExW(&wcex);

        // ��������E�B���h�E�T�C�Y���擾����
        int w, h;
        g_game->GetDefaultSize(w, h);
        RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };

        // �E�B���h�E�̃N���C�A���g�̈�̃T�C�Y����A�E�B���h�E�S�̗̂̈�̃T�C�Y�����߂�
        AdjustWindowRect(&rc, windowStyle, FALSE);

        // �E�B���h�E�𐶐�����
        HWND hwnd = CreateWindowExW(
            0, wcex.lpszClassName, wWindowTitle, windowStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, wcex.hInstance,
            nullptr
        );
        if (!hwnd) { error(L"�E�B���h�E�̐����Ɏ��s���܂����B"); return 1; }

        // �E�B���h�E�̕\����Ԃ�ݒ肷��
        ShowWindow(hwnd, nCmdShow);

        // �E�B���h�E��GWLP_USERDATA������g_game�̃|�C���^��ݒ肷��
        // �����WndProc����g_game�փA�N�Z�X�ł���悤�ɂ��邽�߂ł���
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()));

        // ���������E�B���h�E�̃N���C�A���g�̈���擾����
        GetClientRect(hwnd, &rc);

        // g_game������������
        g_game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);
    }

	return 0;
}

int gd::Window::waitUntilExit()
{
    // POD�^�ł͋�̏������q���X�g��0�ɏ����������
    MSG msg = {};

    // �I������܂Ŗ������[�v����
    while (WM_QUIT != msg.message)
    {
        // �E�B���h�E���b�Z�[�W�������Ă����ꍇ�͂������������
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // �E�B���h�E���X�V����
        else
        {
            g_game->Tick();
        }
    }

    // wWinMain�֐����Ԃ��ׂ��l��Ԃ�
    return static_cast<int>(msg.wParam);
}

void gd::Window::exit()
{
    // �X���b�h�̃��b�Z�[�W�L���[��WM_QUIT�𑗂�
    PostQuitMessage(0);
}

void gd::Window::destroy()
{
	// Game�C���X�^���X�����Z�b�g����
	g_game.reset();

	// ���݂̃X���b�h��COM���C�u���������
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