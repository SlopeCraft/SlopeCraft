#ifndef SLOPECRAFT_VISUALCRAFT_VISUALCRAFTL_GLOBAL_H
#define SLOPECRAFT_VISUALCRAFT_VISUALCRAFTL_GLOBAL_H

// #ifdef VISUALCRAFTL_BUILD

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) ||  \
    defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || \
    defined(__WIN32__) || defined(__NT__)
#define Q_DECL_EXPORT __declspec(dllexport)
#define Q_DECL_IMPORT __declspec(dllimport)
#else
#define Q_DECL_EXPORT __attribute__((visibility("default")))
#define Q_DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(VISUALCRAFTL_BUILD)
#define VCL_EXPORT Q_DECL_EXPORT
#else
#define VCL_EXPORT Q_DECL_IMPORT
#endif

#ifdef _WIN32
#define VCL_EXPORT_FUN __stdcall VCL_EXPORT
#else
#define VCL_EXPORT_FUN VCL_EXPORT
#endif

#endif  // SLOPECRAFT_VISUALCRAFT_VISUALCRAFTL_GLOBAL_H