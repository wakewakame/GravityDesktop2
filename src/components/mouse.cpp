#include "components/mouse.h"

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

	case WM_LBUTTONDBLCLK:
		aLDouble = true;
		[[fallthrough]];
	case WM_LBUTTONDOWN:
		// �h���b�O���ɉ�ʊO�փ}�E�X���ړ����Ă��L���v�`���𑱂���
		if (setMouseCapture(hWnd))
		{
			aLPressed = true;
			aLDragStart = p;
		}
		break;

	case WM_RBUTTONDBLCLK:
		aRDouble = true;
		[[fallthrough]];
	case WM_RBUTTONDOWN:
		// �h���b�O���ɉ�ʊO�փ}�E�X���ړ����Ă��L���v�`���𑱂���
		if (setMouseCapture(hWnd))
		{
			aRPressed = true;
			aRDragStart = p;
		}
		break;

	case WM_MBUTTONDBLCLK:
		aMDouble = true;
		[[fallthrough]];
	case WM_MBUTTONDOWN:
		// �h���b�O���ɉ�ʊO�փ}�E�X���ړ����Ă��L���v�`���𑱂���
		if (setMouseCapture(hWnd))
		{
			aMPressed = true;
			aMDragStart = p;
		}
		break;

	case WM_LBUTTONUP:
		// �}�E�X�̃L���v�`�����I�����āAaLPressed�AaRPressed�AaMPressed��false��ݒ肷��
		releaseMouseCapture();
		break;

	case WM_RBUTTONUP:
		// �}�E�X�̃L���v�`�����I�����āAaLPressed�AaRPressed�AaMPressed��false��ݒ肷��
		releaseMouseCapture();
		break;

	case WM_MBUTTONUP:
		// �}�E�X�̃L���v�`�����I�����āAaLPressed�AaRPressed�AaMPressed��false��ݒ肷��
		releaseMouseCapture();
		break;

	case WM_MOUSEWHEEL:
		aZDelta += GET_WHEEL_DELTA_WPARAM(wParam);
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

	bZDelta = aZDelta;
	aZDelta = 0;
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

	mouse.zDelta = bZDelta;

	return mouse;
}

bool gd::MouseProcess::setMouseCapture(HWND hWnd)
{
	if (!isCaptured) { SetCapture(hWnd); isCaptured = true; return true; }
	else { releaseMouseCapture(); return false; }
}

void gd::MouseProcess::releaseMouseCapture()
{
	if (isCaptured) { ReleaseCapture(); isCaptured = false; }
	aLPressed = false;
	aRPressed = false;
	aMPressed = false;
}