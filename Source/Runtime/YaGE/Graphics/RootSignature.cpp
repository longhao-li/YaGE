#include "RootSignature.h"
#include "../Core/Exception.h"
#include "RenderDevice.h"

#include <d3dcompiler.h>

#include <vector>

using namespace YaGE;

YaGE::RootSignature::RootSignature(const RootSignatureInfo &info, D3D12_ROOT_SIGNATURE_FLAGS flags)
    : rootSignature(),
      constantBufferViewOffset(),
      constantBufferViewCount(),
      shaderResourceViewOffset(),
      shaderResourceViewCount(),
      unorderedAccessViewOffset(),
      unorderedAccessViewCount(),
      samplerViewOffset(),
      samplerViewCount() {
    std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
    std::vector<D3D12_DESCRIPTOR_RANGE> samplerRanges;

    // Apply offsets.
    for (size_t i = 0; i < std::size(constantBufferViewOffset); ++i) {
        constantBufferViewOffset[i] = constantBufferViewCount;
        constantBufferViewCount += info.constantBufferViewCount[i];

        if (info.constantBufferViewCount[i] != 0)
            ranges.push_back(D3D12_DESCRIPTOR_RANGE{
                /* RangeType                         = */ D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
                /* NumDescriptors                    = */ info.constantBufferViewCount[i],
                /* BaseShaderRegister                = */ 0,
                /* RegisterSpace                     = */ static_cast<UINT>(i),
                /* OffsetInDescriptorsFromTableStart = */ constantBufferViewOffset[i],
            });
    }

    for (size_t i = 0; i < std::size(shaderResourceViewOffset); ++i) {
        shaderResourceViewOffset[i] = shaderResourceViewCount;
        shaderResourceViewOffset[i] += constantBufferViewCount;
        shaderResourceViewCount += info.shaderResourceViewCount[i];

        if (info.shaderResourceViewCount[i] != 0)
            ranges.push_back(D3D12_DESCRIPTOR_RANGE{
                /* RangeType                         = */ D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                /* NumDescriptors                    = */ info.shaderResourceViewCount[i],
                /* BaseShaderRegister                = */ 0,
                /* RegisterSpace                     = */ static_cast<UINT>(i),
                /* OffsetInDescriptorsFromTableStart = */ shaderResourceViewOffset[i],
            });
    }

    for (size_t i = 0; i < std::size(unorderedAccessViewOffset); ++i) {
        unorderedAccessViewOffset[i] = unorderedAccessViewCount;
        unorderedAccessViewOffset[i] += constantBufferViewCount;
        unorderedAccessViewOffset[i] += shaderResourceViewCount;
        unorderedAccessViewCount += info.unorderedAccessViewCount[i];

        if (info.unorderedAccessViewCount[i] != 0)
            ranges.push_back(D3D12_DESCRIPTOR_RANGE{
                /* RangeType                         = */ D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
                /* NumDescriptors                    = */ info.unorderedAccessViewCount[i],
                /* BaseShaderRegister                = */ 0,
                /* RegisterSpace                     = */ static_cast<UINT>(i),
                /* OffsetInDescriptorsFromTableStart = */ unorderedAccessViewOffset[i],
            });
    }

    for (size_t i = 0; i < std::size(samplerViewOffset); ++i) {
        samplerViewOffset[i] = samplerViewCount;
        samplerViewCount += info.samplerViewCount[i];

        if (info.samplerViewCount[i] != 0)
            samplerRanges.push_back(D3D12_DESCRIPTOR_RANGE{
                /* RangeType                         = */ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
                /* NumDescriptors                    = */ info.samplerViewCount[i],
                /* BaseShaderRegister                = */ 0,
                /* RegisterSpace                     = */ static_cast<UINT>(i),
                /* OffsetInDescriptorsFromTableStart = */ samplerViewOffset[i],
            });
    }

    D3D12_ROOT_PARAMETER rootParamerers[2];
    uint32_t             rootParameterCount = 0;

    if (!ranges.empty()) {
        rootParamerers[rootParameterCount].ParameterType   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParamerers[rootParameterCount].DescriptorTable = {
            /* NumDescriptorRanges = */ static_cast<UINT>(ranges.size()),
            /* pDescriptorRanges   = */ ranges.data(),
        };
        rootParamerers[rootParameterCount].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        rootParameterCount += 1;
    }

    if (!samplerRanges.empty()) {
        rootParamerers[rootParameterCount].ParameterType   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParamerers[rootParameterCount].DescriptorTable = {
            /* NumDescriptorRanges = */ static_cast<UINT>(samplerRanges.size()),
            /* pDescriptorRanges   = */ samplerRanges.data(),
        };
        rootParamerers[rootParameterCount].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        rootParameterCount += 1;
    }

    const D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{
        /* NumParameters     = */ rootParameterCount,
        /* pParameters       = */ rootParamerers,
        /* NumStaticSamplers = */ 0,
        /* pStaticSamplers   = */ nullptr,
        /* Flags             = */ flags,
    };

    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSignature;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
                                             serializedRootSignature.GetAddressOf(), nullptr);
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to serialize root signature: ");

    RenderDevice &device = RenderDevice::Singleton();
    // Create root signature.
    hr = device.Device()->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(),
                                              serializedRootSignature->GetBufferSize(),
                                              IID_PPV_ARGS(rootSignature.GetAddressOf()));
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to create root signature.");
}

YaGE::RootSignature::~RootSignature() noexcept {}
