#pragma once

#include "RootSignature.h"

namespace YaGE {

class PipelineState {
protected:
    /// @brief
    ///   Create a pipeline state object.
    PipelineState(YaGE::RootSignature &rootSig) noexcept : rootSignature(&rootSig), pipelineState() {}

public:
    /// @brief
    ///   Copy constructor of pipeline state object is disabled.
    PipelineState(const PipelineState &) = delete;

    /// @brief
    ///   Copy assignment of pipeline state object is disabled.
    auto operator=(const PipelineState &) = delete;

    /// @brief
    ///   Move constructor of pipeline state object.
    ///
    /// @param other    The pipeline state object to moved. The moved pipeline state object will be invalidated.
    PipelineState(PipelineState &&other) noexcept = default;

    /// @brief
    ///   Move assignment of pipeline state object.
    ///
    /// @param other    The pipeline state object to moved.
    ///
    /// @return PipelineState &
    ///   Return reference to this pipeline state object.
    auto operator=(PipelineState &&other) noexcept -> PipelineState & = default;

    /// @brief
    ///   Destroy this pipeline state object.
    YAGE_API virtual ~PipelineState() noexcept;

    /// @brief
    ///   Get root signature of this pipeline state.
    ///
    /// @return RootSignature &
    ///   Return reference to root signature of this pipeline state.
    YAGE_NODISCARD auto RootSignature() const noexcept -> YaGE::RootSignature & { return *rootSignature; }

    /// @brief
    ///   Get D3D12 pipeline state object.
    ///
    /// @return ID3D12PipelineState *
    ///   Return D3D12 pipeline state object.
    YAGE_NODISCARD auto D3D12PipelineState() const noexcept -> ID3D12PipelineState * { return pipelineState.Get(); }

protected:
    /// @brief  Root signature of this pipeline state. This pointer should never be nullptr for valid pipeline state.
    YaGE::RootSignature *rootSignature;

    /// @brief  D3D12 pipeline state object.
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
};

class GraphicsPipelineState : public PipelineState {
public:
    /// @brief
    ///   Create a graphics pipeline state object.
    ///
    /// @param[in] rootSignature    Root signature of this graphics pipeline state.
    /// @param[in] desc             D3D12 graphics pipeline state description.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create graphics pipeline state object.
    YAGE_API GraphicsPipelineState(YaGE::RootSignature &rootSignature, const D3D12_GRAPHICS_PIPELINE_STATE_DESC &desc);

    /// @brief
    ///   Move constructor of graphics pipeline state object.
    ///
    /// @param other    The graphics pipeline state object to moved. The moved graphics pipeline state object will be invalidated.
    YAGE_API GraphicsPipelineState(GraphicsPipelineState &&other) noexcept;

    /// @brief
    ///   Move assignment of graphics pipeline state object.
    ///
    /// @param other    The graphics pipeline state object to moved. The moved graphics pipeline state object will be invalidated.
    ///
    /// @return GraphicsPipelineState &
    ///   Return reference to this graphics pipeline state object.
    YAGE_API auto operator=(GraphicsPipelineState &&other) noexcept -> GraphicsPipelineState &;

    /// @brief
    ///   Destroy this graphics pipeline state object.
    YAGE_API ~GraphicsPipelineState() noexcept override;

    /// @brief
    ///   Get number of render targets in this graphics pipeline state.
    ///
    /// @return uint32_t
    ///   Return number of render targets in this graphics pipeline state.
    YAGE_NODISCARD auto RenderTargetCount() const noexcept -> uint32_t { return renderTargetCount; }

    /// @brief
    ///   Get pixel format for the specified render target.
    ///
    /// @param index    Index of the render target.
    ///
    /// @return DXGI_FORMAT
    ///   Return pixel format for the specified render target. Return @p DXGI_FORMAT_UNKNOWN if index is out of range.
    YAGE_NODISCARD auto RenderTargetFormat(uint32_t index) const noexcept -> DXGI_FORMAT {
        if (index >= renderTargetCount)
            return DXGI_FORMAT_UNKNOWN;

        return renderTargetFormats[index];
    }

    /// @brief
    ///   Get depth stencil format of this graphics pipeline state.
    ///
    /// @return DXGI_FORMAT
    ///   Return depth stencil format of this graphics pipeline state.
    YAGE_NODISCARD auto DepthStencilFormat() const noexcept -> DXGI_FORMAT { return depthStencilFormat; }

    /// @brief
    ///   Get primitive topology type of this graphics pipeline state.
    ///
    /// @return D3D12_PRIMITIVE_TOPOLOGY_TYPE
    ///   Return primitive topology type of this graphics pipeline state.
    YAGE_NODISCARD auto PrimitiveTopology() const noexcept -> D3D12_PRIMITIVE_TOPOLOGY_TYPE {
        return primitiveTopology;
    }

    /// @brief
    ///   Get sample count of this graphics pipeline state.
    ///
    /// @return uint32_t
    ///   Return sample count of this graphics pipeline state.
    YAGE_NODISCARD auto SampleCount() const noexcept -> uint32_t { return sampleCount; }

private:
    /// @brief  Number of render targets in this graphics pipeline state.
    uint32_t renderTargetCount;

    /// @brief  Render target formats of this graphics pipeline state.
    DXGI_FORMAT renderTargetFormats[8];

    /// @brief  Depth stencil format of this graphics pipeline state.
    DXGI_FORMAT depthStencilFormat;

    /// @brief  Primitive topology type of this graphics pipeline state.
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology;

    /// @brief  Sample count of this graphics pipeline state.
    uint32_t sampleCount;
};

} // namespace YaGE
