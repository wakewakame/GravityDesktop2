#pragma once

#include "pch.h"

namespace gd
{
	struct Color
	{
		uint8_t r, g, b, a;

		Color() : r(0), g(0), b(0), a(255) {}

		Color(uint32_t rgb, uint8_t a = 255)
			: r((rgb & 0xFF0000) >> 16), g((rgb & 0x00FF00) >> 8), b((rgb & 0x0000FF) >> 0), a(a) {}

		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}

		virtual ~Color() {}

		DirectX::XMFLOAT4 toXMFLOAT4() const {
			return DirectX::XMFLOAT4{ (float)r, (float)g, (float)b, (float)a };
		}
	};

	class Graph
	{
	public:
		Graph();

		virtual ~Graph();

		void CreateDevice(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext);

		void CreateResources(const UINT backBufferWidth, const UINT backBufferHeight);

		void OnDeviceLost();

		void Line(const POINTS p1, const POINTS p2, const Color color = Color());

	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
		std::unique_ptr<DirectX::CommonStates> m_states;
		std::unique_ptr<DirectX::BasicEffect> m_effect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		DirectX::SimpleMath::Matrix m_proj;
	};
}