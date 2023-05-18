#pragma once

#include "Vector3.h"

#include <cstddef>

namespace YaGE {

struct BoundingSphere {
    Vector3 center;
    float   radius;

    /// @brief
    ///   Create a new bounding sphere with zero radius.
    YAGE_FORCEINLINE BoundingSphere() noexcept : center(), radius() {}

    /// @brief
    ///   Create a new bounding sphere with the given center and radius.
    ///
    /// @param center   The center of the bounding sphere.
    /// @param radius   The radius of the bounding sphere.
    YAGE_FORCEINLINE BoundingSphere(Vector3 center, float radius) noexcept : center(center), radius(radius) {}

    /// @brief
    ///   Create a new bounding sphere with minimum possible radius that could cover all of the given points.
    ///
    /// @param count    The number of points to be convered.
    /// @param points   The points to be convered.
    YAGE_FORCEINLINE BoundingSphere(size_t count, const Vector3 *points) noexcept {
        const Vector3 *lastPoint = points + count;
        for (const Vector3 *point = points; point != lastPoint; ++point)
            center += *point;

        center /= static_cast<float>(count);
        for (const Vector3 *point = points; point != lastPoint; ++point) {
            Vector3 diff = (*point - center);
            float   dist = Dot(diff, diff);
            radius       = (dist > radius) ? dist : radius;
        }

        radius = sqrtf(radius);
    }

    /// @brief
    ///   Check if the bounding sphere covers the given point.
    ///
    /// @param point    The point to be checked.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Covers(Vector3 point) const noexcept -> bool {
        Vector3 diff = point - center;
        return Dot(diff, diff) <= radius * radius;
    }

    /// @brief
    ///   Check if this bounding sphere covers the given bounding sphere.
    ///
    /// @param sphere   The bounding sphere to be checked.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Covers(BoundingSphere sphere) const noexcept -> bool {
        Vector3 diff = sphere.center - center;
        float   dist = diff.Length();
        return dist + sphere.radius <= radius;
    }

    /// @brief
    ///   Merge 2 bounding spheres into this bounding sphere.
    ///
    /// @param sphere   The bounding sphere to be merged.
    YAGE_FORCEINLINE auto Merge(BoundingSphere sphere) noexcept -> void {
        Vector3 diff = sphere.center - center;
        float   dist = diff.Length();

        // This bounding sphere covers the given bounding sphere.
        if (dist + sphere.radius <= radius)
            return;

        // The given bounding sphere covers this bounding sphere.
        if (dist + radius <= sphere.radius) {
            center = sphere.center;
            radius = sphere.radius;
            return;
        }

        // The two bounding spheres intersect.
        radius      = (dist + radius + sphere.radius) * 0.5f;
        float scale = radius / dist;
        center      = (center + sphere.center) * scale;
    }

    /// @brief
    ///   Merge a point into this bounding sphere.
    ///
    /// @param point    The point to be merged.
    YAGE_FORCEINLINE auto Merge(Vector3 point) noexcept -> void {
        Vector3 diff = point - center;
        float   dist = diff.Length();

        // The given point is inside this bounding sphere.
        if (dist <= radius)
            return;

        radius      = (dist + radius) * 0.5f;
        float scale = radius / dist;
        center      = (center + point) * scale;
    }
};

} // namespace YaGE
