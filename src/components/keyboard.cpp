#include "components/keyboard.h"

gd::KeyboardProcess::KeyboardProcess()
{

}

gd::KeyboardProcess::~KeyboardProcess()
{

}

void gd::KeyboardProcess::OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	uint8_t key = static_cast<uint8_t>(wParam);
	switch (message)
	{
	case WM_KEYDOWN:
		aKeys.emplace(wParam);
		break;
	case WM_KEYUP:
		aKeys.erase(wParam);
		break;
	}
}

void gd::KeyboardProcess::nextFrame()
{
	// キー入力を更新
	cKeys = bKeys;
	bKeys = aKeys;
}

gd::Keyboard gd::KeyboardProcess::getKeyboardStatus() const
{
	Keyboard keyboard{ bKeys, cKeys };

	return keyboard;
}