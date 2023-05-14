#pragma once

#include "Descriptor.h"
#include "PixelBuffer.h"

namespace YaGE {

class Texture : public PixelBuffer {
public:
    /// @brief
    ///   Create an empty texture.
    YAGE_API Texture() noexcept;

    /// @brief
    ///   Create a new texture.
    ///
    /// @param width            Width in pixel of this texture.
    /// @param height           Height in pixel of this texture.
    /// @param arraySize        2D texture array size. For 2D texture, this value is 1.
    /// @param format           Pixel format of this texture.
    /// @param mipmapLevels     Number of mipmap levels of this texture. Please note that mipmap will not be automatically generated. Pass 0 to use maximum supported mipmap levels.
    /// @param isCubeTexture    Specifies whether this is a cube texture. This value is used only when array size is a multiple of 6.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create D3D12 resource for this texture.
    YAGE_API Texture(uint32_t    width,
                     uint32_t    height,
                     uint32_t    arraySize,
                     DXGI_FORMAT format,
                     uint32_t    mipmapLevels,
                     bool        isCubeTexture = false);

    /// @brief
    ///   Create a 2D texture.
    ///
    /// @param width            Width in pixel of this texture.
    /// @param height           Height in pixel of this texture.
    /// @param format           Pixel format of this texture.
    /// @param mipmapLevels     Number of mipmap levels of this texture. Please note that mipmap will not be automatically generated. Pass 0 to use maximum supported mipmap levels.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create D3D12 resource for this texture.
    YAGE_API Texture(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t mipmapLevels);

    /// @brief
    ///   Move constructor of Texture. The moved Texture will be invalidated.
    ///
    /// @param other    The Texture to be moved.
    YAGE_API Texture(Texture &&other) noexcept;

    /// @brief
    ///   Move assignment of Texture. The moved Texture will be invalidated.
    ///
    /// @param other    The Texture to be moved.
    ///
    /// @return Texture &
    ///   Return a reference to this Texture.
    YAGE_API auto operator=(Texture &&other) noexcept -> Texture &;

    /// @brief
    ///   Destroy this texture.
    YAGE_API ~Texture() noexcept override;

    /// @brief
    ///   Checks whether this is a cube texture.
    ///
    /// @return bool
    /// @retval true    This is a cube texture.
    /// @retval false   This is not a cube texture.
    YAGE_NODISCARD auto IsCubeTexture() const noexcept -> bool { return isCubeTexture; }

    /// @brief
    ///   Get shader resource view that referres to the whole texture. For cube texture, this is a texture cube shader resource view.
    ///
    /// @return CpuDescriptorHandle
    ///   Return a CPU descriptor handle to the shader resource view.
    YAGE_NODISCARD auto ShaderResourceView() const noexcept -> CpuDescriptorHandle { return srv; }

private:
    /// @brief  Specifies whether this is a cube texture.
    bool isCubeTexture;

    /// @brief  Shader resource view that referres to the whole texture. For cube texture, this is a texture cube shader resource view.
    YaGE::ShaderResourceView srv;
};

} // namespace YaGE
