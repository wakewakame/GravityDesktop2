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
		 * vertext�֐��œh��Ԃ��F�̎w������܂��B
		 * @param rgb �F ( 0x000000 - 0xFFFFFF )
		 * @param a �����x ( 0 - 255 )
		 */
		void fill(uint32_t rgb, uint8_t a = 255);

		/**
		 * vertext�֐��ŕ`�悷��F�̎w������܂��B
		 * @param r, g, b �F ( 0.0f - 1.0f )
		 * @param a �����x ( 0.0f - 1.0f )
		 */
		void fill(float r, float g, float b, float a = 1.f);

		/**
		 * vertext�֐��ł̐��̐F�̎w������܂��B
		 * @param rgb �F ( 0x000000 - 0xFFFFFF )
		 * @param a �����x ( 0 - 255 )
		 */
		void stroke(uint32_t rgb, uint8_t a = 255);

		/**
		 * vertext�֐��ł̐��̐F�̎w������܂��B
		 * @param r, g, b �F ( 0.0f - 1.0f )
		 * @param a �����x ( 0.0f - 1.0f )
		 */
		void stroke(float r, float g, float b, float a = 1.f);

		/**
		 * �}�`�̍쐬���J�n����
		 * ���̊֐����Ăяo�������vertext�֐��Œ��_��ǉ�����
		 * ���_��ǉ����I������endShape�֐��ŕ`����s��
		 * @param enableFill fill�֐��Ŏw�肵���F�ɓh��Ԃ�
		 * @param strokeWeight stroke�֐��Ŏw�肵���F�̐��̑������w�肷��
		 * @return �J�n�ɐ��������1���Ԃ�A���s�����0���Ԃ�
		 */
		int beginShape(bool enableFill = true, float strokeWeight = 1.0f);

		/**
		 * �}�`�ɒ��_��ǉ�����
		 * @param x, y �ǉ����钸�_�̉�ʏ�̍��W
		 */
		void vertext(float x, float y);

		/**
		 * �}�`�̍쐬���I�����A��ʏ�ɐ}�`��`�悷��
		 * @param loopStroke �}�`�̘g�����n�[�ƏI�[�ŕ���
		 * @return �`��ɐ��������1���Ԃ�A���s�����0���Ԃ�
		 */
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
		std::vector<uint16_t> fillIndices;
		std::vector<DirectX::VertexPositionColor> fillVertices;
		std::vector<DirectX::VertexPositionColor> strokeVertices;
		DirectX::XMFLOAT4 fillBrush = {0.f, 0.f, 0.f, 1.f};
		DirectX::XMFLOAT4 strokeBrush = {0.f, 0.f, 0.f, 1.f};
		float strokeWeightBrush = 1.0f;
	};
}