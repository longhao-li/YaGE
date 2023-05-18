#pragma once

#include "Vector2.h"

namespace YaGE {

struct Vector3 {
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
        float   _arr[3];
        Vector2 xy;
        struct {
            float x;
            union {
                Vector2 yz;
                struct {
                    float y;
                    float z;
                };
            };
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
    ///   Create a 3D vector and initialize with 0.
    YAGE_FORCEINLINE Vector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}

    /// @brief
    ///   Create a 3D vector and initialize with the specified value.
    ///
    /// @param v    The value to initialize all elements of the vector with.
    YAGE_FORCEINLINE explicit Vector3(float v) noexcept : x(v), y(v), z(v) {}

    /// @brief
    ///   Create a 3D vector and initialize with specified values.
    ///
    /// @param x    The x component of the vector.
    /// @param y    The y component of the vector.
    /// @param z    The z component of the vector.
    YAGE_FORCEINLINE Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

    /// @brief
    ///   Create a 3D vector and initialize with specified values.
    ///
    /// @param arr    The array of values to initialize the vector with.
    YAGE_FORCEINLINE explicit Vector3(const float arr[3]) noexcept : x(arr[0]), y(arr[1]), z(arr[2]) {}

    /// @brief
    ///   Create a 3D vector and initialize with specified values.
    ///
    /// @param xy   The 2D vector to initialize the vector with.
    /// @param z    The z component of the vector.
    YAGE_FORCEINLINE Vector3(Vector2 xy, float z) noexcept : x(xy.x), y(xy.y), z(z) {}

    /// @brief
    ///   Create a 3D vector and initialize with specified values.
    ///
    /// @param x    The x component of the vector.
    /// @param yz   The 2D vector to initialize the vector with.
    YAGE_FORCEINLINE Vector3(float x, Vector2 yz) noexcept : x(x), y(yz.x), z(yz.y) {}

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
    ///   Return length of this 3D vector.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Length() const noexcept -> float { return std::sqrt(x * x + y * y + z * z); }

    /// @brief
    ///   Normalize this vector.
    /// @note
    ///   To get normalized vector of this vector without modifying this vector, use @p Normalized() instead.
    ///
    /// @return Vector3 &
    ///   Return reference to this vector.
    YAGE_FORCEINLINE auto Normalize() noexcept -> Vector3 & {
        const float length    = Length();
        const float invLength = 1.0f / length;

        x *= invLength;
        y *= invLength;
        z *= invLength;

        return *this;
    }

    /// @brief
    ///   Get normalized vector of this vector.
    ///
    /// @return Vector3
    ///   Return normalized vector of this vector.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Normalized() const noexcept -> Vector3 {
        const float length    = Length();
        const float invLength = 1.0f / length;

        return Vector3(x * invLength, y * invLength, z * invLength);
    }

    YAGE_NODISCARD YAGE_FORCEINLINE auto operator+() const noexcept -> Vector3 { return Vector3(x, y, z); }
    YAGE_NODISCARD YAGE_FORCEINLINE auto operator-() const noexcept -> Vector3 { return Vector3(-x, -y, -z); }

    YAGE_FORCEINLINE auto operator+=(float rhs) noexcept -> Vector3 & {
        x += rhs;
        y += rhs;
        z += rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator+=(Vector3 rhs) noexcept -> Vector3 & {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(float rhs) noexcept -> Vector3 & {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(Vector3 rhs) noexcept -> Vector3 & {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(float rhs) noexcept -> Vector3 & {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(Vector3 rhs) noexcept -> Vector3 & {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    YAGE_FORCEINLINE auto operator/=(float rhs) noexcept -> Vector3 & {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator/=(Vector3 rhs) noexcept -> Vector3 & {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }
};

YAGE_NODISCARD YAGE_FORCEINLINE auto operator==(Vector3 lhs, Vector3 rhs) noexcept -> bool {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

YAGE_NODISCARD YAGE_FORCEINLINE auto operator!=(Vector3 lhs, Vector3 rhs) noexcept -> bool {
    return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
}

YAGE_FORCEINLINE auto operator+(Vector3 lhs, Vector3 rhs) noexcept -> Vector3 {
    return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

YAGE_FORCEINLINE auto operator+(Vector3 lhs, float rhs) noexcept -> Vector3 {
    return Vector3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
}

YAGE_FORCEINLINE auto operator+(float lhs, Vector3 rhs) noexcept -> Vector3 {
    return Vector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
}

YAGE_FORCEINLINE auto operator-(Vector3 lhs, Vector3 rhs) noexcept -> Vector3 {
    return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

YAGE_FORCEINLINE auto operator-(Vector3 lhs, float rhs) noexcept -> Vector3 {
    return Vector3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
}

YAGE_FORCEINLINE auto operator*(Vector3 lhs, Vector3 rhs) noexcept -> Vector3 {
    return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

YAGE_FORCEINLINE auto operator*(Vector3 lhs, float rhs) noexcept -> Vector3 {
    return Vector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

YAGE_FORCEINLINE auto operator*(float lhs, Vector3 rhs) noexcept -> Vector3 {
    return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

YAGE_FORCEINLINE auto operator/(Vector3 lhs, Vector3 rhs) noexcept -> Vector3 {
    return Vector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

YAGE_FORCEINLINE auto operator/(Vector3 lhs, float rhs) noexcept -> Vector3 {
    return Vector3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Dot(Vector3 lhs, Vector3 rhs) noexcept -> float {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Cross(Vector3 lhs, Vector3 rhs) noexcept -> Vector3 {
    return Vector3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Lerp(Vector3 start, Vector3 end, float t) noexcept -> Vector3 {
    return start + (end - start) * t;
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Abs(Vector3 vec) noexcept -> Vector3 {
    return Vector3{
        vec.x < 0 ? -vec.x : vec.x,
        vec.y < 0 ? -vec.y : vec.y,
        vec.z < 0 ? -vec.z : vec.z,
    };
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Min(Vector3 lhs, Vector3 rhs) noexcept -> Vector3 {
    return Vector3{
        lhs.x < rhs.x ? lhs.x : rhs.x,
        lhs.y < rhs.y ? lhs.y : rhs.y,
        lhs.z < rhs.z ? lhs.z : rhs.z,
    };
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Max(Vector3 lhs, Vector3 rhs) noexcept -> Vector3 {
    return Vector3{
        lhs.x < rhs.x ? rhs.x : lhs.x,
        lhs.y < rhs.y ? rhs.y : lhs.y,
        lhs.z < rhs.z ? rhs.z : lhs.z,
    };
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Clamp(Vector3 vec, Vector3 floor, Vector3 ceil) noexcept -> Vector3 {
    return Vector3{
        vec.x < floor.x ? floor.x : (vec.x > ceil.x ? ceil.x : vec.x),
        vec.y < floor.y ? floor.y : (vec.y > ceil.y ? ceil.y : vec.y),
        vec.z < floor.z ? floor.z : (vec.z > ceil.z ? ceil.z : vec.z),
    };
}

} // namespace YaGE
