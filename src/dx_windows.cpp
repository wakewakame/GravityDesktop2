#include "dx_windows.h"

using namespace DirectX;

size_t gd::Windows::createCount = 0;

gd::Windows::Windows(HINSTANCE hInstance, int nCmdShow) : hInstance(hInstance), nCmdShow(nCmdShow)
{
    // DirectXMath���C�u�������g�p�\�����m�F����
    if (!XMVerifyCPUSupport()) { error(L"����PC�ł�DirectXMath�����p�ł��܂���B"); }

    // ���݂̃X���b�h��COM���C�u����������������
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr)) { error(L"COM���C�u�����̏������Ɏ��s���܂����B"); }
}

gd::Windows::~Windows()
{
    /*
    ���̂悤�ȃv���O�����̓C�e���[�^�̔j����N�����܂��B
    
    for (auto windowItr = windows.begin(); windowItr != windows.end(); ++windowItr)
    {
        DestroyWindow(windowItr->first);
    }
    
    �����DestroyWindow�̒��ŏ�����WinProc�Ƀf�B�X�p�b�`����A���̒���windows.erase�����s����邽�߂ł��B
    ���������邽�߁A�E�B���h�E�n���h���̔z����쐬���Ă���E�B���h�E��j�����܂��B
    */

    // �S�ẴE�B���h�E�n���h�����擾����
    std::vector<HWND> hwndArray;
    hwndArray.reserve(windows.size());
    for (auto windowItr = windows.begin(); windowItr != windows.end(); ++windowItr)
    {
        hwndArray.push_back(windowItr->first);
    }

    // �S�ẴE�B���h�E��j������
    for (HWND hwnd : hwndArray) DestroyWindow(hwnd);
    assert(0 == windows.size());

    // COM���C�u���������
    CoUninitialize();
}

int gd::Windows::create(
    const std::string& windowTitle,
    DWORD windowStyle, bool enableDoubleClick
)
{
    // �E�B���h�E�N���X�̖��O�̏d��������邽�߂Ɍ���ɐ�����ǉ�����
    std::string windowClass = windowTitle + std::to_string(createCount++);

    // std::string����LPCWSTR�ɕϊ�����
    using convert_t = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_t, wchar_t> strconverter;
    std::wstring wWindowTitle_ = strconverter.from_bytes(windowTitle);
    std::wstring wWindowClass_ = strconverter.from_bytes(windowClass);
    LPCWSTR wWindowTitle = wWindowTitle_.c_str();
    LPCWSTR wWindowClass = wWindowClass_.c_str();

	// DxWindow�C���X�^���X�𐶐�����
    std::unique_ptr<Window> window = std::make_unique<Window>();

    // �E�B���h�E�N���X�̓o�^�ƃE�B���h�E�𐶐�����
    {
        // POD�^�ł͋�̏������q���X�g��0�ɏ����������
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);

        // �T�C�Y���ς�����Ƃ��C�E�C���h�E�S�̂��ĕ`�悷��
        wcex.style |= CS_HREDRAW | CS_VREDRAW;

        // �_�u���N���b�N��L��������
        if (enableDoubleClick) wcex.style |= CS_DBLCLKS;

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
        if (!RegisterClassExW(&wcex)) { error(L"�E�B���h�E�N���X�̓o�^�Ɏ��s���܂����B"); return 1; }

        // ��������E�B���h�E�T�C�Y���擾����
        int w, h;
        window->GetDefaultSize(w, h);
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

        // �E�B���h�E��GWLP_USERDATA������windows�̃|�C���^��ݒ肷��
        // �����WndProc����windows�փA�N�Z�X�ł���悤�ɂ��邽�߂ł���
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&windows));

        // ���������E�B���h�E�̃N���C�A���g�̈���擾����
        GetClientRect(hwnd, &rc);

        // window������������
        window->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);

        // window��windows�ɓo�^����
        assert(0 == windows.count(hwnd));
        windows.insert(std::make_pair(hwnd, std::move(window)));
    }

	return 0;
}

int gd::Windows::waitUntilExit()
{
    // POD�^�ł͋�̏������q���X�g��0�ɏ����������
    MSG msg = {};

    // �S�ẴE�B���h�E���I������܂Ń��[�v����
    while (windows.size() > 0)
    {
        // �E�B���h�E���b�Z�[�W�������Ă����ꍇ�͂������������
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // WndProc�Ƀ��b�Z�[�W���f�B�X�p�b�`����
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // WM_QUIT�������Ă�����I������
            if (WM_QUIT == msg.message) break;
        }
        // �E�B���h�E���X�V����
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
    // �X���b�h�̃��b�Z�[�W�L���[��WM_QUIT�𑗂�
    PostQuitMessage(0);
}

void gd::Windows::error(const std::string& description)
{
    // std::string����LPCWSTR�ɕϊ�����
    using convert_t = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_t, wchar_t> strconverter;
    std::wstring wDescription_ = strconverter.from_bytes(description);
    LPCWSTR wDescription = wDescription_.c_str();
    error(wDescription);
}

void gd::Windows::error(LPCWSTR description)
{
    // �G���[�_�C�A���O��\������
    MessageBoxW(nullptr, description, L"error", MB_OK | MB_ICONERROR);
}

LRESULT CALLBACK gd::Windows::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // GWLP_USERDATA�����ɐݒ肵��game�̃C���X�^���X���擾����
    auto windows = reinterpret_cast<std::map<HWND, std::unique_ptr<Window>>*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if ((nullptr == windows) || (0 == windows->count(hWnd)))
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    std::unique_ptr<Window>& window = windows->at(hWnd);

    // �����Ă����E�B���h�E���b�Z�[�W��game�ɂ�����
    window->GetMessage(message, wParam, lParam);

    // WM_DESTROY�������Ă�����games����E�B���h�E�n���h�����폜����
    if (WM_DESTROY == message)
    {
        windows->erase(hWnd);
    }

    // ���̂ق��̃��b�Z�[�W�̓f�t�H���g�̏������s��
    return DefWindowProc(hWnd, message, wParam, lParam);
}