#ifndef CVD_CONFIG_H
#define CVD_CONFIG_H
#define CVD_ARCH_LITTLE_ENDIAN 1
#ifndef CVD_DISABLE_CPU_arm
    #define CVD_HAVE_CPU_arm 1
#endif
#ifndef CVD_DISABLE_GLOB
    #define CVD_HAVE_GLOB 1
#endif
#ifndef CVD_DISABLE_INLINE_ASM
    #define CVD_HAVE_INLINE_ASM 1
#endif
#ifndef CVD_DISABLE_MEMALIGN
    #define CVD_HAVE_MEMALIGN 1
#endif
#ifndef CVD_DISABLE_PTHREAD
    #define CVD_HAVE_PTHREAD 1
#endif
#ifndef CVD_DISABLE_PTHREAD_YIELD_NP
    #define CVD_HAVE_PTHREAD_YIELD_NP 1
#endif
#ifndef CVD_DISABLE_TOON
    #define CVD_HAVE_TOON 1
#endif
#ifndef CVD_DISABLE_TR1_SHARED_PTR
    #define CVD_HAVE_TR1_SHARED_PTR 1
#endif
#define CVD_MAJOR_VERSION 0
#define CVD_MINOR_VERSION 8
#endif
