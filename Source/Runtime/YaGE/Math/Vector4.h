#pragma once

#include "Vector3.h"

#include <DirectXMath.h>

namespace YaGE {

struct Vector4 {
#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 4201)
#endif
    union {
        DirectX::XMVECTOR _vec;
        float             _arr[4];
        struct {
            float x;
            float y;
            float z;
            float w;
        };
    };
#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif

    /// @brief
    ///   Create a 4D vector and initialize with 0.
    YAGE_FORCEINLINE Vector4() noexcept : _vec(DirectX::XMVectorZero()) {}

    /// @brief
    ///   Create a 4D vector and initialize with the specified value.
    ///
    /// @param v    The value to initialize all elements of the vector with.
    YAGE_FORCEINLINE explicit Vector4(float v) noexcept : _vec(DirectX::XMVectorReplicate(v)) {}

    /// @brief
    ///   Create a 4D vector and initialize with the specified values.
    ///
    /// @param x    The x component of the vector.
    /// @param y    The y component of the vector.
    /// @param z    The z component of the vector.
    /// @param w    The w component of the vector.
    YAGE_FORCEINLINE Vector4(float x, float y, float z, float w) noexcept : _vec(DirectX::XMVectorSet(x, y, z, w)) {}

    /// @brief
    ///   Create a 4D vector and initialize with specified values.
    ///
    /// @param arr  The array of values to initialize the vector with.
    YAGE_FORCEINLINE explicit Vector4(const float arr[4]) noexcept
        : _vec(DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4 *>(arr))) {}

    /// @brief
    ///   Create a 4D vector and initialize with specified values.
    ///
    /// @param xyz  The 3D vector to initialize the vector with.
    /// @param w    The w component of the vector.
    YAGE_FORCEINLINE Vector4(const Vector3 &xyz, float w) noexcept
        : _vec(DirectX::XMVectorSet(xyz.x, xyz.y, xyz.z, w)) {}

    /// @brief
    ///   Create a 4D vector and initialize with specified values.
    ///
    /// @param x    The x component of the vector.
    /// @param yzw  The 3D vector to initialize the vector with.
    YAGE_FORCEINLINE Vector4(float x, const Vector3 &yzw) noexcept
        : _vec(DirectX::XMVectorSet(x, yzw.x, yzw.y, yzw.z)) {}

    /// @brief
    ///   Create a 4D vector and initialize with specified values.
    ///
    /// @param xy   The 2D vector to initialize the vector with.
    /// @param zw   The 2D vector to initialize the vector with.
    YAGE_FORCEINLINE Vector4(const Vector2 &xy, const Vector2 &zw) noexcept
        : _vec(DirectX::XMVectorSet(xy.x, xy.y, zw.x, zw.y)) {}

    /// @brief
    ///   Create a 4D vector and initialize with specified values.
    ///
    /// @param xy   The 2D vector to initialize the vector with.
    /// @param z    The z component of the vector.
    /// @param w    The w component of the vector.
    YAGE_FORCEINLINE Vector4(const Vector2 &xy, float z, float w) noexcept
        : _vec(DirectX::XMVectorSet(xy.x, xy.y, z, w)) {}

    /// @brief
    ///   Create a 4D vector and initialize with specified values.
    ///
    /// @param x    The x component of the vector.
    /// @param yz   The 2D vector to initialize the vector with.
    /// @param w    The w component of the vector.
    YAGE_FORCEINLINE Vector4(float x, const Vector2 &yz, float w) noexcept
        : _vec(DirectX::XMVectorSet(x, yz.x, yz.y, w)) {}

    /// @brief
    ///   Create a 4D vector and initialize with specified values.
    ///
    /// @param x    The x component of the vector.
    /// @param y    The y component of the vector.
    /// @param zw   The 2D vector to initialize the vector with.
    YAGE_FORCEINLINE Vector4(float x, float y, const Vector2 &zw) noexcept
        : _vec(DirectX::XMVectorSet(x, y, zw.x, zw.y)) {}

    /// @brief
    ///   Initialize a 4D vector with DirectX math vector.
    ///
    /// @param v    The DirectX math vector to initialize the vector with.
    YAGE_FORCEINLINE Vector4(DirectX::XMVECTOR v) noexcept : _vec(v) {}

    /// @brief
    ///   Random access elements of this vector by index.
    ///
    /// @param i    The index of the element to access.
    YAGE_NODISCARD YAGE_FORCEINLINE auto operator[](size_t i) noexcept -> float & { return _arr[i]; }

    /// @brief
    ///   Random access elements of this vector by index.
    ///
    /// @param i    The index of the element to access.
    YAGE_NODISCARD YAGE_FORCEINLINE auto operator[](size_t i) const noexcept -> const float & { return _arr[i]; }

    /// @brief
    ///   Calculate length of this vector.
    ///
    /// @return float
    ///   Return length of this 4D vector.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Length() const noexcept -> float {
        return DirectX::XMVectorGetX(DirectX::XMVector4Length(_vec));
    }

    /// @brief
    ///   Normalize this vector.
    /// @note
    ///   To get normalized vector of this vector without modifying this vector, use @p Normalized() instead.
    ///
    /// @return Vector4 &
    ///   Return reference to this vector.
    YAGE_FORCEINLINE auto Normalize() noexcept -> Vector4 & {
        _vec = DirectX::XMVector4Normalize(_vec);
        return *this;
    }

    /// @brief
    ///   Get normalized vector of this vector.
    ///
    /// @return Vector4
    ///   Return normalized vector of this vector.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Normalized() const noexcept -> Vector4 {
        return Vector4(DirectX::XMVector4Normalize(_vec));
    }

    YAGE_NODISCARD YAGE_FORCEINLINE auto operator+() const noexcept -> Vector4 { return Vector4(_vec); }
    YAGE_NODISCARD YAGE_FORCEINLINE auto operator-() const noexcept -> Vector4 { return DirectX::XMVectorNegate(_vec); }

    YAGE_FORCEINLINE auto operator+=(float rhs) noexcept -> Vector4 & {
        _vec = DirectX::XMVectorAdd(_vec, DirectX::XMVectorReplicate(rhs));
        return *this;
    }

    YAGE_FORCEINLINE auto operator+=(Vector4 rhs) noexcept -> Vector4 & {
        _vec = DirectX::XMVectorAdd(_vec, rhs._vec);
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(float rhs) noexcept -> Vector4 & {
        _vec = DirectX::XMVectorSubtract(_vec, DirectX::XMVectorReplicate(rhs));
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(Vector4 rhs) noexcept -> Vector4 & {
        _vec = DirectX::XMVectorSubtract(_vec, rhs._vec);
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(float rhs) noexcept -> Vector4 & {
        _vec = DirectX::XMVectorMultiply(_vec, DirectX::XMVectorReplicate(rhs));
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(Vector4 rhs) noexcept -> Vector4 & {
        _vec = DirectX::XMVectorMultiply(_vec, rhs._vec);
        return *this;
    }

    YAGE_FORCEINLINE auto operator/=(float rhs) noexcept -> Vector4 & {
        _vec = DirectX::XMVectorDivide(_vec, DirectX::XMVectorReplicate(rhs));
        return *this;
    }

    YAGE_FORCEINLINE auto operator/=(Vector4 rhs) noexcept -> Vector4 & {
        _vec = DirectX::XMVectorDivide(_vec, rhs._vec);
        return *this;
    }
};

YAGE_NODISCARD YAGE_FORCEINLINE auto operator==(Vector4 lhs, Vector4 rhs) noexcept -> bool {
    return DirectX::XMVector4Equal(lhs._vec, rhs._vec);
}

YAGE_NODISCARD YAGE_FORCEINLINE auto operator!=(Vector4 lhs, Vector4 rhs) noexcept -> bool {
    return DirectX::XMVector4NotEqual(lhs._vec, rhs._vec);
}

YAGE_FORCEINLINE auto operator+(Vector4 lhs, Vector4 rhs) noexcept -> Vector4 {
    return DirectX::XMVectorAdd(lhs._vec, rhs._vec);
}

YAGE_FORCEINLINE auto operator+(Vector4 lhs, float rhs) noexcept -> Vector4 {
    return DirectX::XMVectorAdd(lhs._vec, DirectX::XMVectorReplicate(rhs));
}

YAGE_FORCEINLINE auto operator+(float lhs, Vector4 rhs) noexcept -> Vector4 {
    return DirectX::XMVectorAdd(DirectX::XMVectorReplicate(lhs), rhs._vec);
}

YAGE_FORCEINLINE auto operator-(Vector4 lhs, Vector4 rhs) noexcept -> Vector4 {
    return DirectX::XMVectorSubtract(lhs._vec, rhs._vec);
}

YAGE_FORCEINLINE auto operator-(Vector4 lhs, float rhs) noexcept -> Vector4 {
    return DirectX::XMVectorSubtract(lhs._vec, DirectX::XMVectorReplicate(rhs));
}

YAGE_FORCEINLINE auto operator*(Vector4 lhs, Vector4 rhs) noexcept -> Vector4 {
    return DirectX::XMVectorMultiply(lhs._vec, rhs._vec);
}

YAGE_FORCEINLINE auto operator*(Vector4 lhs, float rhs) noexcept -> Vector4 {
    return DirectX::XMVectorMultiply(lhs._vec, DirectX::XMVectorReplicate(rhs));
}

YAGE_FORCEINLINE auto operator*(float lhs, Vector4 rhs) noexcept -> Vector4 {
    return DirectX::XMVectorMultiply(DirectX::XMVectorReplicate(lhs), rhs._vec);
}

YAGE_FORCEINLINE auto operator/(Vector4 lhs, Vector4 rhs) noexcept -> Vector4 {
    return DirectX::XMVectorDivide(lhs._vec, rhs._vec);
}

YAGE_FORCEINLINE auto operator/(Vector4 lhs, float rhs) noexcept -> Vector4 {
    return DirectX::XMVectorDivide(lhs._vec, DirectX::XMVectorReplicate(rhs));
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Dot(Vector4 lhs, Vector4 rhs) noexcept -> float {
    return DirectX::XMVectorGetX(DirectX::XMVector4Dot(lhs._vec, rhs._vec));
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Cross(Vector4 lhs, Vector4 rhs) noexcept -> Vector4 {
    return DirectX::XMVector3Cross(lhs._vec, rhs._vec);
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Cross(Vector4 v0, Vector4 v1, Vector4 v2) noexcept -> Vector4 {
    return DirectX::XMVector4Cross(v0._vec, v1._vec, v2._vec);
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Lerp(Vector4 start, Vector4 end, float t) noexcept -> Vector4 {
    return DirectX::XMVectorLerp(start._vec, end._vec, t);
}

} // namespace YaGE
