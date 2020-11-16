#include "dx_windows.h"

using namespace DirectX;

size_t gd::Windows::createCount = 0;

gd::Windows::Windows(HINSTANCE hInstance, int nCmdShow) : hInstance(hInstance), nCmdShow(nCmdShow)
{
    /*
    ���� : CoInitializeEx�̑�������COINIT_APARTMENTTHREADED���w�肵�Ă��闝�R�ɂ���

    DirectX3D���̂͊Ȉ�COM���g�p���Ă��邪�ADirect3D Audio�Ȃǂ̓t���X�y�b�N��
    COM�Ɉˑ������R���|�[�l���g������B
    CoInitializeEx�̑�������COINITBASE_MULTITHREADED�Ȃǂ��w�肵����ԂŃt
    ���X�y�b�N��COM�Ɉˑ������R���|�[�l���g���g�p����ƃq�[�v��������j�󂳂��\
    �������邽�߁A�����ł�COINIT_APARTMENTTHREADED���w�肵���B
    */

    // DirectXMath���C�u�������g�p�\�����m�F����
    if (!XMVerifyCPUSupport()) { error(L"����PC�ł�DirectXMath�����p�ł��܂���B"); }
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
}

int gd::Windows::create(std::unique_ptr<RootComponent>&& rootComponent)
{
    assert(static_cast<bool>(rootComponent));

    // rootComponent��hInstance��n��
    rootComponent->setInstance(hInstance);

    // �E�B���h�E�N���X�̖��O�̏d��������邽�߂Ɍ���ɐ�����ǉ�����
    std::string windowClass = rootComponent->getTitle() + std::to_string(createCount++);

    // std::string����LPCWSTR�ɕϊ�����
    using convert_t = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_t, wchar_t> strconverter;
    std::wstring wWindowTitle_ = strconverter.from_bytes(rootComponent->getTitle());
    std::wstring wWindowClass_ = strconverter.from_bytes(windowClass);
    LPCWSTR wWindowTitle = wWindowTitle_.c_str();
    LPCWSTR wWindowClass = wWindowClass_.c_str();

    // �E�B���h�E�N���X�̓o�^�ƃE�B���h�E�𐶐�����
    {
        // POD�^�ł͋�̏������q���X�g��0�ɏ����������
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);

        // �E�B���h�E�N���X�̃X�^�C�����w�肷��
        wcex.style = rootComponent->getWindowClassStyle();

        // �����Ă���E�B���h�E���b�Z�[�W���������邽�߂̃R�[���o�b�N�֐�(�E�B���h�E�v���V�[�W��)���w�肷��
        wcex.lpfnWndProc = WndProc;

        // �C���X�^���X�ւ̃n���h�����w�肷��
        wcex.hInstance = hInstance;

        // �A�v���P�[�V�����̃A�C�R�����w�肷��
        wcex.hIcon = rootComponent->getIcon();
        wcex.hIconSm = rootComponent->getIcon();

        // �E�B���h�E�̃}�E�X�J�[�\�����w�肷��
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);

        // �w�i�u���V�ւ̃n���h�����w�肷��@(�I�������l��+1����K�v������)
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

        // �E�B���h�E�N���X�����w�肷��
        wcex.lpszClassName = wWindowClass;

        // �E�B���h�E�N���X��o�^����
        if (!RegisterClassExW(&wcex)) { error(L"�E�B���h�E�N���X�̓o�^�Ɏ��s���܂����B"); return 1; }

        // ��������E�B���h�E�T�C�Y���擾����
        SIZE windowSize = rootComponent->getDefaultSize();
        RECT rc = { 0, 0, windowSize.cx, windowSize.cy };

        // �E�B���h�E�̃N���C�A���g�̈�̃T�C�Y����A�E�B���h�E�S�̗̂̈�̃T�C�Y�����߂�
        DWORD windowStyle = rootComponent->getWindowStyle();
        AdjustWindowRect(&rc, windowStyle, FALSE);

        // �E�B���h�E�𐶐�����
        HWND hWnd = CreateWindowExW(
            0, wcex.lpszClassName, wWindowTitle, windowStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, wcex.hInstance,
            reinterpret_cast<LPVOID>(this)
        );
        if (!hWnd) { error(L"�E�B���h�E�̐����Ɏ��s���܂����B"); return 1; }

        // �E�B���h�E�̕\����Ԃ�ݒ肷��
        ShowWindow(hWnd, nCmdShow);

        // ���������E�B���h�E�̃N���C�A���g�̈���擾����
        GetClientRect(hWnd, &rc);

        // DxWindow�C���X�^���X�𐶐�����
        std::unique_ptr<Window> window = std::make_unique<Window>();

        // window������������
        window->Initialize(hWnd, rc.right - rc.left, rc.bottom - rc.top);

        // window��rootComponent��o�^����
        window->SetRootComponent(std::move(rootComponent));

        // window��windows�ɓo�^����
        windows.insert(std::make_pair(hWnd, std::move(window)));
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

LRESULT CALLBACK gd::Windows::SubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (windows.count(hWnd))
    {
        // �����Ă����E�B���h�E���b�Z�[�W��game�ɂ�����
        windows[hWnd]->OnWindowMessage(uMsg, wParam, lParam);

        // �E�B���h�E���j������悤�Ƃ��Ă���Ƃ���window��j������
        if (WM_DESTROY == uMsg)
        {
            windows.erase(hWnd);
        }
    }

    // ���̂ق��̃��b�Z�[�W�̓f�t�H���g�̏������s��
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK gd::Windows::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // CreateWindowExW�ɂ���ăE�B���h�E�����������Ƃ�
    if (WM_NCCREATE == uMsg)
    {
        // �E�B���h�E�T�u�N���X�̊֐����`
        // uIdSubclass�ɂ͎��g��subClassProc�̃|�C���^������
        // dwRefData�ɂ�gd::Windows�̃C���X�^���X������
        static SUBCLASSPROC const subClassProc = [](
            HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData
        ) -> LRESULT
        {
            assert(dwRefData);

            // gd::Windows�̃����o�֐���SubProc���Ăяo��
            LRESULT const result = reinterpret_cast<Windows*>(dwRefData)->SubProc(hWnd, uMsg, wParam, lParam);

            // �E�B���h�E���j�������O�ɃT�u�N���X�̓o�^����������
            if (WM_DESTROY == uMsg) { RemoveWindowSubclass(hWnd, reinterpret_cast<SUBCLASSPROC>(uIdSubclass), uIdSubclass); }

            return result;
        };

        // subClassProc���E�B���h�E�T�u�N���X�Ƃ��ēo�^����
        CREATESTRUCT* const createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        if (!SetWindowSubclass(
            hWnd, subClassProc, reinterpret_cast<UINT_PTR>(subClassProc), reinterpret_cast<DWORD_PTR>(createStruct->lpCreateParams)
        ))
        {
            // �T�u�N���X�������s��������0��Ԃ�
            // ����ɂ��CreateWindowExW��NULL���Ԃ����悤�ɂȂ�
            return 0;
        }
    }

    // ���Ƃ̓f�t�H���g�̃E�B���h�E�v���V�[�W���ɔC����
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}