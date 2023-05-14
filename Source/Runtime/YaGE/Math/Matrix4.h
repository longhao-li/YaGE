#pragma once

#include "Vector4.h"

namespace YaGE {

struct Quaternion;

struct Matrix4 {
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
        Vector4           data[4];
        DirectX::XMMATRIX _xmmat;
    };
#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif

    /// @brief
    ///   Initialize this matrix with 0.
    YAGE_FORCEINLINE Matrix4() noexcept : data() {}

    /// @brief
    ///   Create a diagonal matrix with the specified values.
    ///
    /// @param v00  The value at (0, 0).
    /// @param v11  The value at (1, 1).
    /// @param v22  The value at (2, 2).
    /// @param v33  The value at (3, 3).
    YAGE_FORCEINLINE Matrix4(float v00, float v11, float v22, float v33) noexcept
        : _xmmat(DirectX::XMMatrixSet(v00, 0, 0, 0, 0, v11, 0, 0, 0, 0, v22, 0, 0, 0, 0, v33)) {}

    /// @brief
    ///   Create a diagonal matrix with the specified value.
    ///
    /// @param value  The value at (0, 0), (1, 1), (2, 2) and (3, 3).
    YAGE_FORCEINLINE explicit Matrix4(float value) noexcept : Matrix4(value, value, value, value) {}

    // clang-format off
    /// @brief
    ///   Create a matrix with the specified values.
    ///
    /// @param v00  The value at (0, 0).
    /// @param v01  The value at (0, 1).
    /// @param v02  The value at (0, 2).
    /// @param v03  The value at (0, 3).
    /// @param v10  The value at (1, 0).
    /// @param v11  The value at (1, 1).
    /// @param v12  The value at (1, 2).
    /// @param v13  The value at (1, 3).
    /// @param v20  The value at (2, 0).
    /// @param v21  The value at (2, 1).
    /// @param v22  The value at (2, 2).
    /// @param v23  The value at (2, 3).
    /// @param v30  The value at (3, 0).
    /// @param v31  The value at (3, 1).
    /// @param v32  The value at (3, 2).
    /// @param v33  The value at (3, 3).
    YAGE_FORCEINLINE Matrix4(float v00, float v01, float v02, float v03,
                             float v10, float v11, float v12, float v13,
                             float v20, float v21, float v22, float v23,
                             float v30, float v31, float v32, float v33) noexcept
        : _xmmat(DirectX::XMMatrixSet(v00, v01, v02, v03, v10, v11, v12, v13, v20, v21, v22, v23, v30, v31, v32, v33)) {}
    // clang-format on

    /// @brief
    ///   Create a matrix with the specified values.
    ///
    /// @param row0  The values of the first row.
    /// @param row1  The values of the second row.
    /// @param row2  The values of the third row.
    /// @param row3  The values of the fourth row.
    YAGE_FORCEINLINE
    Matrix4(const Vector4 &row0, const Vector4 &row1, const Vector4 &row2, const Vector4 &row3) noexcept
        : data{row0, row1, row2, row3} {}

    /// @brief
    ///   Create a matrix with the specified values.
    ///
    /// @param arr  The array of values to initialize the matrix with.
    YAGE_FORCEINLINE explicit Matrix4(const float arr[16]) noexcept : _xmmat(arr) {}

    /// @brief
    ///   Create a matrix from DirectX math matrix.
    ///
    /// @param mat  The DirectX math matrix to initialize the matrix with.
    YAGE_FORCEINLINE Matrix4(DirectX::XMMATRIX mat) noexcept : _xmmat(mat) {}

    /// @brief
    ///   Create a rotation matrix from quaternion.
    ///
    /// @param quat  The quaternion to initialize the matrix with.
    YAGE_FORCEINLINE Matrix4(Quaternion quat) noexcept;

    /// @brief
    ///   Random access elements of this matrix by index.
    ///
    /// @param i  The index of the element to access.
    YAGE_NODISCARD YAGE_FORCEINLINE auto operator[](size_t i) noexcept -> Vector4 & { return data[i]; }

    /// @brief
    ///   Random access elements of this matrix by index.
    ///
    /// @param i  The index of the element to access.
    YAGE_NODISCARD YAGE_FORCEINLINE auto operator[](size_t i) const noexcept -> const Vector4 & { return data[i]; }

    /// @brief
    ///   Calculate determinant of this matrix.
    ///
    /// @return float
    ///   Return the determinant of this matrix.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Determinant() const noexcept -> float {
        return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(_xmmat));
    }

    /// @brief
    ///   Transpose this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a transposed matrix, use Transposed() instead.
    ///
    /// @return Matrix4 &
    ///   Return reference to this matrix after transposing.
    YAGE_FORCEINLINE auto Transpose() noexcept -> Matrix4 & {
        _xmmat = DirectX::XMMatrixTranspose(_xmmat);
        return *this;
    }

    /// @brief
    ///   Get a transposed matrix of this matrix.
    ///
    /// @return Matrix4
    ///   Return a transposed matrix of this matrix.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Transposed() const noexcept -> Matrix4 {
        return Matrix4(DirectX::XMMatrixTranspose(_xmmat));
    }

    /// @brief
    ///   Inverse this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a inversed matrix, use Inversed() instead.
    ///
    /// @return Matrix4 &
    ///   Return reference to this matrix after inverting.
    YAGE_FORCEINLINE auto Inverse() noexcept -> Matrix4 & {
        _xmmat = DirectX::XMMatrixInverse(nullptr, _xmmat);
        return *this;
    }

    /// @brief
    ///   Get a inversed matrix of this matrix.
    ///
    /// @return Matrix4
    ///   Return a inversed matrix of this matrix.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Inversed() const noexcept -> Matrix4 {
        return Matrix4(DirectX::XMMatrixInverse(nullptr, _xmmat));
    }

    /// @brief
    ///   Apply scale transform to this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a scaled matrix, use Scaled() instead.
    ///
    /// @param x    X scale factor.
    /// @param y    Y scale factor.
    /// @param z    Z scale factor.
    /// @param w    W scale factor.
    ///
    /// @return Matrix4 &
    ///   Return reference to this matrix after scaling.
    YAGE_FORCEINLINE auto Scale(float x, float y, float z, float w) noexcept -> Matrix4 & {
        Vector4 scale(x, y, z, w);
        data[0] *= scale;
        data[1] *= scale;
        data[2] *= scale;
        data[3] *= scale;
        return *this;
    }

    /// @brief
    ///   Apply scale transform to this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a scaled matrix, use Scaled() instead.
    ///
    /// @param x    X scale factor.
    /// @param y    Y scale factor.
    /// @param z    Z scale factor.
    ///
    /// @return Matrix4 &
    ///   Return reference to this matrix after scaling.
    YAGE_FORCEINLINE auto Scale(float x, float y, float z) noexcept -> Matrix4 & {
        Vector4 scale(x, y, z, 1.0f);
        data[0] *= scale;
        data[1] *= scale;
        data[2] *= scale;
        data[3] *= scale;
        return *this;
    }

    /// @brief
    ///   Apply scale transform to this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a scaled matrix, use Scaled() instead.
    ///
    /// @param scale    Scale factors of each dimension.
    ///
    /// @return Matrix4 &
    ///   Return reference to this matrix after scaling.
    YAGE_FORCEINLINE auto Scale(Vector4 scale) noexcept -> Matrix4 & {
        data[0] *= scale;
        data[1] *= scale;
        data[2] *= scale;
        data[3] *= scale;
        return *this;
    }

    /// @brief
    ///   Apply scale transform to this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a scaled matrix, use Scaled() instead.
    ///
    /// @param scale    Scale factors of each dimension.
    ///
    /// @return Matrix4 &
    ///   Return reference to this matrix after scaling.
    YAGE_FORCEINLINE auto Scale(Vector3 scale) noexcept -> Matrix4 & {
        Vector4 scale4(scale, 1.0f);
        data[0] *= scale4;
        data[1] *= scale4;
        data[2] *= scale4;
        data[3] *= scale4;
        return *this;
    }

    /// @brief
    ///   Get the matrix that is applied by the specified scale factors.
    ///
    /// @param x    X scale factor.
    /// @param y    Y scale factor.
    /// @param z    Z scale factor.
    /// @param w    W scale factor.
    ///
    /// @return Matrix4
    ///   Return a matrix that is applied by the specified scale factors.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Scaled(float x, float y, float z, float w) const noexcept -> Matrix4 {
        Vector4 scale(x, y, z, w);
        return Matrix4(data[0] * scale, data[1] * scale, data[2] * scale, data[3] * scale);
    }

    /// @brief
    ///   Get the matrix that is applied by the specified scale factors.
    ///
    /// @param x    X scale factor.
    /// @param y    Y scale factor.
    /// @param z    Z scale factor.
    ///
    /// @return Matrix4
    ///   Return a matrix that is applied by the specified scale factors.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Scaled(float x, float y, float z) const noexcept -> Matrix4 {
        Vector4 scale(x, y, z, 1.0f);
        return Matrix4(data[0] * scale, data[1] * scale, data[2] * scale, data[3] * scale);
    }

    /// @brief
    ///   Get the matrix that is applied by the specified scale factors.
    ///
    /// @param scale    Scale factors of each dimension.
    ///
    /// @return Matrix4
    ///   Return a matrix that is applied by the specified scale factors.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Scaled(Vector4 scale) const noexcept -> Matrix4 {
        return Matrix4(data[0] * scale, data[1] * scale, data[2] * scale, data[3] * scale);
    }

    /// @brief
    ///   Get the matrix that is applied by the specified scale factors.
    ///
    /// @param scale    Scale factors of each dimension.
    ///
    /// @return Matrix4
    ///   Return a matrix that is applied by the specified scale factors.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Scaled(Vector3 scale) const noexcept -> Matrix4 {
        Vector4 scale4(scale, 1.0f);
        return Matrix4(data[0] * scale4, data[1] * scale4, data[2] * scale4, data[3] * scale4);
    }

    /// @brief
    ///   Apply rotate transform to this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a rotated matrix, use Rotated() instead.
    ///
    /// @param axis     Rotation axis. Should not be zero vector.
    /// @param radian   Rotation angle in radian.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after rotation.
    YAGE_FORCEINLINE auto Rotate(Vector3 axis, float radian) noexcept -> Matrix4 & {
        _xmmat *= DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(axis.x, axis.y, axis.z, 0), radian);
        return *this;
    }

    /// @brief
    ///   Apply rotate transform to this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a rotated matrix, use Rotated() instead.
    ///
    /// @param axis     Rotation axis. Must be a valid homogeneous vector.
    /// @param radian   Rotation angle in radian.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after rotation.
    YAGE_FORCEINLINE auto Rotate(Vector4 axis, float radian) noexcept -> Matrix4 & {
        _xmmat *= DirectX::XMMatrixRotationAxis(axis._vec, radian);
        return *this;
    }

    /// @brief
    ///   Get the matrix that is applied by the specified rotation.
    ///
    /// @param axis     Rotation axis. Should not be zero vector.
    /// @param radian   Rotation angle in radian.
    ///
    /// @return Matrix4
    ///   Return a matrix that is applied by the specified rotation.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Rotated(Vector3 axis, float radian) const noexcept -> Matrix4 {
        auto rotate = DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(axis.x, axis.y, axis.z, 0), radian);
        return Matrix4(_xmmat * rotate);
    }

    /// @brief
    ///   Get the matrix that is applied by the specified rotation.
    ///
    /// @param axis     Rotation axis. Must be a valid homogeneous vector.
    /// @param radian   Rotation angle in radian.
    ///
    /// @return Matrix4
    ///   Return a matrix that is applied by the specified rotation.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Rotated(Vector4 axis, float radian) const noexcept -> Matrix4 {
        auto rotate = DirectX::XMMatrixRotationAxis(axis._vec, radian);
        return Matrix4(_xmmat * rotate);
    }

    /// @brief
    ///   Apply translation transform to this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a translated matrix, use Translated() instead.
    ///
    /// @param x    X translation offset.
    /// @param y    Y translation offset.
    /// @param z    Z translation offset.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after translation.
    YAGE_FORCEINLINE auto Translate(float x, float y, float z) noexcept -> Matrix4 & {
        Vector4 translate(x, y, z, 0);
        data[0] += (data[0][3] * translate);
        data[1] += (data[1][3] * translate);
        data[2] += (data[2][3] * translate);
        data[3] += (data[3][3] * translate);
        return *this;
    }

    /// @brief
    ///   Apply translation transform to this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a translated matrix, use Translated() instead.
    ///
    /// @param offset    Translation offset.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after translation.
    YAGE_FORCEINLINE auto Translate(Vector3 offset) noexcept -> Matrix4 & {
        Vector4 translate(offset, 0);
        data[0] += (data[0][3] * translate);
        data[1] += (data[1][3] * translate);
        data[2] += (data[2][3] * translate);
        data[3] += (data[3][3] * translate);
        return *this;
    }

    /// @brief
    ///   Apply translation transform to this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a translated matrix, use Translated() instead.
    ///
    /// @param offset    Translation offset. Must be a valid homogeneous vector.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after translation.
    YAGE_FORCEINLINE auto Translate(Vector4 offset) noexcept -> Matrix4 & {
        data[0] += (data[0][3] * offset);
        data[1] += (data[1][3] * offset);
        data[2] += (data[2][3] * offset);
        data[3] += (data[3][3] * offset);
        return *this;
    }

    /// @brief
    ///   Get the matrix that is applied by the specified translation.
    ///
    /// @param x    X translation offset.
    /// @param y    Y translation offset.
    /// @param z    Z translation offset.
    ///
    /// @return Matrix4
    ///   Return a matrix that is applied by the specified translation.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Translated(float x, float y, float z) const noexcept -> Matrix4 {
        Vector4 translate(x, y, z, 0);
        return Matrix4(data[0] + (data[0][3] * translate), data[1] + (data[1][3] * translate),
                       data[2] + (data[2][3] * translate), data[3] + (data[3][3] * translate));
    }

    /// @brief
    ///   Get the matrix that is applied by the specified translation.
    ///
    /// @param offset    Translation offset.
    ///
    /// @return Matrix4
    ///   Return a matrix that is applied by the specified translation.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Translated(Vector3 offset) const noexcept -> Matrix4 {
        Vector4 translate(offset, 0);
        return Matrix4(data[0] + (data[0][3] * translate), data[1] + (data[1][3] * translate),
                       data[2] + (data[2][3] * translate), data[3] + (data[3][3] * translate));
    }

    /// @brief
    ///   Get the matrix that is applied by the specified translation.
    ///
    /// @param offset    Translation offset.
    ///
    /// @return Matrix4
    ///   Return a matrix that is applied by the specified translation.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Translated(Vector4 offset) const noexcept -> Matrix4 {
        return Matrix4(data[0] + (data[0][3] * offset), data[1] + (data[1][3] * offset),
                       data[2] + (data[2][3] * offset), data[3] + (data[3][3] * offset));
    }

    /// @brief
    ///   Apply look at transform to this matrix.
    /// @note
    ///   This function will change the matrix itself.
    ///
    /// @param eye      Eye position. Must be a valid homogeneous coordinate point.
    /// @param target   Target position. Must be a valid homogeneous coordinate point.
    /// @param up       Up direction. Must be a valid homogeneous coordinate vector.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after look at transform.
    YAGE_FORCEINLINE auto LookAt(Vector4 eye, Vector4 target, Vector4 up) noexcept -> Matrix4 & {
        _xmmat *= DirectX::XMMatrixLookAtLH(eye._vec, target._vec, up._vec);
        return *this;
    }

    /// @brief
    ///   Apply look at transform to this matrix.
    /// @note
    ///   This function will change the matrix itself.
    ///
    /// @param eye      Eye position.
    /// @param target   Target position.
    /// @param up       Up direction.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after look at transform.
    YAGE_FORCEINLINE auto LookAt(Vector3 eye, Vector3 target, Vector3 up) noexcept -> Matrix4 & {
        auto eyePos    = DirectX::XMVectorSet(eye.x, eye.y, eye.z, 1.0f);
        auto targetPos = DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f);
        auto upDir     = DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f);
        _xmmat *= DirectX::XMMatrixLookAtLH(eyePos, targetPos, upDir);
        return *this;
    }

    /// @brief
    ///   Apply look to transform to this matrix.
    /// @note
    ///   This function will change the matrix itself.
    ///
    /// @param eye       Eye position. Must be a valid homogeneous coordinate point.
    /// @param direction Look to direction. Must be a valid homogeneous coordinate vector.
    /// @param up        Up direction. Must be a valid homogeneous coordinate vector.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after look to transform.
    YAGE_FORCEINLINE auto LookTo(Vector4 eye, Vector4 direction, Vector4 up) noexcept -> Matrix4 & {
        _xmmat *= DirectX::XMMatrixLookToLH(eye._vec, direction._vec, up._vec);
        return *this;
    }

    /// @brief
    ///   Apply look to transform to this matrix.
    /// @note
    ///   This function will change the matrix itself.
    ///
    /// @param eye       Eye position.
    /// @param direction Look to direction.
    /// @param up        Up direction.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after look to transform.
    YAGE_FORCEINLINE auto LookTo(Vector3 eye, Vector3 direction, Vector3 up) noexcept -> Matrix4 & {
        auto eyePos    = DirectX::XMVectorSet(eye.x, eye.y, eye.z, 1.0f);
        auto targetDir = DirectX::XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
        auto upDir     = DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f);
        _xmmat *= DirectX::XMMatrixLookToLH(eyePos, targetDir, upDir);
        return *this;
    }

    /// @brief
    ///   Apply perspective projection transform to this matrix.
    /// @note
    ///   This function will change the matrix itself.
    ///
    /// @param fovY          Field of view angle in the y direction, in radians.
    /// @param aspectRatio   Aspect ratio, defined as view space width divided by height.
    /// @param zNear         Distance to the near view plane.
    /// @param zFar          Distance to the far view plane.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after perspective transform.
    YAGE_FORCEINLINE auto Perspective(float fovY, float aspectRatio, float zNear, float zFar) noexcept -> Matrix4 & {
        _xmmat *= DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, zNear, zFar);
        return *this;
    }

    /// @brief
    ///   Apply orthographic projection transform to this matrix.
    /// @note
    ///   This function will change the matrix itself.
    ///
    /// @param width    Width of the view volume.
    /// @param height   Height of the view volume.
    /// @param zNear    Minimum z-value of the view volume.
    /// @param zFar     Maximum z-value of the view volume.
    ///
    /// @return Matrix4f &
    ///   Return reference to this matrix after orthographic transform.
    YAGE_FORCEINLINE auto Orthographic(float width, float height, float zNear, float zFar) noexcept -> Matrix4 & {
        _xmmat *= DirectX::XMMatrixOrthographicLH(width, height, zNear, zFar);
        return *this;
    }

    YAGE_FORCEINLINE auto operator+() const noexcept -> Matrix4 { return Matrix4(_xmmat); }
    YAGE_FORCEINLINE auto operator-() const noexcept -> Matrix4 { return Matrix4(-_xmmat); }

    YAGE_FORCEINLINE auto operator+=(float rhs) noexcept -> Matrix4 & {
        data[0] += rhs;
        data[1] += rhs;
        data[2] += rhs;
        data[3] += rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator+=(const Matrix4 &rhs) noexcept -> Matrix4 & {
        data[0] += rhs.data[0];
        data[1] += rhs.data[1];
        data[2] += rhs.data[2];
        data[3] += rhs.data[3];
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(float rhs) noexcept -> Matrix4 & {
        data[0] -= rhs;
        data[1] -= rhs;
        data[2] -= rhs;
        data[3] -= rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(const Matrix4 &rhs) noexcept -> Matrix4 & {
        data[0] -= rhs.data[0];
        data[1] -= rhs.data[1];
        data[2] -= rhs.data[2];
        data[3] -= rhs.data[3];
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(float rhs) noexcept -> Matrix4 & {
        data[0] *= rhs;
        data[1] *= rhs;
        data[2] *= rhs;
        data[3] *= rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(const Matrix4 &rhs) noexcept -> Matrix4 & {
        _xmmat *= rhs._xmmat;
        return *this;
    }

    YAGE_FORCEINLINE auto operator/=(float rhs) noexcept -> Matrix4 & {
        data[0] /= rhs;
        data[1] /= rhs;
        data[2] /= rhs;
        data[3] /= rhs;
        return *this;
    }
};

YAGE_NODISCARD YAGE_FORCEINLINE auto operator==(const Matrix4 &lhs, const Matrix4 &rhs) noexcept -> bool {
    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];
}

YAGE_NODISCARD YAGE_FORCEINLINE auto operator!=(const Matrix4 &lhs, const Matrix4 &rhs) noexcept -> bool {
    return !(lhs == rhs);
}

YAGE_FORCEINLINE auto operator+(const Matrix4 &lhs, const Matrix4 &rhs) noexcept -> Matrix4 {
    return Matrix4(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]);
}

YAGE_FORCEINLINE auto operator+(const Matrix4 &lhs, float rhs) noexcept -> Matrix4 {
    return Matrix4(lhs[0] + rhs, lhs[1] + rhs, lhs[2] + rhs, lhs[3] + rhs);
}

YAGE_FORCEINLINE auto operator+(float lhs, const Matrix4 &rhs) noexcept -> Matrix4 {
    return Matrix4(lhs + rhs[0], lhs + rhs[1], lhs + rhs[2], lhs + rhs[3]);
}

YAGE_FORCEINLINE auto operator-(const Matrix4 &lhs, const Matrix4 &rhs) noexcept -> Matrix4 {
    return Matrix4(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]);
}

YAGE_FORCEINLINE auto operator-(const Matrix4 &lhs, float rhs) noexcept -> Matrix4 {
    return Matrix4(lhs[0] - rhs, lhs[1] - rhs, lhs[2] - rhs, lhs[3] - rhs);
}

YAGE_FORCEINLINE auto operator*(const Matrix4 &lhs, const Matrix4 &rhs) noexcept -> Matrix4 {
    return DirectX::XMMatrixMultiply(lhs._xmmat, rhs._xmmat);
}

YAGE_FORCEINLINE auto operator*(const Matrix4 &lhs, float rhs) noexcept -> Matrix4 {
    return Matrix4(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs);
}

YAGE_FORCEINLINE auto operator*(float lhs, const Matrix4 &rhs) noexcept -> Matrix4 {
    return Matrix4(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3]);
}

YAGE_FORCEINLINE auto operator*(const Matrix4 &lhs, Vector4 rhs) noexcept -> Vector4 {
    return Vector4(Dot(lhs[0], rhs), Dot(lhs[1], rhs), Dot(lhs[2], rhs), Dot(lhs[3], rhs));
}

YAGE_FORCEINLINE auto operator*(Vector4 lhs, const Matrix4 &rhs) noexcept -> Vector4 {
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
}

YAGE_FORCEINLINE auto operator*=(Vector4 &lhs, const Matrix4 &rhs) noexcept -> Vector4 & {
    lhs = lhs * rhs;
    return lhs;
}

YAGE_FORCEINLINE auto operator/(const Matrix4 &lhs, float rhs) noexcept -> Matrix4 {
    return Matrix4(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs);
}

} // namespace YaGE
