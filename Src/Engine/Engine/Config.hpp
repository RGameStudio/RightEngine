#pragma once

// Platform-specific macros for importing/exporting symbols
#if defined(R_OS_WIN32)

#ifdef ENGINE_EXPORT
    // We are building the library, so export the symbols
#define ENGINE_API __declspec(dllexport)
#else
    // We are using the library, so import the symbols
#define ENGINE_API __declspec(dllimport)
#endif

#else
#define ENGINE_API
#endif

#if defined(R_OS_WIN32)
    #define ENGINE_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define ENGINE_FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define ENGINE_FORCE_INLINE inline
#endif

