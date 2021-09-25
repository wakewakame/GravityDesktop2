#include "components/root_component.h"
#include "components/dx_window.h"

gd::RootComponent::RootComponent() {};
gd::RootComponent::~RootComponent() {};

void gd::RootComponent::init(gd::Graph& graph)
{
	Component::init(graph);
}
void gd::RootComponent::update(float elapsedTime, const gd::Mouse& mouse, const gd::Keyboard& keyboard)
{
	Component::update(elapsedTime, mouse, keyboard);
}
void gd::RootComponent::render(gd::Graph& graph)
{
	Component::render(graph);
}
void gd::RootComponent::exit(gd::Graph& graph)
{
	Component::exit(graph);
}
void gd::RootComponent::resize(int width, int height)
{
	Component::resize(width, height);
}

void gd::RootComponent::setInstance(HINSTANCE hInstance) { this->hInstance = hInstance; }
HINSTANCE gd::RootComponent::getInstance() { return hInstance; }
void gd::RootComponent::setHwnd(HWND hWnd) { this->hWnd = hWnd; }
HWND gd::RootComponent::getHwnd() { return hWnd; }
std::string gd::RootComponent::getTitle() const { return "window component"; }
SIZE gd::RootComponent::getDefaultSize() const { return SIZE{ 640, 480 }; }
DWORD gd::RootComponent::getWindowStyle() const { return WS_OVERLAPPEDWINDOW; }
UINT gd::RootComponent::getWindowClassStyle() const { return CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; }
HICON gd::RootComponent::getIcon() const { return LoadIconW(nullptr, L"IDI_APPLICATION"); }
void gd::RootComponent::setGDWindow(Window* gdWindow) { this->gdWindow = gdWindow; }
void gd::RootComponent::setSize(int width, int height)
{
	gdWindow->OnWindowSizeChanged(width, height);
}
double gd::RootComponent::getFps() const { return 60.0; }
void gd::RootComponent::closeWindow() { if (hWnd) PostMessage(hWnd, WM_CLOSE, NULL, NULL); }