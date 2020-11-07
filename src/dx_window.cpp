#include "dx_window.h"

using namespace DirectX;

using Microsoft::WRL::ComPtr;

extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

gd::Window::Window() noexcept :
    m_window(nullptr),
    m_outputWidth(800),
    m_outputHeight(600)
{
}

// Initialize the Direct3D resources required to run.
void gd::Window::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

// Executes the basic game loop.
void gd::Window::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void gd::Window::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.

    // rootComponent::updateを呼び出す
    if (root_component)
    {
        root_component->update(elapsedTime);
    }
}

// Draws the scene.
void gd::Window::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    // TODO: Add your rendering code here.

    // rootComponent::renderを呼び出す
    if (root_component)
    {
        root_component->render(graph);
    }

    Present();
}

// Helper method to clear the back buffers.
void gd::Window::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void gd::Window::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void gd::Window::OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case static_cast<UINT>(WM_APP_LIST::EXIT):
        DestroyWindow(m_window);
        break;

    case WM_PAINT:
        if (s_in_sizemove)
        {
            Tick();
        }
        else
        {
            PAINTSTRUCT ps;
            (void)BeginPaint(m_window, &ps);
            EndPaint(m_window, &ps);
        }
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                if (!s_in_suspend)
                    OnSuspending();
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            if (s_in_suspend)
                OnResuming();
            s_in_suspend = false;
        }
        else if (!s_in_sizemove)
        {
            OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;

        RECT rc;
        GetClientRect(m_window, &rc);

        OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
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
        if (wParam)
        {
            OnActivated();
        }
        else
        {
            OnDeactivated();
        }
        break;

    case WM_POWERBROADCAST:
        switch (wParam)
        {
        case PBT_APMQUERYSUSPEND:
            if (!s_in_suspend)
                OnSuspending();
            s_in_suspend = true;
            return;

        case PBT_APMRESUMESUSPEND:
            if (!s_minimized)
            {
                if (s_in_suspend)
                    OnResuming();
                s_in_suspend = false;
            }
            return;
        }
        break;
    }
}

void gd::Window::SetRootComponent(std::unique_ptr<gd::RootComponent>&& root_component)
{
    if (!static_cast<bool>(root_component)) return;
    this->root_component.swap(root_component);
    this->root_component->setHwnd(m_window);
    this->root_component->init(graph);
}

void gd::Window::OnActivated()
{
    // TODO: Game is becoming active window.
}

void gd::Window::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void gd::Window::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void gd::Window::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void gd::Window::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

void gd::Window::CreateDevice()
{
    /*
    メモ
    _DEBUGはDebugビルド時に定#defineされる
    NDEBUFはReleaseビルド時に#defineされる
    _DEBUGはVCの独自拡張で、NDEBUFはC言語標準
    */

    // ID3D11DeviceとID3D11DeviceContextの作成
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    static const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    DX::ThrowIfFailed(D3D11CreateDevice(
        nullptr,                         // nullptrでデフォルトのIDXGIAdapterを使用する
        D3D_DRIVER_TYPE_HARDWARE,        // ハードウェアドライバーを使用する
        nullptr,                         // ソフトウェアラスタライザーを実装するDLLへのハンドル
#ifdef _DEBUG
        D3D11_CREATE_DEVICE_DEBUG,       // デバッグモードの時はデバッグレイヤーを有効にする
#else
        0,
#endif
        featureLevels,                   // 作成を試みる機能レベルの順序を決定するD3D_FEATURE_LEVELの配列へのポインター
        _countof(featureLevels),         // featureLevelsの要素数
        D3D11_SDK_VERSION,               // SDKバージョン
        device.ReleaseAndGetAddressOf(), // 作成されたデバイスを表すID3D11Deviceオブジェクトへのポインタ
        &featureLevel,                   // featureLevelsの中で作成に使用された値がfeatureLevelに代入される
        context.ReleaseAndGetAddressOf() // デバイスコンテキストを表すID3D11DeviceContextオブジェクトへのポインタ
    ));

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide[] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    /*
    ComPtr::Asはポインタのコピーにdynamic_castみたいな処理をしている
    しかし、以下の処理はポインタのコピー元もコピー先も同じ型なので
    代入演算子を使ってポインタをコピーしたほうがいいのではないかと思う
    ComPtrの代入演算子を使用していない理由はよくわからないけど
    公式のサンプルプログラムがこうしていたので、マネをしておく
    */
    DX::ThrowIfFailed(device.As(&m_d3dDevice));
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    // GraphのインスタンスにID3D11DeviceContextを渡す
    graph.CreateDevice(m_d3dContext);
}

// Allocate all memory resources that change on a window SizeChanged event.
void gd::Window::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    constexpr UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            m_swapChain.ReleaseAndGetAddressOf()
        ));

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
    graph.CreateResources(backBufferWidth, backBufferHeight);
}

void gd::Window::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}
