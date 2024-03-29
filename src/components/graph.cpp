#pragma once

#include "components/graph.h"

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
    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    m_d3dContext->GetDevice(&m_d3dDevice);

    m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());

    m_effect = std::make_unique<BasicEffect>(m_d3dDevice.Get());
    m_effect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;

    m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    DX::ThrowIfFailed(
        m_d3dDevice->CreateInputLayout(VertexPositionColor::InputElements,
            VertexPositionColor::InputElementCount,
            shaderByteCode, byteCodeLength,
            m_inputLayout.ReleaseAndGetAddressOf()));

    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(m_d3dContext.Get());
    m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());

    setRenderMode(BlendMode::AlphaBlend1, DepthMode::DepthNone, RasterizerMode::CullNone);
}

void gd::Graph::CreateResources(const UINT backBufferWidth, const UINT backBufferHeight)
{
    // 画面サイズに合わせてプロジェクション行列を計算し直す
    m_proj = Matrix::CreateOrthographicOffCenter(
        0.f, float(backBufferWidth), float(backBufferHeight), 0.f, -1.f, 1.f
    );
    m_effect->SetProjection(m_proj);
}

void gd::Graph::OnDeviceLost()
{
    m_states.reset();
    m_effect.reset();
    m_primitiveBatch.reset();
    m_spriteBatch.reset();
    m_inputLayout.Reset();
}

void gd::Graph::fill(uint32_t rgb, uint8_t a)
{
    fill(
        ((float)((rgb & 0xFF0000) >> 16)) / 255.f,
        ((float)((rgb & 0x00FF00) >> 8)) / 255.f,
        ((float)((rgb & 0x0000FF) >> 0)) / 255.f,
        ((float)a) / 255.f
    );
}

void gd::Graph::fill(float r, float g, float b, float a)
{
    fillBrush = XMFLOAT4{ r, g, b, a };
}

void gd::Graph::stroke(uint32_t rgb, uint8_t a)
{
    stroke(
        ((float)((rgb & 0xFF0000) >> 16)) / 255.f,
        ((float)((rgb & 0x00FF00) >> 8)) / 255.f,
        ((float)((rgb & 0x0000FF) >> 0)) / 255.f,
        ((float)a) / 255.f
    );
}

void gd::Graph::stroke(float r, float g, float b, float a)
{
    strokeBrush = XMFLOAT4{ r, g, b, a };
}

int gd::Graph::beginShape(bool enableFill, float strokeWeight)
{
    if (isShapeBegan) return 1;

    isEnableFill = enableFill;
    strokeWeightBrush = strokeWeight;

    // 以前の頂点の削除
    if (isEnableFill) { fillIndices.clear(); fillVertices.clear(); }
    if (strokeWeightBrush > 0.f) { strokeVertices.clear(); }

    isShapeBegan = true;
    return 0;
}

void gd::Graph::vertex(float x, float y)
{
    // 塗りつぶしの頂点の追加
    if (isEnableFill)
    {
        VertexPositionColor v{ Vector3{ x, y, 0.f }, fillBrush };
        fillVertices.push_back(v);
    }

    // 枠線の頂点の追加
    if (strokeWeightBrush > 0.f)
    {
        VertexPositionColor v{ Vector3{ x, y, 0.f }, strokeBrush };
        strokeVertices.push_back(v);
    }
}

int gd::Graph::endShape(bool loopStroke)
{
    if (!isShapeBegan) return 1;
    isShapeBegan = false;

    if ((!isEnableFill) && (strokeWeightBrush <= 0.f)) return 0;

    m_effect->Apply(m_d3dContext.Get());

    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    m_primitiveBatch->Begin();

    // 頂点数が2以下だと描画しないのと同じであるため、頂点数が3以上で描画
    if (isEnableFill && fillVertices.size() >= 3)
    {
        // TRIANGLESTRIPで塗りつぶしを行うので、頂点のインデックスを指定する必要がある
        // たとえば、頂点が10個の場合は次のようにインデックスを振る (左: 頂点の追加順, 右: 頂点のインデックス)
        // 0123456789 => 0918273645
        uint16_t size = fillVertices.size();
        fillIndices.reserve(fillVertices.size());
        for (uint16_t index = 0; index < size; index++)
        {
            uint16_t strip_index = (index & 0b1) ? (size - (index / 2) - 1) : (index / 2);
            fillIndices.push_back(strip_index);
        }

        m_primitiveBatch->DrawIndexed(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
            fillIndices.data(), fillIndices.size(),
            fillVertices.data(), fillVertices.size()
        );
    }

    // 頂点数が1以下だと描画しないのと同じであるため、頂点数が2以上で描画
    if ((strokeWeightBrush > 0.f) && (strokeVertices.size() >= 2))
    {
        // strokeVerticesに太さを持たせた頂点配列を計算してtmpStrokeVerticesに格納する
        std::vector<DirectX::VertexPositionColor> tmpStrokeVertices;
        tmpStrokeVertices.reserve(
            (strokeVertices.size() * 4) + (loopStroke ? 2 : -4)
        );

        // 必要な要素数をあらかじめ確保する
        if (loopStroke) { strokeVertices.push_back(strokeVertices[0]); }

        for (size_t index = 0; index < strokeVertices.size() - 1; index++)
        {
            // 直線の先端と終端の点
            const Vector3 p1 = strokeVertices[index].position;
            const Vector3 p2 = strokeVertices[index + 1].position;

            // 直線の法線の単位ベクトル
            Vector3 normal = p2 - p1;
            normal.Normalize();
            std::swap(normal.x, normal.y);
            normal.x *= -1.f;

            // 生成される直線の先端の上側
            Vector3 p1_up = p1 + (normal * strokeWeightBrush);
            tmpStrokeVertices.push_back(
                VertexPositionColor{ p1_up, strokeVertices[index].color }
            );

            // 生成される直線の先端の下側
            Vector3 p1_dw = p1 - (normal * strokeWeightBrush);
            tmpStrokeVertices.push_back(
                VertexPositionColor{ p1_dw, strokeVertices[index].color }
            );

            // 生成される直線の終端の上側
            Vector3 p2_up = p2 + (normal * strokeWeightBrush);
            tmpStrokeVertices.push_back(
                VertexPositionColor{ p2_up, strokeVertices[index + 1].color }
            );

            // 生成される直線の終端の上側
            Vector3 p2_dw = p2 - (normal * strokeWeightBrush);
            tmpStrokeVertices.push_back(
                VertexPositionColor{ p2_dw, strokeVertices[index + 1].color }
            );
        }

        if (loopStroke) {
            // 線の終端を始端と繋げる
            tmpStrokeVertices.push_back(tmpStrokeVertices[0]);
            tmpStrokeVertices.push_back(tmpStrokeVertices[1]);
        }

        m_primitiveBatch->Draw(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
            tmpStrokeVertices.data(), tmpStrokeVertices.size()
        );
    }

    m_primitiveBatch->End();

    return 0;
}

void gd::Graph::setRenderMode(BlendMode blend, DepthMode depth, RasterizerMode rasterizer)
{
    blendState =
        (BlendMode::Opaque      == blend) ? m_states->Opaque()     :  // 上書き
        (BlendMode::Additive    == blend) ? m_states->Additive()   :  // 加算合成
        (BlendMode::AlphaBlend2 == blend) ? m_states->AlphaBlend() :  // アルファブレンド2
        m_states->NonPremultiplied();                                 // アルファブレンド1

    depthState =
        (DepthMode::DepthDefault == depth) ? m_states->DepthDefault() :  // Zバッファを使用する
        m_states->DepthNone();                                           // Zバッファを使用しない

    rasterozerState =
        (RasterizerMode::CullClockwise == rasterizer) ? m_states->CullClockwise() :
        (RasterizerMode::CullCounterClockwise == rasterizer) ? m_states->CullCounterClockwise() :
        (RasterizerMode::Wireframe == rasterizer) ? m_states->Wireframe() :
        m_states->CullNone();

    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_d3dContext->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);
    m_d3dContext->OMSetDepthStencilState(depthState, 0);
    m_d3dContext->RSSetState(rasterozerState);
}

int gd::Graph::line(float x1, float y1, float x2, float y2, float weight)
{
    if (isShapeBegan) return 1;
    bool eStrock = ((strokeBrush.w > 0.f) && (weight > 0.f));
    beginShape(false, eStrock ? weight : 0.f);
    vertex(x1, y1);
    vertex(x2, y2);
    endShape(false);
    return 0;
}

int gd::Graph::rect(float x1, float y1, float x2, float y2, float weight)
{
    if (isShapeBegan) return 1;
    bool eFill = (fillBrush.w > 0.f);
    bool eStrock = ((strokeBrush.w > 0.f) && (weight > 0.f));
    beginShape(eFill, eStrock ? weight : 0.f);
    vertex(x1, y1);
    vertex(x2, y1);
    vertex(x2, y2);
    vertex(x1, y2);
    endShape(true);
    return 0;
}

int gd::Graph::ellipse(float x, float y, float r, float weight, uint8_t div)
{
    if (isShapeBegan) return 1;
    if (div <= 1) return 0;
    bool eFill = (fillBrush.w > 0.f);
    bool eStrock = ((strokeBrush.w > 0.f) && (weight > 0.f));
    beginShape(eFill, eStrock ? weight : 0.f);
    for (int i = 0; i < div; i++)
    {
        float p = (float)i / (float)div;
        float theta = XM_2PI * p;
        float x_ = x + std::cos(theta) * r;
        float y_ = y + std::sin(theta) * r;
        vertex(x_, y_);
    }
    endShape(true);
    return 0;
}

int gd::Graph::image(
    const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture,
    const RECT area,
    const XMFLOAT2 position,
    const XMFLOAT2 origin,
    const float angle
)
{
    m_spriteBatch->Begin(SpriteSortMode_Deferred, blendState, nullptr, depthState, rasterozerState, [=] {
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        m_d3dContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
    });
    m_spriteBatch->Draw(
        texture.Get(),
        position,                // 描画の中心座標
        &area,                   // 描画範囲
        DirectX::Colors::White,  // スプライトを着色する色
        angle,                   // 回転度数
        origin                   // 画像の原点
    );
    m_spriteBatch->End();
    return 0;
}