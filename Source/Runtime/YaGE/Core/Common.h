#ifndef YAGE_CORE_COMMON_H
#define YAGE_CORE_COMMON_H

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

#if defined(_MSC_VER) && (_MSC_VER >= 1700)
#    define YAGE_NODISCARD _Check_return_
#elif defined(__clang__) || defined(__GNUC__)
#    define YAGE_NODISCARD __attribute__((warn_unused_result))
#elif YAGE_CPLUSPLUS >= 201703L
#    define YAGE_NODISCARD [[nodiscard]]
#else
#    define YAGE_NODISCARD
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1700)
#    define YAGE_FALLTHROUGH __fallthrough
#elif defined(__clang__) || defined(__GNUC__)
#    define YAGE_FALLTHROUGH __attribute__((fallthrough))
#elif YAGE_CPLUSPLUS >= 201703L
#    define YAGE_FALLTHROUGH [[fallthrough]]
#else
#    define YAGE_FALLTHROUGH
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1700)
#    define YAGE_DEPRECATED __declspec(deprecated)
#elif defined(__clang__) || defined(__GNUC__)
#    define YAGE_DEPRECATED __attribute__((deprecated))
#elif YAGE_CPLUSPLUS >= 201402L
#    define YAGE_DEPRECATED [[deprecated]]
#else
#    define YAGE_DEPRECATED
#endif

#if defined(_MSC_VER)
#    define YAGE_FORCEINLINE __forceinline
#elif defined(__clang__) || defined(__GNUC__)
#    define YAGE_FORCEINLINE __attribute__((always_inline)) inline
#else
#    define YAGE_FORCEINLINE inline
#endif

#endif // YAGE_CORE_COMMON_H
