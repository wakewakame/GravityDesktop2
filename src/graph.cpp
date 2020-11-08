#pragma once

#include "graph.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

XMFLOAT4 gd::color() { return DirectX::XMFLOAT4{ 0.f, 0.f, 0.f, 1.f }; }
XMFLOAT4 gd::color(uint32_t rgb, uint8_t a)
{
	return DirectX::XMFLOAT4{
		(float)((rgb & 0xFF0000) >> 16),
		(float)((rgb & 0x00FF00) >> 8),
		(float)((rgb & 0x0000FF) >> 0),
		(float)a
	};
}
XMFLOAT4 gd::color(float r, float g, float b, float a)
{
	return DirectX::XMFLOAT4{ r, g, b, a };
}

gd::Graph::Graph() : m_d3dContext(nullptr)
{

}

gd::Graph::~Graph()
{

}

void gd::Graph::CreateDevice(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext)
{
    // ID3D11DeviceContextのポインタを持っておく
    // (ComPtrの代入演算子は参照カウントをインクリメントして、ポインタの所有権を共有する)
    m_d3dContext = d3dContext;

    // ID3D11Deviceを取得する
    ID3D11Device* m_d3dDevice;
    m_d3dContext->GetDevice(&m_d3dDevice);

    m_states = std::make_unique<CommonStates>(m_d3dDevice);

    m_effect = std::make_unique<BasicEffect>(m_d3dDevice);
    m_effect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;

    m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    DX::ThrowIfFailed(
        m_d3dDevice->CreateInputLayout(VertexPositionColor::InputElements,
            VertexPositionColor::InputElementCount,
            shaderByteCode, byteCodeLength,
            m_inputLayout.ReleaseAndGetAddressOf()));

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(m_d3dContext.Get());
}

void gd::Graph::CreateResources(const UINT backBufferWidth, const UINT backBufferHeight)
{
    m_proj = Matrix::CreateOrthographicOffCenter(
        0.f, float(backBufferWidth), float(backBufferHeight), 0.f, -1.f, 1.f
    );

    m_effect->SetProjection(m_proj);
}

void gd::Graph::OnDeviceLost()
{
    m_states.reset();
    m_effect.reset();
    m_batch.reset();
    m_inputLayout.Reset();
}

void gd::Graph::fill(Color c)
{
    fillBrush = c;
}

void gd::Graph::stroke(Color c)
{
    strokeBrush = c;
}

void gd::Graph::strokeWeight(float weight)
{
    strokeWeightBrush = weight;
}

int gd::Graph::beginShape(bool enableFill, bool enableStroke)
{
    if (isShapeBegan) return 1;

    isEnableFill = enableFill;
    isEnableStroke = enableStroke;

    if (isEnableFill) { fillIndices.clear(); fillVertices.clear(); }
    if (isEnableStroke) { strokeVertices.clear(); }

    isShapeBegan = true;
    return 0;
}

void gd::Graph::vertext(float x, float y)
{
    if (isEnableFill)
    {
        VertexPositionColor v{ Vector3{ x, y, 0.f }, fillBrush };
        fillVertices.push_back(v);
    }
}

int gd::Graph::endShape(bool loopStroke)
{
    if (!isShapeBegan) return 1;
    isShapeBegan = false;

    if ((!isEnableFill) && (!isEnableStroke)) return 0;

    m_d3dContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    m_d3dContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
    m_d3dContext->RSSetState(m_states->CullNone());

    m_effect->Apply(m_d3dContext.Get());

    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    m_batch->Begin();

    if (isEnableFill)
    {
        // TRIANGLESTRIPで塗りつぶしを行うので、頂点のインデックスを指定する必要がある
        // たとえば、頂点が10個の場合は次のようにインデックスを振る (左: 頂点の追加順, 右: 頂点のインデックス)
        // 0123456789 => 0918273645
        uint16_t size = fillVertices.size();
        fillIndices.reserve(fillVertices.size());
        for (uint16_t index = 0; index < size; index++)
        {
            //uint16_t strip_index = (size + ((index + 1) / 2) * (1 - (index & 1) * 2)) % size;
            uint16_t strip_index = (index & 0b1) ? (size - (index / 2) - 1) : (index / 2);
            fillIndices.push_back(strip_index);
        }

        m_batch->DrawIndexed(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
            fillIndices.data(), fillIndices.size(),
            fillVertices.data(), fillVertices.size()
        );
    }

    if (isEnableStroke)
    {
        m_batch->Draw(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ, 
            strokeVertices.data(), strokeVertices.size()
        );
    }

    m_batch->End();

    return 0;
}