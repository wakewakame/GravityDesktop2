#include "mouse.h"

gd::MouseProcess::MouseProcess()
{

}

gd::MouseProcess::~MouseProcess()
{

}

void gd::MouseProcess::OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
		aMouse.x = static_cast<LONG>(LOWORD(lParam));
		aMouse.y = static_cast<LONG>(HIWORD(lParam));
		break;

	case WM_LBUTTONDOWN:
		aLPressed = true;
		aLDragStart.x = static_cast<LONG>(LOWORD(lParam));
		aLDragStart.y = static_cast<LONG>(HIWORD(lParam));
		break;

	case WM_RBUTTONDOWN:
		aRPressed = true;
		aRDragStart.x = static_cast<LONG>(LOWORD(lParam));
		aRDragStart.y = static_cast<LONG>(HIWORD(lParam));
		break;

	case WM_MBUTTONDOWN:
		aMPressed = true;
		aMDragStart.x = static_cast<LONG>(LOWORD(lParam));
		aMDragStart.y = static_cast<LONG>(HIWORD(lParam));
		break;

	case WM_LBUTTONUP:
		aLPressed = false;
		break;

	case WM_RBUTTONUP:
		aRPressed = false;
		break;

	case WM_MBUTTONUP:
		aMPressed = false;
		break;

	case WM_LBUTTONDBLCLK:
		aLDouble = true;
		break;

	case WM_RBUTTONDBLCLK:
		aRDouble = true;
		break;

	case WM_MBUTTONDBLCLK:
		aMDouble = true;
		break;
	}
}

void gd::MouseProcess::nextFrame()
{
	// マウス座標を更新
	cMouse = bMouse;
	bMouse = aMouse;

	bLDragStart = aLDragStart;
	bRDragStart = aRDragStart;
	bMDragStart = aMDragStart;

	// クリック情報を更新
	cLPressed = bLPressed;
	bLPressed = aLPressed;

	cRPressed = bRPressed;
	bRPressed = aRPressed;

	cMPressed = bMPressed;
	bMPressed = aMPressed;

	bLDouble = aLDouble;
	aLDouble = false;

	bRDouble = aRDouble;
	aRDouble = false;

	bMDouble = aMDouble;
	aMDouble = false;
}

gd::Mouse gd::MouseProcess::getMouseStatus()
{
	Mouse mouse;

	mouse.point  = bMouse;
	mouse.point_ = cMouse;

	mouse.lDragStart = bLDragStart;
	mouse.rDragStart = bRDragStart;
	mouse.mDragStart = bMDragStart;

	mouse.lPressed  = bLPressed;
	mouse.lPressed_ = cLPressed;

	mouse.rPressed  = bRPressed;
	mouse.rPressed_ = cRPressed;

	mouse.mPressed  = bMPressed;
	mouse.mPressed_ = cMPressed;

	mouse.lDouble = bLDouble;
	mouse.rDouble = bRDouble;
	mouse.mDouble = bMDouble;

	return mouse;
}