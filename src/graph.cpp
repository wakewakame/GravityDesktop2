#pragma once

#include "graph.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

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

void gd::Graph::Line(const POINT p1, const POINT p2, const Color color)
{
    m_d3dContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    m_d3dContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
    m_d3dContext->RSSetState(m_states->CullNone());

    m_effect->Apply(m_d3dContext.Get());

    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    m_batch->Begin();

    Vector3 a_vec{ (float)p1.x, (float)p1.y, 0.f };
    Vector3 b_vec{ (float)p2.x, (float)p2.y, 0.f };

    m_batch->DrawLine(
        VertexPositionColor(a_vec, color.toXMFLOAT4()),
        VertexPositionColor(b_vec, color.toXMFLOAT4())
    );

    m_batch->End();
}