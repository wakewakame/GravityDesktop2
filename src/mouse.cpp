#include "mouse.h"

gd::MouseProcess::MouseProcess()
{

}

gd::MouseProcess::~MouseProcess()
{

}

void gd::MouseProcess::OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINTS p = (*((POINTS FAR*) & (lParam)));

	switch (message)
	{
	case WM_MOUSEMOVE:
		aMouse = p;
		break;

	case WM_LBUTTONDOWN:
		// �h���b�O���ɉ�ʊO�փ}�E�X���ړ����Ă��L���v�`���𑱂���
		if (!isCapture) { SetCapture(hWnd); isCapture = true; }
		else { ReleaseCapture(); isCapture = false; }
		aLPressed = true;
		aLDragStart = p;
		break;

	case WM_RBUTTONDOWN:
		// �h���b�O���ɉ�ʊO�փ}�E�X���ړ����Ă��L���v�`���𑱂���
		if (!isCapture) { SetCapture(hWnd); isCapture = true; }
		else { ReleaseCapture(); isCapture = false; }
		aRPressed = true;
		aRDragStart = p;
		break;

	case WM_MBUTTONDOWN:
		// �h���b�O���ɉ�ʊO�փ}�E�X���ړ����Ă��L���v�`���𑱂���
		if (!isCapture) { SetCapture(hWnd); isCapture = true; }
		else { ReleaseCapture(); isCapture = false; }
		aMPressed = true;
		aMDragStart = p;
		break;

	case WM_LBUTTONUP:
		if (isCapture) { ReleaseCapture(); isCapture = false; }
		aLPressed = false;
		break;

	case WM_RBUTTONUP:
		if (isCapture) { ReleaseCapture(); isCapture = false; }
		aRPressed = false;
		break;

	case WM_MBUTTONUP:
		if (isCapture) { ReleaseCapture(); isCapture = false; }
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
	// �}�E�X���W���X�V
	cMouse = bMouse;
	bMouse = aMouse;

	bLDragStart = aLDragStart;
	bRDragStart = aRDragStart;
	bMDragStart = aMDragStart;

	// �N���b�N�����X�V
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