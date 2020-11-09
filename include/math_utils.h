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
		// ���� vecX ��X���Ƃ����Ƃ��̊��x�N�g�� X, Y �����߂�
		XYAxes localXY{ 0.f, 0.f, 0.f, 0.f };
		const float vecXLength = getXYLength(vecX);
		if (vecXLength == 0.f) return localXY;
		localXY.vecX = scaleXY(vecX, length / vecXLength);
		localXY.vecY.x = -localXY.vecX.y;
		localXY.vecY.y = localXY.vecX.x;
		return localXY;
	}

	/**
	 * 2�����̌�_�����߂�
	 * @param line1, line2 2�̒������w�肷��
	 * @return
	 *   uint8_t�^��0�������Ă���ꍇ : 2�����͕��s�ł���A�������݂��Ȃ�
	 *   uint8_t�^��1�������Ă���ꍇ : �����͈̔͊O�Ō������A���̉���Vector3�^�Ɋi�[�����
	 *   uint8_t�^��2�������Ă���ꍇ : �����͈͓̔��Ō������A���̉���Vector3�^�Ɋi�[�����
	 */
	inline std::pair<uint8_t, Vector3> getXYCrossPoint(XYLine line1, XYLine line2)
	{
		/*
		line1 �� line2 �̌�_�����߂�B
		�܂��A0�ȏ� 1�ȉ� �̒l���Ƃ�ϐ� float p, q; ���l����B
		����ƁAline1, line2 �̏�ɂ���_ L1, L2 �͎��̂悤�ɒ�`�ł���B

		Vector3 L1 = line1.start + ((line1.end - line1.start) * p);
		Vector3 L2 = line2.start + ((line2.end - line2.start) * q);

		�܂�AL1 == L2 �ƂȂ�悤�� p, q �����߂�ƌ�_�����܂�B
		*/

		std::pair<uint8_t, Vector3> result{ 0, Vector3{.0f, .0f, .0f} };

		// �v�Z���ȗ������邽�߂̈ꎞ�ϐ�
		const Vector3 A = line1.start;
		const Vector3 B = line1.end;
		const Vector3 C = line2.start;
		const Vector3 D = line2.end;
		const float a = B.x - A.x;
		const float b = D.y - C.y;
		const float c = B.y - A.y;
		const float d = D.x - C.x;
		const float denominator = (a * b) - (c * d);

		// 2���������s�ŉ������݂��Ȃ��ꍇ
		if (denominator == 0.f) return result;

		// p, q �����߂�
		const float p = (((C.x * b) - (C.y * d)) - ((A.x * b) - (A.y * d))) / denominator;
		const float q = (((C.x * c) - (C.y * a)) - ((A.x * c) - (A.y * a))) / denominator;

		// ��_�����߂�
		result.second.x = (A.x * (1.f - p)) + (B.x * p);
		result.second.y = (A.y * (1.f - p)) + (B.y * p);

		// ��_�������͈̔͊O�ł���ꍇ
		if ((0 > p) || (1 < p) || (0 > q) || (1 < q))
		{
			result.first = 1;
			return result;
		}

		// ��_�������͈͓̔��ł���ꍇ
		result.first = 2;
		return result;
	}
}