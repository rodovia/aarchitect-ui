#pragma once

/* CXX_EXTERNAL_* */
#ifdef __cplusplus
#   define CXX_EXTERNAL_START extern "C" {
#else
#   define CXX_EXTERNAL_START
#endif
#ifdef __cplusplus
#   define CXX_EXTERNAL_END }
#else
#   define CXX_EXTERNAL_END
#endif

/* DEPRECATE/DEPRECATEEX */
#if defined(__GNUC__)
#   define DEPRECATE(S) __attribute__ ((deprecated)) S 
#   define DEPRECATEEX(S, M) __attribute__ ((deprecated (M))) S 
#elif defined(_MSC_VER)
#   define DEPRECATE(S) __declspec(deprecated) S
#   define DEPRECATEEX(S, M) __declspec(deprecated(M)) S
#endif

/* DLL_PUBLIC/DLL_LOCAL */
#if defined(_WIN32) || defined(__CYGWIN__)
#   if defined(BUILDING_DLL)
#       ifdef __GNUC__
#           define DLL_PUBLIC __attribute__ ((dllexport))
#       else /* !__GNUC__ */
#           define DLL_PUBLIC __declspec(dllexport)
#       endif
#   elif defined(IMPORTING_DLL)
#       ifdef __GNUC__
#           define DLL_PUBLIC __attribute__ ((dllimport))
#       else /* !__GNUC__ */
#           define DLL_PUBLIC __declspec(dllimport)
#       endif
#   else /* !defined(BUILDING_DLL) && !defined(IMPORTING_DLL) */
#       define DLL_PUBLIC
#   endif
#   define DLL_LOCAL
#else /* !defined(_WIN32) && !defined(__CYGWIN__) */
#   if __GNUC__ >= 4
#       define DLL_PUBLIC __attribute__ ((visibility ("default")))
#       define DLL_LOCAL __attribute__ ((visibility ("hidden")))
#   else
#       define DLL_PUBLIC
#       define DLL_LOCAL
#   endif
#endif
