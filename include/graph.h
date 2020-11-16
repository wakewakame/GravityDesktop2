#pragma once

#include "pch.h"

namespace gd
{
	enum class BlendMode : uint8_t
	{
		Opaque,      // �s���� (���l�𖳎�����)
		AlphaBlend,  // �A���t�@�u�����h
		Additive,    // ���Z����
	};

	enum class DepthMode : uint8_t
	{
		DepthNone,     // Z�o�b�t�@���g�p���Ȃ� (�`�揇�����\�[�g���Ȃ�)
		DepthDefault,  // Z�o�b�t�@���g�p���� (�`�揇�����J�����̋߂��Ń\�[�g����)
	};

	enum class RasterizerMode : uint8_t
	{
		CullNone,              // �|���S���𗼖ʕ`�悷��
		CullClockwise,         // �|���S����Жʕ`�悷��
		CullCounterClockwise,  // �|���S����Жʕ`�悷�� (CullClockwise�̋t�̖�)
		Wireframe              // �|���S�������C���[�t���[���ŕ`�悷��
	};

	class Graph
	{
	public:
		Graph();

		virtual ~Graph();

		void CreateDevice(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext);

		void CreateResources(const UINT backBufferWidth, const UINT backBufferHeight);

		void OnDeviceLost();

		inline Microsoft::WRL::ComPtr<ID3D11DeviceContext>& getDeviceContext() { return m_d3dContext; }

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
		void vertex(float x, float y);

		/**
		 * �}�`�̍쐬���I�����A��ʏ�ɐ}�`��`�悷��
		 * @param loopStroke �}�`�̘g�����n�[�ƏI�[�ŕ���
		 * @return �`��ɐ��������1���Ԃ�A���s�����0���Ԃ�
		 */
		int endShape(bool loopStroke = false);
		
		/**
		 * �`�惂�[�h��ύX���܂�
		 * @param blend �������[�h�̎w��
		 * @param depth �`�揇���̃\�[�g���@�̎w��
		 * @param rasterizer �ʂ̕`����@�̎w��
		 */
		void setRenderMode(
			BlendMode blend = BlendMode::AlphaBlend,
			DepthMode depth = DepthMode::DepthNone,
			RasterizerMode rasterizer = RasterizerMode::CullNone
		);

		int line(float x1, float y1, float x2, float y2, float weight = 1.f);

		int rect(float x1, float y1, float x2, float y2, float weight = 1.f);

		int ellipse(float x, float y, float r, float weight = 1.f, uint8_t div = 32);

		int image(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& const texture);

	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
		std::unique_ptr<DirectX::CommonStates> m_states;
		std::unique_ptr<DirectX::BasicEffect> m_effect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;
		std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
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