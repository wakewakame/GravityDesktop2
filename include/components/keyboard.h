/*

����̓L�[�C�x���g�𒊏ۉ����邽�߂̃v���O�����ł��B

*/

#pragma once

#include "utils/pch.h"


namespace gd
{
	struct Keyboard
	{
		std::set<uint8_t> keys;   // ���݂̃t���[���̃L�[����
		std::set<uint8_t> keys_;  // 1�t���[���O�̃t���[���̃L�[����
	};

	class KeyboardProcess
	{
	public:
		KeyboardProcess();
		virtual ~KeyboardProcess();
		void OnWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		/**
		 * ���݂̃t���[���ł̃L�[�{�[�h�̏�Ԃ��X�V����
		 */
		void nextFrame();

		/**
		 * ���݂̃t���[���ɂ�����L�[�{�[�h�̏�Ԃ��擾����
		 * @return ���݂̃t���[���ɂ�����L�[�{�[�h�̏��
		 */
		Keyboard getKeyboardStatus() const;

	private:
		std::set<uint8_t> aKeys;  // �ŐV�̃L�[����
		std::set<uint8_t> bKeys;  // ���݂̃t���[���̃L�[����
		std::set<uint8_t> cKeys;  // 1�t���[���O�̃t���[���̃L�[����
	};
}