#pragma once

#include "pch.h"

namespace gd
{
	using Vector3 = DirectX::SimpleMath::Vector3;

	struct XYLine
	{
		Vector3 start, end;
	};

	struct XYAxes
	{
		Vector3 vecX, vecY;

		XYAxes(float xX, float xY, float yX, float yY)
			: vecX( xX, xY, .0f ), vecY(yX, yY, .0f) {}
	};

	inline float getXYLength(const Vector3 vec)
	{
		return std::sqrt((vec.x * vec.x) + (vec.y * vec.y));
	}

	inline Vector3 scaleXY(const Vector3 vec, float scale)
	{
		return Vector3{ vec.x * scale, vec.y * scale, 0.f };
	}

	inline XYAxes getXYLocalAxes(const Vector3 vecX, float length = 1.f)
	{
		// 引数 vecX をX軸としたときの基底ベクトル X, Y を求める
		XYAxes localXY{ 0.f, 0.f, 0.f, 0.f };
		const float vecXLength = getXYLength(vecX);
		if (vecXLength == 0.f) return localXY;
		localXY.vecX = scaleXY(vecX, length / vecXLength);
		localXY.vecY.x = -localXY.vecX.y;
		localXY.vecY.y = localXY.vecX.x;
		return localXY;
	}

	/**
	 * 2直線の交点を求める
	 * @param line1, line2 2つの直線を指定する
	 * @return
	 *   uint8_t型に0が入っている場合 : 2直線は並行であり、解が存在しない
	 *   uint8_t型に1が入っている場合 : 直線の範囲外で交差し、その解がVector3型に格納される
	 *   uint8_t型に2が入っている場合 : 直線の範囲内で交差し、その解がVector3型に格納される
	 */
	inline std::pair<uint8_t, Vector3> getXYCrossPoint(XYLine line1, XYLine line2)
	{
		/*
		line1 と line2 の交点を求める。
		まず、0以上 1以下 の値をとる変数 float p, q; を考える。
		すると、line1, line2 の上にある点 L1, L2 は次のように定義できる。

		Vector3 L1 = line1.start + ((line1.end - line1.start) * p);
		Vector3 L2 = line2.start + ((line2.end - line2.start) * q);

		つまり、L1 == L2 となるような p, q を求めると交点が求まる。
		*/

		std::pair<uint8_t, Vector3> result{ 0, Vector3{.0f, .0f, .0f} };

		// 計算を簡略化するための一時変数
		const Vector3 A = line1.start;
		const Vector3 B = line1.end;
		const Vector3 C = line2.start;
		const Vector3 D = line2.end;
		const float a = B.x - A.x;
		const float b = D.y - C.y;
		const float c = B.y - A.y;
		const float d = D.x - C.x;
		const float denominator = (a * b) - (c * d);

		// 2直線が並行で解が存在しない場合
		if (denominator == 0.f) return result;

		// p, q を求める
		const float p = (((C.x * b) - (C.y * d)) - ((A.x * b) - (A.y * d))) / denominator;
		const float q = (((C.x * c) - (C.y * a)) - ((A.x * c) - (A.y * a))) / denominator;

		// 交点を求める
		result.second.x = (A.x * (1.f - p)) + (B.x * p);
		result.second.y = (A.y * (1.f - p)) + (B.y * p);

		// 交点が直線の範囲外である場合
		if ((0 > p) || (1 < p) || (0 > q) || (1 < q))
		{
			result.first = 1;
			return result;
		}

		// 交点が直線の範囲内である場合
		result.first = 2;
		return result;
	}
}