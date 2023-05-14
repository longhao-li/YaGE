#pragma once

#include "Matrix3.h"
#include "Matrix4.h"

namespace YaGE {

struct Quaternion {
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
    ///   Create a quaternion and initialize with 0.
    YAGE_FORCEINLINE Quaternion() noexcept : _vec(DirectX::XMVectorZero()) {}

    /// @brief
    ///   Create a quaternion from a real number. All imaginary parts are set to 0.
    YAGE_FORCEINLINE Quaternion(float real) noexcept : _vec(DirectX::XMVectorSet(0, 0, 0, real)) {}

    /// @brief
    ///   Create a quaternion from the given values.
    ///
    /// @param real The real part of the quaternion.
    /// @param imgX The x component of the imaginary part of the quaternion.
    /// @param imgY The y component of the imaginary part of the quaternion.
    /// @param imgZ The z component of the imaginary part of the quaternion.
    YAGE_FORCEINLINE Quaternion(float real, float imgX, float imgY, float imgZ) noexcept
        : _vec(DirectX::XMVectorSet(imgX, imgY, imgZ, real)) {}

    /// @brief
    ///   Create a quaternion for rotation around the given axis.
    ///
    /// @param axis     The axis to rotate around.
    /// @param radians  The angle to rotate.
    YAGE_FORCEINLINE Quaternion(Vector3 axis, float radians) noexcept
        : _vec(DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(axis.x, axis.y, axis.z, 0), radians)) {}

    /// @brief
    ///   Create a quaternion for rotation around the given axis.
    ///
    /// @param axis     The axis to rotate around. Must be a valid homogeneous coordinate vector.
    /// @param radians  The angle to rotate.
    YAGE_FORCEINLINE Quaternion(Vector4 axis, float radians) noexcept
        : _vec(DirectX::XMQuaternionRotationAxis(axis._vec, radians)) {}

    /// @brief
    ///   Create a quaternion for rotation around Euler angles.
    ///
    /// @param pitch    The pitch angle in radians.
    /// @param yaw      The yaw angle in radians.
    /// @param roll     The roll angle in radians.
    YAGE_FORCEINLINE Quaternion(float pitch, float yaw, float roll) noexcept
        : _vec(DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll)) {}

    /// @brief
    ///   Create a quaternion for rotation matrix.
    ///
    /// @param mat  The rotation matrix.
    YAGE_FORCEINLINE Quaternion(const Matrix3 &mat) noexcept {
        DirectX::XMMATRIX rotateMat{
            mat[0][0], mat[0][1], mat[0][2], 0.0f, mat[1][0], mat[1][1], mat[1][2], 0.0f,
            mat[2][0], mat[2][1], mat[2][2], 0.0f, 0.0f,      0.0f,      0.0f,      1.0f,
        };
        _vec = DirectX::XMQuaternionRotationMatrix(rotateMat);
    }

    /// @brief
    ///   Create a quaternion for rotation matrix.
    ///
    /// @param mat  The rotation matrix.
    YAGE_FORCEINLINE Quaternion(const Matrix4 &mat) noexcept : _vec(DirectX::XMQuaternionRotationMatrix(mat._xmmat)) {}

    /// @brief
    ///   Create a quaternion from DirectX math vector.
    ///
    /// @param vec  The DirectX math vector.
    YAGE_FORCEINLINE Quaternion(DirectX::XMVECTOR vec) noexcept : _vec(vec) {}

    /// @brief
    ///   Calculate length of the quaternion.
    ///
    /// @return float
    ///   The length of the quaternion.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Length() const noexcept -> float {
        return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(_vec));
    }

    /// @brief
    ///   Normalize this quaternion.
    /// @note
    ///   To get normalized quaternion of this quaternion without modifying this quaternion, use @p Normalized() instead.
    ///
    /// @return Quaternion &
    ///   Return reference to this quaternion after normalization.
    YAGE_FORCEINLINE auto Normalize() noexcept -> Quaternion & {
        _vec = DirectX::XMQuaternionNormalize(_vec);
        return *this;
    }

    /// @brief
    ///   Get normalized quaternion of this quaternion.
    ///
    /// @return Quaternion
    ///   Return the normalized quaternion of this quaternion.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Normalized() const noexcept -> Quaternion {
        return Quaternion(DirectX::XMQuaternionNormalize(_vec));
    }

    /// @brief
    ///   Conjugate this quaternion.
    /// @note
    ///   To get conjugate quaternion of this quaternion without modifying this quaternion, use @p Conjugated() instead.
    ///
    /// @return Quaternion &
    ///   Return reference to this quaternion after conjugation.
    YAGE_FORCEINLINE auto Conjugate() noexcept -> Quaternion & {
        _vec = DirectX::XMQuaternionConjugate(_vec);
        return *this;
    }

    /// @brief
    ///   Get conjugate quaternion of this quaternion.
    ///
    /// @return Quaternion
    ///   Return the conjugate quaternion of this quaternion.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Conjugated() const noexcept -> Quaternion {
        return Quaternion(DirectX::XMQuaternionConjugate(_vec));
    }

    /// @brief
    ///   Invert this quaternion.
    /// @note
    ///   To get inverted quaternion of this quaternion without modifying this quaternion, use @p Inversed() instead.
    ///
    /// @return Quaternion &
    ///   Return reference to this quaternion after inversion.
    YAGE_FORCEINLINE auto Inverse() noexcept -> Quaternion & {
        _vec = DirectX::XMQuaternionInverse(_vec);
        return *this;
    }

    /// @brief
    ///   Get inverted quaternion of this quaternion.
    ///
    /// @return Quaternion
    ///   Return the inverted quaternion of this quaternion.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Inversed() const noexcept -> Quaternion {
        return Quaternion(DirectX::XMQuaternionInverse(_vec));
    }

    YAGE_FORCEINLINE auto operator+() const noexcept -> Quaternion { return Quaternion(_vec); }
    YAGE_FORCEINLINE auto operator-() const noexcept -> Quaternion { return Quaternion(DirectX::XMVectorNegate(_vec)); }

    YAGE_FORCEINLINE auto operator+=(float rhs) noexcept -> Quaternion & {
        _vec = DirectX::XMVectorAdd(_vec, DirectX::XMVectorSet(0, 0, 0, rhs));
        return *this;
    }

    YAGE_FORCEINLINE auto operator+=(Quaternion rhs) noexcept -> Quaternion & {
        _vec = DirectX::XMVectorAdd(_vec, rhs._vec);
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(float rhs) noexcept -> Quaternion & {
        _vec = DirectX::XMVectorSubtract(_vec, DirectX::XMVectorSet(0, 0, 0, rhs));
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(Quaternion rhs) noexcept -> Quaternion & {
        _vec = DirectX::XMVectorSubtract(_vec, rhs._vec);
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(float rhs) noexcept -> Quaternion & {
        _vec = DirectX::XMVectorScale(_vec, rhs);
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(Quaternion rhs) noexcept -> Quaternion & {
        _vec = DirectX::XMQuaternionMultiply(_vec, rhs._vec);
        return *this;
    }

    YAGE_FORCEINLINE auto operator/=(float rhs) noexcept -> Quaternion & {
        _vec = DirectX::XMVectorScale(_vec, 1.0f / rhs);
        return *this;
    }
};

YAGE_FORCEINLINE auto operator==(Quaternion lhs, Quaternion rhs) noexcept -> bool {
    return DirectX::XMVector4Equal(lhs._vec, rhs._vec);
}

YAGE_FORCEINLINE auto operator!=(Quaternion lhs, Quaternion rhs) noexcept -> bool {
    return DirectX::XMVector4NotEqual(lhs._vec, rhs._vec);
}

YAGE_FORCEINLINE auto operator+(Quaternion lhs, Quaternion rhs) noexcept -> Quaternion {
    return Quaternion(DirectX::XMVectorAdd(lhs._vec, rhs._vec));
}

YAGE_FORCEINLINE auto operator+(Quaternion lhs, float rhs) noexcept -> Quaternion {
    return Quaternion(DirectX::XMVectorAdd(lhs._vec, DirectX::XMVectorSet(0, 0, 0, rhs)));
}

YAGE_FORCEINLINE auto operator+(float lhs, Quaternion rhs) noexcept -> Quaternion {
    return Quaternion(DirectX::XMVectorAdd(DirectX::XMVectorSet(0, 0, 0, lhs), rhs._vec));
}

YAGE_FORCEINLINE auto operator-(Quaternion lhs, Quaternion rhs) noexcept -> Quaternion {
    return Quaternion(DirectX::XMVectorSubtract(lhs._vec, rhs._vec));
}

YAGE_FORCEINLINE auto operator-(Quaternion lhs, float rhs) noexcept -> Quaternion {
    return Quaternion(DirectX::XMVectorSubtract(lhs._vec, DirectX::XMVectorSet(0, 0, 0, rhs)));
}

YAGE_FORCEINLINE auto operator-(float lhs, Quaternion rhs) noexcept -> Quaternion {
    return Quaternion(DirectX::XMVectorSubtract(DirectX::XMVectorSet(0, 0, 0, lhs), rhs._vec));
}

YAGE_FORCEINLINE auto operator*(Quaternion lhs, Quaternion rhs) noexcept -> Quaternion {
    return Quaternion(DirectX::XMQuaternionMultiply(lhs._vec, rhs._vec));
}

YAGE_FORCEINLINE auto operator*(Quaternion lhs, float rhs) noexcept -> Quaternion {
    return Quaternion(DirectX::XMVectorScale(lhs._vec, rhs));
}

YAGE_FORCEINLINE auto operator*(float lhs, Quaternion rhs) noexcept -> Quaternion {
    return Quaternion(DirectX::XMVectorScale(rhs._vec, lhs));
}

YAGE_FORCEINLINE auto operator/(Quaternion lhs, float rhs) noexcept -> Quaternion {
    return Quaternion(DirectX::XMVectorScale(lhs._vec, 1.0f / rhs));
}

YAGE_FORCEINLINE auto Dot(Quaternion lhs, Quaternion rhs) noexcept -> float {
    return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(lhs._vec, rhs._vec));
}

YAGE_FORCEINLINE auto NLerp(Quaternion start, Quaternion end, float t) noexcept -> Quaternion {
    return DirectX::XMVectorLerp(start._vec, end._vec, t);
}

YAGE_FORCEINLINE auto SLerp(Quaternion start, Quaternion end, float t) noexcept -> Quaternion {
    return DirectX::XMQuaternionSlerp(start._vec, end._vec, t);
}

/// @brief
///   Create a rotation matrix from quaternion.
///
/// @param quat  The quaternion to initialize the matrix with.
YAGE_FORCEINLINE Matrix3::Matrix3(Quaternion quat) noexcept {
    Matrix4 mat(DirectX::XMMatrixRotationQuaternion(quat._vec));
    data[0] = mat[0].xyz;
    data[1] = mat[1].xyz;
    data[2] = mat[2].xyz;
}

/// @brief
///   Create a rotation matrix from quaternion.
///
/// @param quat  The quaternion to initialize the matrix with.
YAGE_FORCEINLINE Matrix4::Matrix4(Quaternion quat) noexcept : _xmmat(DirectX::XMMatrixRotationQuaternion(quat._vec)) {}

} // namespace YaGE
