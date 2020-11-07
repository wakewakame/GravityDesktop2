#pragma once

#include "pch.h"


namespace gd
{
	struct Mouse
	{
		POINT point  = {};       // ���݂̃t���[���̃}�E�X���W
		POINT point_ = {};       // 1�t���[���O�̃}�E�X���W��

		POINT lDragStart;        // ���݂̃t���[����aLDragStart

		POINT rDragStart;        // ���݂̃t���[����aRDragStart

		POINT mDragStart;        // ���݂̃t���[����aMDragStart

		bool lPressed  = false;  // ���݂̃t���[���̍��N���b�N
		bool lPressed_ = false;  // 1�t���[���O�̍��N���b�N

		bool rPressed  = false;  // ���݂̃t���[���̉E�N���b�N
		bool rPressed_ = false;  // 1�t���[���O�̉E�N���b�N

		bool mPressed  = false;  // ���݂̃t���[���̒����N���b�N
		bool mPressed_ = false;  // 1�t���[���O�̒����N���b�N

		bool lDouble = false;    // ���݂̃t���[���̍��_�u���N���b�N

		bool rDouble = false;    // ���݂̃t���[���̉E�_�u���N���b�N

		bool mDouble = false;    // ���݂̃t���[���̒����_�u���N���b�N
	};

	class MouseProcess
	{
	public:
		MouseProcess();
		virtual ~MouseProcess();
		void OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam);
		void nextFrame();
		Mouse getMouseStatus();

	private:
		POINT aMouse = {};       // �ŐV�̃}�E�X���W
		POINT bMouse = {};       // ���݂̃t���[���̃}�E�X���W
		POINT cMouse = {};       // 1�t���[���O�̃}�E�X���W��

		POINT aLDragStart;       // ���h���b�O���J�n�������_�ł̃}�E�X���W
		POINT bLDragStart;       // ���݂̃t���[����aLDragStart

		POINT aRDragStart;       // �E�h���b�O���J�n�������_�ł̃}�E�X���W
		POINT bRDragStart;       // ���݂̃t���[����aRDragStart

		POINT aMDragStart;       // �����h���b�O���J�n�������_�ł̃}�E�X���W
		POINT bMDragStart;       // ���݂̃t���[����aMDragStart

		bool aLPressed = false;  // �ŐV�̍��N���b�N
		bool bLPressed = false;  // ���݂̃t���[���̍��N���b�N
		bool cLPressed = false;  // 1�t���[���O�̍��N���b�N

		bool aRPressed = false;  // �ŐV�̉E�N���b�N
		bool bRPressed = false;  // ���݂̃t���[���̉E�N���b�N
		bool cRPressed = false;  // 1�t���[���O�̉E�N���b�N

		bool aMPressed = false;  // �ŐV�̒����N���b�N
		bool bMPressed = false;  // ���݂̃t���[���̒����N���b�N
		bool cMPressed = false;  // 1�t���[���O�̒����N���b�N
		
		bool aLDouble = false;   // �ŐV�̍��_�u���N���b�N
		bool bLDouble = false;   // ���݂̃t���[���̍��_�u���N���b�N

		bool aRDouble = false;   // �ŐV�̉E�_�u���N���b�N
		bool bRDouble = false;   // ���݂̃t���[���̉E�_�u���N���b�N

		bool aMDouble = false;   // �ŐV�̒����_�u���N���b�N
		bool bMDouble = false;   // ���݂̃t���[���̒����_�u���N���b�N
	};
}