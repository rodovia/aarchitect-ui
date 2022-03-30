#pragma once

#ifndef _AAR_API /* This is the only file that has a include guard since we want to know if
                    some random header from aarchitect folder is being included directly. */
#define _AAR_API 1

#if defined(__i386__)
#   if defined(_WIN32) && defined(_MSC_VER)
#       define AAR_CALLCONV __fastcall
#   elif defined(__GNUC__) && defined(_WIN32)
#       define AAR_CALLCONV __attribute__((fastcall)) __attribute__((ms_abi))
#   endif /* defined(_WIN32) && defined(_MSC_VER) */
#elif defined(__x86_64__)
#   if defined(_WIN32) && defined(_MSC_VER)
#       define AAR_CALLCONV __stdcall
#   elif defined(__GNUC__) && defined(_WIN32)
#       define AAR_CALLCONV __attribute__((stdcall))  __attribute__((ms_abi))
#   endif
#endif

#if defined(__GNUC__)
#   define AAR_ALWAYSINLINE __attribute__ ((always_inline))
#else
#   define AAR_ALWAYSINLINE /* MSVC */
#endif

#endif /* ifndef _AAR_API */