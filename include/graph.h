#pragma once

#include "pch.h"

namespace gd
{
	class Graph
	{
	public:
		Graph();

		virtual ~Graph();

		void CreateDevice(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext);

		void CreateResources(const UINT backBufferWidth, const UINT backBufferHeight);

		void OnDeviceLost();

		void Line();

	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
		std::unique_ptr<DirectX::CommonStates> m_states;
		std::unique_ptr<DirectX::BasicEffect> m_effect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		DirectX::SimpleMath::Matrix m_proj;
	};
}