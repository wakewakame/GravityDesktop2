/*

����̓v���O�����Ŏg�p�����{�I�Ȍ^���܂Ƃ߂����̂ł��B

*/

#pragma once

#include <string>

namespace gd
{
	/**
	 * ���s����\���̂���֐��Ăяo���̖߂�l�Ƃ��Ďg�p����^�ł��B
	 */
	template<typename T>
	struct Result
	{
		Result() {}
		Result(const T& value) : value(value), isNone(false) {}
		virtual ~Result() {};

		T value;                    // �֐��̖߂�l
		bool isNone = true;         // ���ʂ����݂��Ȃ����true�ɂȂ�
		bool isErr = false;         // �֐��Ăяo�������s�����true�ɂȂ�
		LPCWSTR description = L"";  // ���s�̐�����

		static Result Ok(const T& value) { return Result{ value }; }
		static Result None() { return Result{}; }
		static Result Err(LPCWSTR description)
		{
			Result result;
			result.isErr = true;
			result.description = description;
			return result;
		}
	};
}