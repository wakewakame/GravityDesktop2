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

		/**
		 * vertext関数で塗りつぶす色の指定をします。
		 * @param rgb 色 ( 0x000000 - 0xFFFFFF )
		 * @param a 透明度 ( 0 - 255 )
		 */
		void fill(uint32_t rgb, uint8_t a = 255);

		/**
		 * vertext関数で描画する色の指定をします。
		 * @param r, g, b 色 ( 0.0f - 1.0f )
		 * @param a 透明度 ( 0.0f - 1.0f )
		 */
		void fill(float r, float g, float b, float a = 1.f);

		/**
		 * vertext関数での線の色の指定をします。
		 * @param rgb 色 ( 0x000000 - 0xFFFFFF )
		 * @param a 透明度 ( 0 - 255 )
		 */
		void stroke(uint32_t rgb, uint8_t a = 255);

		/**
		 * vertext関数での線の色の指定をします。
		 * @param r, g, b 色 ( 0.0f - 1.0f )
		 * @param a 透明度 ( 0.0f - 1.0f )
		 */
		void stroke(float r, float g, float b, float a = 1.f);

		/**
		 * 図形の作成を開始する
		 * この関数を呼び出した後はvertext関数で頂点を追加する
		 * 頂点を追加し終えたらendShape関数で描画を行う
		 * @param enableFill fill関数で指定した色に塗りつぶす
		 * @param strokeWeight stroke関数で指定した色の線の太さを指定する
		 * @return 開始に成功すると1が返り、失敗すると0が返る
		 */
		int beginShape(bool enableFill = true, float strokeWeight = 1.0f);

		/**
		 * 図形に頂点を追加する
		 * @param x, y 追加する頂点の画面上の座標
		 */
		void vertex(float x, float y);

		/**
		 * 図形の作成を終了し、画面上に図形を描画する
		 * @param loopStroke 図形の枠線を始端と終端で閉じる
		 * @return 描画に成功すると1が返り、失敗すると0が返る
		 */
		int endShape(bool loopStroke = false);

		int line(float x1, float y1, float x2, float y2, float weight = 1.f);

		int rect(float x1, float y1, float x2, float y2, float weight = 1.f);

		int ellipse(float x, float y, float r, float weight = 1.f, uint8_t div = 32);

	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
		std::unique_ptr<DirectX::CommonStates> m_states;
		std::unique_ptr<DirectX::BasicEffect> m_effect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		DirectX::SimpleMath::Matrix m_proj;

		bool isShapeBegan = false;
		bool isEnableFill = false;
		std::vector<uint16_t> fillIndices;
		std::vector<DirectX::VertexPositionColor> fillVertices;
		std::vector<DirectX::VertexPositionColor> strokeVertices;
		DirectX::XMFLOAT4 fillBrush = {0.f, 0.f, 0.f, 1.f};
		DirectX::XMFLOAT4 strokeBrush = {0.f, 0.f, 0.f, 1.f};
		float strokeWeightBrush = 1.0f;
	};
}