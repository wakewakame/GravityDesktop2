/*

����̓}�E�X�C�x���g�𒊏ۉ����邽�߂̃v���O�����ł��B

*/

#pragma once

#include "utils/pch.h"


namespace gd
{
	struct Mouse
	{
		POINTS point  = {};       // ���݂̃t���[���̃}�E�X���W
		POINTS point_ = {};       // 1�t���[���O�̃}�E�X���W��

		POINTS lDragStart;        // ���݂̃t���[����aLDragStart

		POINTS rDragStart;        // ���݂̃t���[����aRDragStart

		POINTS mDragStart;        // ���݂̃t���[����aMDragStart

		bool lPressed  = false;  // ���݂̃t���[���̍��N���b�N
		bool lPressed_ = false;  // 1�t���[���O�̍��N���b�N

		bool rPressed  = false;  // ���݂̃t���[���̉E�N���b�N
		bool rPressed_ = false;  // 1�t���[���O�̉E�N���b�N

		bool mPressed  = false;  // ���݂̃t���[���̒����N���b�N
		bool mPressed_ = false;  // 1�t���[���O�̒����N���b�N

		bool lDouble = false;    // ���݂̃t���[���̍��_�u���N���b�N

		bool rDouble = false;    // ���݂̃t���[���̉E�_�u���N���b�N

		bool mDouble = false;    // ���݂̃t���[���̒����_�u���N���b�N

		int zDelta = 0;          // ���݂̃t���[���̃z�C�[���̉�]��

		inline bool lClick()   const { return lPressed && (!lPressed_); }
		inline bool lRelease() const { return (!lPressed) && lPressed_; }

		inline bool rClick()   const { return lPressed && (!lPressed_); }
		inline bool rRelease() const { return (!lPressed) && lPressed_; }

		inline bool mClick()   const { return lPressed && (!lPressed_); }
		inline bool mRelease() const { return (!lPressed) && lPressed_; }
	};

	class MouseProcess
	{
	public:
		MouseProcess();
		virtual ~MouseProcess();
		void OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		/**
		 * ���݂̃t���[���ł̃}�E�X�̏�Ԃ��X�V����
		 */
		void nextFrame();

		/**
		 * ���݂̃t���[���ɂ�����}�E�X�̏�Ԃ��擾����
		 * @return ���݂̃t���[���ɂ�����}�E�X�̏��
		 */
		Mouse getMouseStatus() const;

	private:
		POINTS aMouse = {};       // �ŐV�̃}�E�X���W
		POINTS bMouse = {};       // ���݂̃t���[���̃}�E�X���W
		POINTS cMouse = {};       // 1�t���[���O�̃}�E�X���W��

		POINTS aLDragStart;       // ���h���b�O���J�n�������_�ł̃}�E�X���W
		POINTS bLDragStart;       // ���݂̃t���[����aLDragStart

		POINTS aRDragStart;       // �E�h���b�O���J�n�������_�ł̃}�E�X���W
		POINTS bRDragStart;       // ���݂̃t���[����aRDragStart

		POINTS aMDragStart;       // �����h���b�O���J�n�������_�ł̃}�E�X���W
		POINTS bMDragStart;       // ���݂̃t���[����aMDragStart

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

		int aZDelta = 0;         // �ŐV�̃t���[���̉�]��
		int bZDelta = 0;         // ���݂̃t���[���̉�]��

		bool isCaptured = false;

		/**
		 * �}�E�X�̃L���v�`�����J�n����
		 * ���ɃL���v�`�����J�n����Ă���ꍇ��releaseMouseCapture���Ăяo���ăL���v�`�����I������
		 * @return ���������true���Ԃ�A����ȊO(���ɃL���v�`�����J�n����Ă���Ȃ�)�ł�false���Ԃ�
		 */
		bool setMouseCapture(HWND hWnd);

		/**
		 * �}�E�X�̃L���v�`�����~����
		 * ���̍ۂ�aLPressed�AaRPressed�AaMPressed��false��ݒ肷��
		 */
		void releaseMouseCapture();
	};
}