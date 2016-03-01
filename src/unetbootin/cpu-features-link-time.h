#ifndef CPU_FEATURES_LINK_TIME_H
#define CPU_FEATURES_LINK_TIME_H

#if !defined(__i386__) && !defined(__x86_64__)

#error link time cpu feature detection not available

#endif


#include <stdint.h>
#include "cpu-features.h"

#ifdef __i386__
static inline __attribute__((always_inline)) void x86_cpuid(int func, uint32_t values[4])
{
    uint32_t a, b, c, d;
    /* We need to preserve ebx since we're compiling PIC code */
    /* this means we can't use "=b" for the second output register */
    __asm__ __volatile__ ( \
      "push %%ebx\n"
      "cpuid\n" \
      "mov %%ebx, %1\n"
      "pop %%ebx\n"
      : "=a" (a), "=r" (b), "=c" (c), "=d" (d) \
      : "a" (func) \
    );
    values[0] = a;
    values[1] = b;
    values[2] = c;
    values[3] = d;
}
static inline __attribute__((always_inline)) void x86_cpuid_round(int func, int round, uint32_t values[4])
{
    uint32_t a, b, c, d;
    /* We need to preserve ebx since we're compiling PIC code */
    /* this means we can't use "=b" for the second output register */
    __asm__ __volatile__ ( \
      "push %%ebx\n"
      "cpuid\n" \
      "mov %%ebx, %1\n"
      "pop %%ebx\n"
      : "=a" (a), "=r" (b), "=c" (c), "=d" (d) \
      : "a" (func), "c" (round) \
    );
    values[0] = a;
    values[1] = b;
    values[2] = c;
    values[3] = d;
}
#elif defined(__x86_64__)
static inline __attribute__((always_inline)) void x86_cpuid(int func, uint32_t values[4])
{
    uint64_t a, b, c, d;
    /* We need to preserve ebx since we're compiling PIC code */
    /* this means we can't use "=b" for the second output register */
    __asm__ __volatile__ ( \
      "push %%rbx\n"
      "cpuid\n" \
      "mov %%rbx, %1\n"
      "pop %%rbx\n"
      : "=a" (a), "=r" (b), "=c" (c), "=d" (d) \
      : "a" (func) \
    );
    values[0] = a;
    values[1] = b;
    values[2] = c;
    values[3] = d;
}
static inline __attribute__((always_inline)) void x86_cpuid_round(int func, int round, uint32_t values[4])
{
    uint64_t a, b, c, d;
    /* We need to preserve ebx since we're compiling PIC code */
    /* this means we can't use "=b" for the second output register */
    __asm__ __volatile__ ( \
      "push %%rbx\n"
      "cpuid\n" \
      "mov %%rbx, %1\n"
      "pop %%rbx\n"
      : "=a" (a), "=r" (b), "=c" (c), "=d" (d) \
      : "a" (func), "c" (round) \
    );
    values[0] = a;
    values[1] = b;
    values[2] = c;
    values[3] = d;
}
#endif

/* this is intended to use with STT_GNU_IFUNC during link time
   which is single threaded
*/

static uint64_t __g_cpu_features_link_time_features = ~0ULL;

/* According to http://en.wikipedia.org/wiki/CPUID */

/* This spells out "GenuineIntel".  */
#define VENDOR_INTEL_b  0x756e6547
#define VENDOR_INTEL_c  0x6c65746e
#define VENDOR_INTEL_d  0x49656e69

/* This spells out "AuthenticAMD".  */
#define VENDOR_AMD_b  0x68747541
#define VENDOR_AMD_c  0x444d4163
#define VENDOR_AMD_d  0x69746e65

static inline __attribute__((always_inline))
uint64_t android_getCpuFeatures_link_time(void)
{
    uint32_t regs[4] = {0};
    if (__g_cpu_features_link_time_features == ~0ULL)
    {
        __g_cpu_features_link_time_features = 0;

        x86_cpuid(0, regs);
        int vendorIsIntel = (regs[1] == VENDOR_INTEL_b &&
                             regs[2] == VENDOR_INTEL_c &&
                             regs[3] == VENDOR_INTEL_d);

        int family = (regs[0] >> 8) & 0x0f;
        int model = (regs[0] >> 4) & 0x0f;
        int extended_model = (regs[0] >> 12) & 0xf0;
        model += extended_model;
        (void)family;

        x86_cpuid(1, regs);
        if ((regs[2] & (1 << 9)) != 0) {
            __g_cpu_features_link_time_features |= ANDROID_CPU_X86_FEATURE_SSSE3;
        }
        if ((regs[2] & (1 << 23)) != 0) {
            __g_cpu_features_link_time_features |= ANDROID_CPU_X86_FEATURE_POPCNT;
        }
        if ((regs[2] & (1 << 19)) != 0) {
            __g_cpu_features_link_time_features |= ANDROID_CPU_X86_FEATURE_SSE4_1;
        }
        if ((regs[2] & (1 << 20)) != 0) {
            __g_cpu_features_link_time_features |= ANDROID_CPU_X86_FEATURE_SSE4_2;
        }
        if (vendorIsIntel && (regs[2] & (1 << 22)) != 0) {
            __g_cpu_features_link_time_features |= ANDROID_CPU_X86_FEATURE_MOVBE;
        }
        if (vendorIsIntel && model == 0x26) {
            __g_cpu_features_link_time_features |= ANDROID_CPU_X86_FEATURE_SLOW_BSF;
        }
        if ((regs[2] & (1 << 1)) != 0) {
            __g_cpu_features_link_time_features |= ANDROID_CPU_X86_FEATURE_PCLMULQDQ;
        }
        if ((regs[2] & (1 << 25)) != 0) {
            __g_cpu_features_link_time_features |= ANDROID_CPU_X86_FEATURE_AES;
        }
        if ((regs[2] & (1 << 28)) != 0) {
            __g_cpu_features_link_time_features |= ANDROID_CPU_X86_FEATURE_AVX;
        }

        x86_cpuid(7, regs);
        if ((regs[1] & (1 << 5)) != 0) {
            __g_cpu_features_link_time_features |= ANDROID_CPU_X86_FEATURE_AVX2 |
                                                   ANDROID_CPU_X86_FEATURE_AVX_FAST_UNALIGNED;
        }
    }
    return __g_cpu_features_link_time_features;
}

/* see http://www.microbe.cz/docs/CPUID.pdf for constant value */
#define KB 1024
#define MB 1024 * KB

static const struct {
    uint32_t value;
    int level;
    int size;
} cpu_cache_size [] = {
    {0x0A, 1, 8 * KB},
    {0x0C, 1, 16 * KB},
    {0x0D, 1, 16 * KB},
    {0x0E, 1, 24 * KB},
    {0x21, 2, 256 * KB},
    {0x22, 3, 512 * KB},
    {0x23, 3, 1 * MB},
    {0x25, 3, 2 * MB},
    {0x29, 3, 4 * MB},
    {0x2C, 1, 32 * KB},
    {0x41, 2, 128 * KB},
    {0x42, 2, 256 * KB},
    {0x43, 2, 512 * KB},
    {0x44, 2, 1 * MB},
    {0x45, 2, 2 * MB},
    {0x46, 3, 4 * MB},
    {0x47, 3, 8 * MB},
    {0x48, 2, 3 * MB},
    {0x49, 3, 4 * MB},
    {0x4A, 3, 6 * MB},
    {0x4B, 3, 8 * MB},
    {0x4C, 3, 12 * MB},
    {0x4D, 3, 16 * KB},
    {0x4E, 2, 6 * KB},
    {0x60, 1, 16 * KB},
    {0x66, 1, 8 * KB},
    {0x67, 1, 16 * KB},
    {0x68, 1, 32 * KB},
    {0x78, 2, 1 * MB},
    {0x79, 2, 128 * KB},
    {0x7A, 2, 256 * KB},
    {0x7B, 2, 512 * KB},
    {0x7C, 2, 1 * MB},
    {0x7D, 2, 2 * MB},
    {0x7F, 2, 512 * KB},
    {0x80, 2, 512 * KB},
    {0x82, 2, 256 * KB},
    {0x83, 2, 512 * KB},
    {0x84, 2, 1 * MB},
    {0x85, 2, 2 * MB},
    {0x86, 2, 512 * KB},
    {0x87, 2, 1 * MB},
    {0xD0, 3, 512 * KB},
    {0xD1, 3, 1 * MB},
    {0xD2, 3, 2 * MB},
    {0xD6, 3, 1 * MB},
    {0xD7, 3, 2 * MB},
    {0xD8, 3, 4 * MB},
    {0xDC, 3, 1536 * KB},
    {0xDD, 3, 3 * MB},
    {0xDE, 3, 6 * MB},
    {0xE2, 3, 2 * MB},
    {0xE3, 3, 4 * MB},
    {0xE4, 3, 8 * MB},
    {0xEA, 3, 12 * MB},
    {0xEB, 3, 18 * MB},
    {0xEC, 3, 24 * MB},
};
#define cpu_cache_size_len (sizeof(cpu_cache_size) / sizeof(cpu_cache_size[0]))

#undef MB
#undef KB

static __attribute__((noinline))
void handle_intel_cpuid4(int* l1d, int* l2, int* l3)
{
    uint32_t regs[4] = {0};
    int round = 0;
    int type = 0;
    int level = 0;
    int size = 0;

    while (1) {
        x86_cpuid_round(4, round, regs);
        type = regs[0] & 0x1f;
        if (type == 0) break;

        level = (regs[0] >> 5) & 0x7;
        size = (((regs[1] >> 22) + 1) * (((regs[1] >> 12) & 0x3ff) + 1) * ((regs[1] & 0xfff) + 1) * (regs[2] + 1));
        switch (level)
        {
        case 1:
            if (type == 1) {
                *l1d = size;
            }
            break;
        case 2:
            *l2 = size;
            break;
        case 3:
            *l3 = size;
            break;
        }
        round++;
    }
}

static  __attribute__((noinline))
int handle_intel_cpuid2(uint32_t reg, int* l1d, int* l2, int* l3)
{
    uint32_t desc;
    uint32_t i;

    if (reg & 0x80000000)
    {
        return 0;
    }

    while (reg != 0)
    {
        desc = reg & 0xff;
        if (desc == 0xff)
        {
            handle_intel_cpuid4(l1d, l2, l3);
            return 1;
        }

        for (i=0;i<cpu_cache_size_len;i++)
            if (desc == cpu_cache_size[i].value)
            {
                switch (cpu_cache_size[i].level)
                {
                case 1:
                    *l1d = cpu_cache_size[i].size;
                    break;
                case 2:
                    *l2 = cpu_cache_size[i].size;
                    break;
                case 3:
                    *l3 = cpu_cache_size[i].size;
                    break;
                }
                break;
            }

        reg >>= 8;
    }

    return *l1d && *l2 && *l3;
}

static __attribute__((noinline))
void handle_intel(int* l1d, int* l2, int* l3)
{
    uint32_t regs[4] = {0};
    int rounds = 0;

    do {
        x86_cpuid(2, regs);
        if (rounds == 0) {
            rounds = regs[0] & 0xff;
            regs[0] &= 0xffffff00;
        }

        if (handle_intel_cpuid2((uint32_t)regs[0], l1d, l2, l3) ||
            handle_intel_cpuid2((uint32_t)regs[1], l1d, l2, l3) ||
            handle_intel_cpuid2((uint32_t)regs[2], l1d, l2, l3) ||
            handle_intel_cpuid2((uint32_t)regs[3], l1d, l2, l3))
        {
            break;
        }
    } while(--rounds > 0);
}

static __attribute__((noinline))
void handle_amd(int* l1d, int* l2, int* l3)
{
    uint32_t regs[4] = {0};
    x86_cpuid(0x80000000, regs);

    if (regs[0] >= 0x80000006)
    {
        x86_cpuid(0x80000006, regs);

        *l1d = (regs[2] >> 14) & 0x3fc00;
        *l2 = (regs[2] & 0xf000) == 0 ? 0 : (regs[2] >> 6) & 0x3fffc00;
        *l3 = (regs[3] & 0xf000) == 0 ? 0 : (regs[3] & 0x3ffc0000) << 1;
    }
    else if (regs[0] >= 0x80000005)
    {
        x86_cpuid(0x80000005, regs);

        *l1d = (regs[2] >> 14) & 0x3fc00;
        *l2 = *l3 = 0;
    }
}

static inline __attribute__((always_inline))
void android_getCpu_cache_size(int* l1d, int* l2, int* l3)
{
    uint32_t regs[4] = {0};

    x86_cpuid(0, regs);
    int vendorIsIntel = (regs[1] == VENDOR_INTEL_b &&
                         regs[2] == VENDOR_INTEL_c &&
                         regs[3] == VENDOR_INTEL_d);
    int vendorIsAMD   = (regs[1] == VENDOR_AMD_b &&
                         regs[2] == VENDOR_AMD_c &&
                         regs[3] == VENDOR_AMD_d);
    if (vendorIsIntel)
    {
        handle_intel(l1d, l2, l3);
    }
    else if (vendorIsAMD)
    {
        handle_amd(l1d, l2, l3);
    }
}

#endif /* CPU_FEATURES_LINK_TIME_H */

