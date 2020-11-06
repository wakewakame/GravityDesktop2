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

void gd::Graph::CreateDevice(ID3D11Device1* const m_d3dDevice, ID3D11DeviceContext1* const m_d3dContext)
{
    this->m_d3dContext = m_d3dContext;

    m_world = Matrix::Identity;

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

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(this->m_d3dContext);
}

void gd::Graph::CreateResources(const UINT backBufferWidth, const UINT backBufferHeight)
{
    m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
        Vector3::Zero, Vector3::UnitY);
    m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
        float(backBufferWidth) / float(backBufferHeight), 0.1f, 10.f);

    m_effect->SetView(m_view);
    m_effect->SetProjection(m_proj);
}

void gd::Graph::OnDeviceLost()
{
    m_states.reset();
    m_effect.reset();
    m_batch.reset();
    m_inputLayout.Reset();
}

void gd::Graph::Line()
{
    m_d3dContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    m_d3dContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
    m_d3dContext->RSSetState(m_states->CullNone());

    m_effect->SetWorld(m_world);

    m_effect->Apply(m_d3dContext);

    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    m_batch->Begin();

    Vector3 xaxis(2.f, 0.f, 0.f);
    Vector3 yaxis(0.f, 0.f, 2.f);
    Vector3 origin = Vector3::Zero;

    size_t divisions = 20;

    for (size_t i = 0; i <= divisions; ++i)
    {
        float fPercent = float(i) / float(divisions);
        fPercent = (fPercent * 2.0f) - 1.0f;

        Vector3 scale = xaxis * fPercent + origin;

        VertexPositionColor v1(scale - yaxis, Colors::White);
        VertexPositionColor v2(scale + yaxis, Colors::White);
        m_batch->DrawLine(v1, v2);
    }

    for (size_t i = 0; i <= divisions; i++)
    {
        float fPercent = float(i) / float(divisions);
        fPercent = (fPercent * 2.0f) - 1.0f;

        Vector3 scale = yaxis * fPercent + origin;

        VertexPositionColor v1(scale - xaxis, Colors::White);
        VertexPositionColor v2(scale + xaxis, Colors::White);
        m_batch->DrawLine(v1, v2);
    }

    m_batch->End();
}