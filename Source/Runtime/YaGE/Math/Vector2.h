#pragma once

#include "../Core/Common.h"

#include <cmath>

namespace YaGE {

struct Vector2 {
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
        float _arr[2];
        struct {
            float x;
            float y;
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
    ///   Create a 2D vector and initialize with 0.
    YAGE_FORCEINLINE Vector2() noexcept : x(0.0f), y(0.0f) {}

    /// @brief
    ///   Create a 2D vector and initialize with the specified value.
    ///
    /// @param v    The value to initialize all elements of the vector with.
    YAGE_FORCEINLINE explicit Vector2(float v) noexcept : x(v), y(v) {}

    /// @brief
    ///   Create a 2D vector and initialize with specified values.
    ///
    /// @param x    The x component of the vector.
    /// @param y    The y component of the vector.
    YAGE_FORCEINLINE Vector2(float x, float y) noexcept : x(x), y(y) {}

    /// @brief
    ///   Create a 2D vector and initialize with specified values.
    ///
    /// @param arr    The array of values to initialize the vector with.
    YAGE_FORCEINLINE explicit Vector2(const float arr[2]) noexcept : x(arr[0]), y(arr[1]) {}

    /// @brief
    ///   Create a 2D vector and initialize with specified values.
    ///
    /// @param v    The vector to initialize the vector with.
    YAGE_FORCEINLINE Vector2(const Vector2 &v) noexcept : x(v.x), y(v.y) {}

    /// @brief
    ///   Create a 2D vector and initialize with specified values.
    ///
    /// @param v    The vector to initialize the vector with.
    YAGE_FORCEINLINE Vector2(Vector2 &&v) noexcept : x(v.x), y(v.y) {}

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
    ///   Return length of this 2D vector.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Length() const noexcept -> float { return std::sqrt(x * x + y * y); }

    /// @brief
    ///   Normalize this vector.
    /// @note
    ///   To get normalized vector of this vector without modifying this vector, use @p Normalized() instead.
    ///
    /// @return Vector2 &
    ///   Return reference to this vector.
    YAGE_FORCEINLINE auto Normalize() noexcept -> Vector2 & {
        const float length    = Length();
        const float invLength = 1.0f / length;

        x *= invLength;
        y *= invLength;

        return *this;
    }

    /// @brief
    ///   Get normalized vector of this vector.
    ///
    /// @return Vector2
    ///   Return normalized vector of this vector.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Normalized() const noexcept -> Vector2 {
        const float length    = Length();
        const float invLength = 1.0f / length;

        return Vector2(x * invLength, y * invLength);
    }

    YAGE_NODISCARD YAGE_FORCEINLINE auto operator+() const noexcept -> Vector2 { return Vector2(x, y); }
    YAGE_NODISCARD YAGE_FORCEINLINE auto operator-() const noexcept -> Vector2 { return Vector2(-x, -y); }

    YAGE_FORCEINLINE auto operator+=(float rhs) noexcept -> Vector2 & {
        x += rhs;
        y += rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator+=(Vector2 rhs) noexcept -> Vector2 & {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(float rhs) noexcept -> Vector2 & {
        x -= rhs;
        y -= rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator-=(Vector2 rhs) noexcept -> Vector2 & {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(float rhs) noexcept -> Vector2 & {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator*=(Vector2 rhs) noexcept -> Vector2 & {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    YAGE_FORCEINLINE auto operator/=(float rhs) noexcept -> Vector2 & {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    YAGE_FORCEINLINE auto operator/=(Vector2 rhs) noexcept -> Vector2 & {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }
};

YAGE_NODISCARD YAGE_FORCEINLINE auto operator==(Vector2 lhs, Vector2 rhs) noexcept -> bool {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

YAGE_NODISCARD YAGE_FORCEINLINE auto operator!=(Vector2 lhs, Vector2 rhs) noexcept -> bool {
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

YAGE_FORCEINLINE auto operator+(Vector2 lhs, Vector2 rhs) noexcept -> Vector2 {
    return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
}

YAGE_FORCEINLINE auto operator+(Vector2 lhs, float rhs) noexcept -> Vector2 {
    return Vector2(lhs.x + rhs, lhs.y + rhs);
}

YAGE_FORCEINLINE auto operator+(float lhs, Vector2 rhs) noexcept -> Vector2 {
    return Vector2(lhs + rhs.x, lhs + rhs.y);
}

YAGE_FORCEINLINE auto operator-(Vector2 lhs, Vector2 rhs) noexcept -> Vector2 {
    return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
}

YAGE_FORCEINLINE auto operator-(Vector2 lhs, float rhs) noexcept -> Vector2 {
    return Vector2(lhs.x - rhs, lhs.y - rhs);
}

YAGE_FORCEINLINE auto operator*(Vector2 lhs, Vector2 rhs) noexcept -> Vector2 {
    return Vector2(lhs.x * rhs.x, lhs.y * rhs.y);
}

YAGE_FORCEINLINE auto operator*(Vector2 lhs, float rhs) noexcept -> Vector2 {
    return Vector2(lhs.x * rhs, lhs.y * rhs);
}

YAGE_FORCEINLINE auto operator*(float lhs, Vector2 rhs) noexcept -> Vector2 {
    return Vector2(lhs * rhs.x, lhs * rhs.y);
}

YAGE_FORCEINLINE auto operator/(Vector2 lhs, Vector2 rhs) noexcept -> Vector2 {
    return Vector2(lhs.x / rhs.x, lhs.y / rhs.y);
}

YAGE_FORCEINLINE auto operator/(Vector2 lhs, float rhs) noexcept -> Vector2 {
    return Vector2(lhs.x / rhs, lhs.y / rhs);
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Dot(Vector2 lhs, Vector2 rhs) noexcept -> float {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

YAGE_NODISCARD YAGE_FORCEINLINE auto Lerp(Vector2 start, Vector2 end, float t) noexcept -> Vector2 {
    return start + (end - start) * t;
}

} // namespace YaGE
