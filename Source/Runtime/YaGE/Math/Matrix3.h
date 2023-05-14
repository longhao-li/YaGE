#pragma once

#include "Vector3.h"

namespace YaGE {

struct Matrix3 {
    Vector3 data[3];

    /// @brief
    ///   Initialize this matrix with 0.
    YAGE_FORCEINLINE Matrix3() noexcept : data() {}

    /// @brief
    ///   Create a diagonal matrix with the specified values.
    ///
    /// @param v00  The value at (0, 0).
    /// @param v11  The value at (1, 1).
    /// @param v22  The value at (2, 2).
    YAGE_FORCEINLINE explicit Matrix3(float v00, float v11, float v22) noexcept
        : data{Vector3(v00, 0.0f, 0.0f), Vector3(0.0f, v11, 0.0f), Vector3(0.0f, 0.0f, v22)} {}

    /// @brief
    ///   Create a diagonal matrix with the specified value.
    ///
    /// @param value  The value at (0, 0), (1, 1) and (2, 2).
    YAGE_FORCEINLINE explicit Matrix3(float value) noexcept : Matrix3(value, value, value) {}

    // clang-format off
    /// @brief
    ///   Create a matrix with the specified values.
    ///
    /// @param v00  The value at (0, 0).
    /// @param v01  The value at (0, 1).
    /// @param v02  The value at (0, 2).
    /// @param v10  The value at (1, 0).
    /// @param v11  The value at (1, 1).
    /// @param v12  The value at (1, 2).
    /// @param v20  The value at (2, 0).
    /// @param v21  The value at (2, 1).
    /// @param v22  The value at (2, 2).
    YAGE_FORCEINLINE Matrix3(float v00, float v01, float v02,
                             float v10, float v11, float v12,
                             float v20, float v21, float v22) noexcept
        : data{Vector3(v00, v01, v02), Vector3(v10, v11, v12), Vector3(v20, v21, v22)} {}
    // clang-format on

    /// @brief
    ///   Create a matrix with the specified values.
    ///
    /// @param row0  The values at row 0.
    /// @param row1  The values at row 1.
    /// @param row2  The values at row 2.
    YAGE_FORCEINLINE Matrix3(const Vector3 &row0, const Vector3 &row1, const Vector3 &row2) noexcept
        : data{row0, row1, row2} {}

    /// @brief
    ///   Create a matrix with the specified values.
    ///
    /// @param arr  The array of values to initialize the matrix with.
    YAGE_FORCEINLINE explicit Matrix3(const float arr[9]) noexcept
        : data{Vector3(arr[0], arr[1], arr[2]), Vector3(arr[3], arr[4], arr[5]), Vector3(arr[6], arr[7], arr[8])} {}

    /// @brief
    ///   Random access elements of this matrix by index.
    ///
    /// @param i  The index of the element to access.
    YAGE_NODISCARD YAGE_FORCEINLINE auto operator[](size_t i) noexcept -> Vector3 & { return data[i]; }

    /// @brief
    ///   Random access elements of this matrix by index.
    ///
    /// @param i  The index of the element to access.
    YAGE_NODISCARD YAGE_FORCEINLINE auto operator[](size_t i) const noexcept -> const Vector3 & { return data[i]; }

    /// @brief
    ///   Calculate determinant of this matrix.
    ///
    /// @return float
    ///   Return the determinant of this matrix.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Determinant() const noexcept -> float {
        return data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1]) -
               data[0][1] * (data[1][0] * data[2][2] - data[1][2] * data[2][0]) +
               data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
    }

    /// @brief
    ///   Transpose this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a transposed matrix, use Transposed() instead.
    ///
    /// @return Matrix3 &
    ///   Return reference to this matrix after transposing.
    YAGE_FORCEINLINE auto Transpose() noexcept -> Matrix3 & {
        float temp = data[0][1];
        data[0][1] = data[1][0];
        data[1][0] = temp;

        temp       = data[0][2];
        data[0][2] = data[2][0];
        data[2][0] = temp;

        temp       = data[1][2];
        data[1][2] = data[2][1];
        data[2][1] = temp;
        return *this;
    }

    /// @brief
    ///   Get a transposed matrix of this matrix.
    ///
    /// @return Matrix3
    ///   Return a transposed matrix of this matrix.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Transposed() const noexcept -> Matrix3 {
        return Matrix3(data[0][0], data[1][0], data[2][0], data[0][1], data[1][1], data[2][1], data[0][2], data[1][2],
                       data[2][2]);
    }

    /// @brief
    ///   Inverse this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a inversed matrix, use Inversed() instead.
    ///
    /// @return Matrix3 &
    ///   Return reference to this matrix after inverting.
    YAGE_FORCEINLINE auto Inverse() noexcept -> Matrix3 & {
        const float det = Determinant();
        if (det == 0.0f)
            return *this;

        Matrix3 temp{
            data[1][1] * data[2][2] - data[1][2] * data[2][1], data[0][2] * data[2][1] - data[0][1] * data[2][2],
            data[0][1] * data[1][2] - data[0][2] * data[1][1], data[1][2] * data[2][0] - data[1][0] * data[2][2],
            data[0][0] * data[2][2] - data[0][2] * data[2][0], data[0][2] * data[1][0] - data[0][0] * data[1][2],
            data[1][0] * data[2][1] - data[1][1] * data[2][0], data[0][1] * data[2][0] - data[0][0] * data[2][1],
            data[0][0] * data[1][1] - data[0][1] * data[1][0],
        };

        const float invDet = 1.0f / det;

        data[0] = temp[0] * invDet;
        data[1] = temp[1] * invDet;
        data[2] = temp[2] * invDet;

        return *this;
    }

    /// @brief
    ///   Get a inversed matrix of this matrix.
    ///
    /// @return Matrix3
    ///   Return a inversed matrix of this matrix.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Inversed() const noexcept -> Matrix3 {
        const float det = Determinant();
        if (det == 0.0f)
            return *this;

        Matrix3 temp{
            data[1][1] * data[2][2] - data[1][2] * data[2][1], data[0][2] * data[2][1] - data[0][1] * data[2][2],
            data[0][1] * data[1][2] - data[0][2] * data[1][1], data[1][2] * data[2][0] - data[1][0] * data[2][2],
            data[0][0] * data[2][2] - data[0][2] * data[2][0], data[0][2] * data[1][0] - data[0][0] * data[1][2],
            data[1][0] * data[2][1] - data[1][1] * data[2][0], data[0][1] * data[2][0] - data[0][0] * data[2][1],
            data[0][0] * data[1][1] - data[0][1] * data[1][0],
        };

        const float invDet = 1.0f / det;

        temp[0] *= invDet;
        temp[1] *= invDet;
        temp[2] *= invDet;

        return temp;
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
    /// @return Matrix3 &
    ///   Return reference to this matrix after scaling.
    YAGE_FORCEINLINE auto Scale(float x, float y, float z) noexcept -> Matrix3 & {
        Vector3 scale(x, y, z);
        data[0] *= scale;
        data[1] *= scale;
        data[2] *= scale;
        return *this;
    }

    /// @brief
    ///   Apply scale transform to this matrix.
    /// @note
    ///   This function will change the matrix itself. If you want to get a scaled matrix, use Scaled() instead.
    ///
    /// @param scale    Scale factors of each dimension.
    ///
    /// @return Matrix3 &
    ///   Return reference to this matrix after scaling.
    YAGE_FORCEINLINE auto Scale(Vector3 scale) noexcept -> Matrix3 & {
        data[0] *= scale;
        data[1] *= scale;
        data[2] *= scale;
        return *this;
    }

    /// @brief
    ///   Get the matrix that is applied by the specified scale factors.
    ///
    /// @param x    X scale factor.
    /// @param y    Y scale factor.
    /// @param z    Z scale factor.
    ///
    /// @return Matrix3
    ///   Return a matrix that is applied by the specified scale factors.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Scaled(float x, float y, float z) const noexcept -> Matrix3 {
        Vector3 scale(x, y, z);
        return Matrix3(data[0] * scale, data[1] * scale, data[2] * scale);
    }

    /// @brief
    ///   Get the matrix that is applied by the specified scale factors.
    ///
    /// @param scale    Scale factors of each dimension.
    ///
    /// @return Matrix3
    ///   Return a matrix that is applied by the specified scale factors.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Scaled(Vector3 scale) const noexcept -> Matrix3 {
        return Matrix3(data[0] * scale, data[1] * scale, data[2] * scale);
    }

    YAGE_FORCEINLINE auto operator+() const noexcept -> Matrix3 { return *this; }
    YAGE_FORCEINLINE auto operator-() const noexcept -> Matrix3 { return Matrix3(-data[0], -data[1], -data[2]); }

    YAGE_FORCEINLINE auto operator+=(float rhs) noexcept -> Matrix3 & {
        data[0] += rhs;
        data[1] += rhs;
        data[2] += rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator+=(const Matrix3 &rhs) noexcept -> Matrix3 & {
        data[0] += rhs[0];
        data[1] += rhs[1];
        data[2] += rhs[2];
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(float rhs) noexcept -> Matrix3 & {
        data[0] -= rhs;
        data[1] -= rhs;
        data[2] -= rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(const Matrix3 &rhs) noexcept -> Matrix3 & {
        data[0] -= rhs[0];
        data[1] -= rhs[1];
        data[2] -= rhs[2];
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(float rhs) noexcept -> Matrix3 & {
        data[0] *= rhs;
        data[1] *= rhs;
        data[2] *= rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(const Matrix3 &rhs) noexcept -> Matrix3 & {
        Vector3 r0(data[0][0] * rhs[0] + data[0][1] * rhs[1] + data[0][2] * rhs[2]);
        Vector3 r1(data[1][0] * rhs[0] + data[1][1] * rhs[1] + data[1][2] * rhs[2]);
        Vector3 r2(data[2][0] * rhs[0] + data[2][1] * rhs[1] + data[2][2] * rhs[2]);

        data[0] = r0;
        data[1] = r1;
        data[2] = r2;

        return *this;
    }

    YAGE_FORCEINLINE auto operator/=(float rhs) noexcept -> Matrix3 & {
        data[0] /= rhs;
        data[1] /= rhs;
        data[2] /= rhs;
        return *this;
    }
};

YAGE_NODISCARD YAGE_FORCEINLINE auto operator==(const Matrix3 &lhs, const Matrix3 &rhs) noexcept -> bool {
    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
}

YAGE_NODISCARD YAGE_FORCEINLINE auto operator!=(const Matrix3 &lhs, const Matrix3 &rhs) noexcept -> bool {
    return !(lhs == rhs);
}

YAGE_FORCEINLINE auto operator+(const Matrix3 &lhs, const Matrix3 &rhs) noexcept -> Matrix3 {
    return Matrix3(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

YAGE_FORCEINLINE auto operator+(const Matrix3 &lhs, float rhs) noexcept -> Matrix3 {
    return Matrix3(lhs[0] + rhs, lhs[1] + rhs, lhs[2] + rhs);
}

YAGE_FORCEINLINE auto operator+(float lhs, const Matrix3 &rhs) noexcept -> Matrix3 {
    return Matrix3(lhs + rhs[0], lhs + rhs[1], lhs + rhs[2]);
}

YAGE_FORCEINLINE auto operator-(const Matrix3 &lhs, const Matrix3 &rhs) noexcept -> Matrix3 {
    return Matrix3(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]);
}

YAGE_FORCEINLINE auto operator-(const Matrix3 &lhs, float rhs) noexcept -> Matrix3 {
    return Matrix3(lhs[0] - rhs, lhs[1] - rhs, lhs[2] - rhs);
}

YAGE_FORCEINLINE auto operator*(const Matrix3 &lhs, const Matrix3 &rhs) noexcept -> Matrix3 {
    Vector3 r0(lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2]);
    Vector3 r1(lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2]);
    Vector3 r2(lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2]);

    return Matrix3(r0, r1, r2);
}

YAGE_FORCEINLINE auto operator*(const Matrix3 &lhs, float rhs) noexcept -> Matrix3 {
    return Matrix3(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs);
}

YAGE_FORCEINLINE auto operator*(float lhs, const Matrix3 &rhs) noexcept -> Matrix3 {
    return Matrix3(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]);
}

YAGE_FORCEINLINE auto operator*(const Matrix3 &lhs, Vector3 rhs) noexcept -> Vector3 {
    return Vector3(lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2],
                   lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2],
                   lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2]);
}

YAGE_FORCEINLINE auto operator*(Vector3 lhs, const Matrix3 &rhs) noexcept -> Vector3 {
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

YAGE_FORCEINLINE auto operator*=(Vector3 &lhs, const Matrix3 &rhs) noexcept -> Vector3 {
    lhs = lhs * rhs;
    return lhs;
}

YAGE_FORCEINLINE auto operator/(const Matrix3 &lhs, float rhs) noexcept -> Matrix3 {
    return Matrix3(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs);
}

} // namespace YaGE
