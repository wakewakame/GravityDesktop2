/*

これはプログラムで使用する基本的な型をまとめたものです。

*/

#pragma once

#include <string>

namespace gd
{
	/**
	 * 失敗する可能性のある関数呼び出しの戻り値として使用する型です。
	 */
	template<typename T>
	struct Result
	{
		Result() {}
		Result(const T& value) : value(value), isNone(false) {}
		virtual ~Result() {};

		T value;                    // 関数の戻り値
		bool isNone = true;         // 結果が存在しなければtrueになる
		bool isErr = false;         // 関数呼び出しが失敗するとtrueになる
		LPCWSTR description = L"";  // 失敗の説明文

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