#pragma once

#include "pch.h"

namespace gd
{
	using Color = DirectX::XMFLOAT4;
	DirectX::XMFLOAT4 color();
	DirectX::XMFLOAT4 color(uint32_t rgb, uint8_t a = 255);
	DirectX::XMFLOAT4 color(float r, float g, float b, float a = 1.f);

	class Graph
	{
	public:
		Graph();

		virtual ~Graph();

		void CreateDevice(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext);

		void CreateResources(const UINT backBufferWidth, const UINT backBufferHeight);

		void OnDeviceLost();

		void fill(Color c);
		void stroke(Color c);
		void strokeWeight(float weight = 1.0f);

		int beginShape(bool enableFill = true, bool enableStroke = true);
		void vertext(float x, float y);
		int endShape(bool loopStroke = false);

	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
		std::unique_ptr<DirectX::CommonStates> m_states;
		std::unique_ptr<DirectX::BasicEffect> m_effect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		DirectX::SimpleMath::Matrix m_proj;

		bool isShapeBegan = false;
		bool isEnableFill = false;
		bool isEnableStroke = false;
		std::vector<uint16_t> fillIndices;
		std::vector<DirectX::VertexPositionColor> fillVertices;
		std::vector<DirectX::VertexPositionColor> strokeVertices;
		Color fillBrush = {};
		Color strokeBrush = {};
		float strokeWeightBrush = 1.0f;
	};
}