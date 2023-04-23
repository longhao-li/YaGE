#ifndef YAGE_CORE_HASH_H
#define YAGE_CORE_HASH_H

#include "Common.h"

#include <cstddef>
#include <cstdint>

namespace YaGE {

/// @brief
///   Calculate hash value of a range of data.
/// @remarks
///   xxhash32 algorithm is used here.
///
/// @param[in] data		Pointer to start of data to be hashed.
/// @param	   size		Size in byte of data to be hashed.
/// @param	   seed		Seed of hash algorithm. Hashing the same data with different seed value will generate different result.
///
/// @return uint32_t
///   Return hash value of the specified data.
YAGE_NODISCARD YAGE_API auto Hash32(const void *data, size_t size, uint32_t seed = 0) noexcept -> uint32_t;

/// @brief
///   Calculate hash value of a range of data.
/// @remarks
///   xxhash64 algorithm is used here.
///
/// @param[in] data		Pointer to start of data to be hashed.
/// @param	   size		Size in byte of data to be hashed.
/// @param	   seed		Seed of hash algorithm. Hashing the same data with different seed value will generate different result.
///
/// @return uint64_t
///   Return hash value of the specified data.
YAGE_NODISCARD YAGE_API auto Hash64(const void *data, size_t size, uint64_t seed = 0) noexcept -> uint64_t;

/// @brief
///   Calculate hash value of a range of data.
///
/// @param[in] data		Pointer to start of data to be hashed.
/// @param	   size		Size in byte of data to be hashed.
/// @param	   seed		Seed of hash algorithm. Hashing the same data with different seed value will generate different result.
///
/// @return size_t
///   Return hash value of the specified data.
YAGE_NODISCARD YAGE_FORCEINLINE auto Hash(const void *data, size_t size, size_t seed = 0) noexcept -> size_t {
#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 4127)
#endif
    if YAGE_IF_CONSTEXPR (sizeof(size_t) >= sizeof(uint64_t))
        return Hash64(data, size, static_cast<uint64_t>(seed));
    else
        return Hash32(data, size, static_cast<uint32_t>(seed));
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif
}

} // namespace YaGE

#endif // YAGE_CORE_HASH_H
