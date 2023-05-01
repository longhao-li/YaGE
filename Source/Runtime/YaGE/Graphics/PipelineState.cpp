#include "PipelineState.h"
#include "../Core/Exception.h"
#include "RenderDevice.h"

using namespace YaGE;

YaGE::PipelineState::~PipelineState() noexcept {}

YaGE::GraphicsPipelineState::GraphicsPipelineState(YaGE::RootSignature                      &rootSignature,
                                                   const D3D12_GRAPHICS_PIPELINE_STATE_DESC &desc)
    : PipelineState(rootSignature),
      renderTargetCount(desc.NumRenderTargets),
      renderTargetFormats{
          desc.RTVFormats[0], desc.RTVFormats[1], desc.RTVFormats[2], desc.RTVFormats[3],
          desc.RTVFormats[4], desc.RTVFormats[5], desc.RTVFormats[6], desc.RTVFormats[7],
      },
      depthStencilFormat(desc.DSVFormat),
      primitiveTopology(desc.PrimitiveTopologyType),
      sampleCount(desc.SampleDesc.Count) {
    HRESULT hr = S_OK;

    // Set root signature.
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc(desc);
    pipelineDesc.pRootSignature = rootSignature.D3D12RootSignature();

    RenderDevice &device = RenderDevice::Singleton();
    hr = device.Device()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(pipelineState.GetAddressOf()));
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to create graphics pipeline state.");
}

YaGE::GraphicsPipelineState::GraphicsPipelineState(GraphicsPipelineState &&other) noexcept
    : PipelineState(std::move(other)),
      renderTargetCount(other.renderTargetCount),
      renderTargetFormats{
          other.renderTargetFormats[0], other.renderTargetFormats[1], other.renderTargetFormats[2],
          other.renderTargetFormats[3], other.renderTargetFormats[4], other.renderTargetFormats[5],
          other.renderTargetFormats[6], other.renderTargetFormats[7],
      },
      depthStencilFormat(other.depthStencilFormat),
      primitiveTopology(other.primitiveTopology),
      sampleCount(other.sampleCount) {
    other.renderTargetCount      = 0;
    other.renderTargetFormats[0] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[1] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[2] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[3] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[4] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[5] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[6] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[7] = DXGI_FORMAT_UNKNOWN;
    other.depthStencilFormat     = DXGI_FORMAT_UNKNOWN;
    other.primitiveTopology      = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    other.sampleCount            = 0;
}

auto YaGE::GraphicsPipelineState::operator=(GraphicsPipelineState &&other) noexcept -> GraphicsPipelineState & {
    PipelineState::operator=(std::move(other));

    renderTargetCount      = other.renderTargetCount;
    renderTargetFormats[0] = other.renderTargetFormats[0];
    renderTargetFormats[1] = other.renderTargetFormats[1];
    renderTargetFormats[2] = other.renderTargetFormats[2];
    renderTargetFormats[3] = other.renderTargetFormats[3];
    renderTargetFormats[4] = other.renderTargetFormats[4];
    renderTargetFormats[5] = other.renderTargetFormats[5];
    renderTargetFormats[6] = other.renderTargetFormats[6];
    renderTargetFormats[7] = other.renderTargetFormats[7];
    depthStencilFormat     = other.depthStencilFormat;
    primitiveTopology      = other.primitiveTopology;
    sampleCount            = other.sampleCount;

    other.renderTargetCount      = 0;
    other.renderTargetFormats[0] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[1] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[2] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[3] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[4] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[5] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[6] = DXGI_FORMAT_UNKNOWN;
    other.renderTargetFormats[7] = DXGI_FORMAT_UNKNOWN;
    other.depthStencilFormat     = DXGI_FORMAT_UNKNOWN;
    other.primitiveTopology      = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    other.sampleCount            = 0;

    return *this;
}

YaGE::GraphicsPipelineState::~GraphicsPipelineState() noexcept {}
