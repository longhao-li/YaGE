#include "RootSignature.h"
#include "../Core/Exception.h"
#include "RenderDevice.h"

#include <d3dcompiler.h>

#include <vector>

using namespace YaGE;
using Microsoft::WRL::ComPtr;

YaGE::RootSignature::RootSignature(const D3D12_ROOT_SIGNATURE_DESC &desc)
    : rootSignature(),
      tableDescriptorCount(),
      staticSamplerCount(),
      samplerCount(),
      descriptorTableFlags(),
      samplerTableFlags(),
      descriptorTableSizes() {
    // Serialize root signature desc.
    HRESULT          hr = S_OK;
    ComPtr<ID3DBlob> serializedDesc;

    hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, serializedDesc.GetAddressOf(), nullptr);
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to serialize root signature.");

    // Create root signature.
    RenderDevice &device = RenderDevice::Singleton();
    hr = device.Device()->CreateRootSignature(0, serializedDesc->GetBufferPointer(), serializedDesc->GetBufferSize(),
                                              IID_PPV_ARGS(rootSignature.GetAddressOf()));
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to create root signature.");

    // Cache metadata.
    staticSamplerCount = desc.NumStaticSamplers;

    for (uint32_t i = 0; i < desc.NumParameters; ++i) {
        const D3D12_ROOT_PARAMETER &param = desc.pParameters[i];
        if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
            const uint32_t numRanges = param.DescriptorTable.NumDescriptorRanges;
            const auto    *ranges    = param.DescriptorTable.pDescriptorRanges;

            for (uint32_t j = 0; j < numRanges; ++j)
                descriptorTableSizes[i] += ranges[j].NumDescriptors;

            if (ranges->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER) {
                samplerTableFlags[i] = 1;
                samplerCount += descriptorTableSizes[i];
            } else {
                descriptorTableFlags[i] = 1;
                tableDescriptorCount += descriptorTableSizes[i];
            }
        }
    }
}

YaGE::RootSignature::~RootSignature() noexcept {}
