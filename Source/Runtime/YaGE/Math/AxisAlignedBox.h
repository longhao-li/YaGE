#pragma once

#include "Vector3.h"

#include <cfloat>

namespace YaGE {

struct AxisAlignedBox {
    Vector3 minCorner;
    Vector3 maxCorner;

    /// @brief
    ///   Create a new axis-aligned box with zero size.
    YAGE_FORCEINLINE AxisAlignedBox() noexcept : minCorner(FLT_MAX), maxCorner(-FLT_MAX) {}

    /// @brief
    ///   Create a new axis-aligned box with the given minimum and maximum corners.
    ///
    /// @param minCorner    The minimum corner of the axis-aligned box.
    /// @param maxCorner    The maximum corner of the axis-aligned box.
    YAGE_FORCEINLINE AxisAlignedBox(Vector3 minCorner, Vector3 maxCorner) noexcept
        : minCorner(minCorner), maxCorner(maxCorner) {}

    /// @brief
    ///   Create a new axis-aligned box with minimum possible size that could cover all of the given points.
    ///
    /// @param count    The number of points to be convered.
    /// @param points   The points to be convered.
    YAGE_FORCEINLINE AxisAlignedBox(size_t count, const Vector3 *points) noexcept
        : minCorner(FLT_MAX), maxCorner(-FLT_MAX) {
        const Vector3 *lastPoint = points + count;
        for (const Vector3 *point = points; point != lastPoint; ++point) {
            minCorner = Min(minCorner, *point);
            maxCorner = Max(maxCorner, *point);
        }
    }

    /// @brief
    ///   Check if the axis-aligned box covers the given point.
    ///
    /// @param point    The point to be checked.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Covers(Vector3 point) const noexcept -> bool {
        return minCorner.x <= point.x && point.x <= maxCorner.x && minCorner.y <= point.y && point.y <= maxCorner.y &&
               minCorner.z <= point.z && point.z <= maxCorner.z;
    }

    /// @brief
    ///   Check if this axis-aligned box covers the given axis-aligned box.
    ///
    /// @param box    The axis-aligned box to be checked.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Covers(AxisAlignedBox box) const noexcept -> bool {
        return minCorner.x <= box.minCorner.x && box.maxCorner.x <= maxCorner.x && minCorner.y <= box.minCorner.y &&
               box.maxCorner.y <= maxCorner.y && minCorner.z <= box.minCorner.z && box.maxCorner.z <= maxCorner.z;
    }

    /// @brief
    ///   Get center of this axis-aligned box.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Center() const noexcept -> Vector3 { return (minCorner + maxCorner) * 0.5f; }

    /// @brief
    ///   Merge the given point into this axis-aligned box.
    ///
    /// @param point    The point to be merged.
    YAGE_FORCEINLINE auto Merge(Vector3 point) noexcept -> void {
        minCorner = Min(minCorner, point);
        maxCorner = Max(maxCorner, point);
    }

    /// @brief
    ///   Merge the given axis-aligned box into this axis-aligned box.
    ///
    /// @param box    The axis-aligned box to be merged.
    YAGE_FORCEINLINE auto Merge(AxisAlignedBox box) noexcept -> void {
        minCorner = Min(minCorner, box.minCorner);
        maxCorner = Max(maxCorner, box.maxCorner);
    }
};

} // namespace YaGE
