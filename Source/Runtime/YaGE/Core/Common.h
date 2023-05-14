#pragma once

#ifdef YAGE_DLL
#    ifdef YAGE_BUILD_DLL
#        define YAGE_API __declspec(dllexport)
#    else
#        define YAGE_API __declspec(dllimport)
#    endif
#else
#    define YAGE_API
#endif

#if defined(_MSC_VER)
#    define YAGE_CPLUSPLUS _MSVC_LANG
#else
#    define YAGE_CPLUSPLUS __cplusplus
#endif

#if YAGE_CPLUSPLUS >= 201703L
#    define YAGE_CONSTEXPR17 constexpr
#    define YAGE_IF_CONSTEXPR constexpr
#else
#    define YAGE_CONSTEXPR17 inline
#    define YAGE_IF_CONSTEXPR
#endif

#if YAGE_CPLUSPLUS >= 202002L
#    define YAGE_CONSTEXPR20 constexpr
#else
#    define YAGE_CONSTEXPR20 inline
#endif

#if defined(__clang__) || defined(__GNUC__)
#    define YAGE_NODISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#    define YAGE_NODISCARD _Check_return_
#elif YAGE_CPLUSPLUS >= 201703L
#    define YAGE_NODISCARD [[nodiscard]]
#else
#    define YAGE_NODISCARD
#endif

#if defined(__clang__) || defined(__GNUC__)
#    define YAGE_FALLTHROUGH __attribute__((fallthrough))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#    define YAGE_FALLTHROUGH __fallthrough
#elif YAGE_CPLUSPLUS >= 201703L
#    define YAGE_FALLTHROUGH [[fallthrough]]
#else
#    define YAGE_FALLTHROUGH
#endif

#if defined(__clang__) || defined(__GNUC__)
#    define YAGE_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#    define YAGE_DEPRECATED __declspec(deprecated)
#elif YAGE_CPLUSPLUS >= 201402L
#    define YAGE_DEPRECATED [[deprecated]]
#else
#    define YAGE_DEPRECATED
#endif

#if defined(__clang__) || defined(__GNUC__)
#    define YAGE_FORCEINLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#    define YAGE_FORCEINLINE __forceinline
#else
#    define YAGE_FORCEINLINE inline
#endif

#if defined(__clang__) || defined(__GNUC__)
#    define YAGE_ALIGNAS(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#    define YAGE_ALIGNAS(x) __declspec(align(x))
#else
#    define YAGE_ALIGNAS(x) alignas(x)
#endif
