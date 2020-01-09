
#ifndef SIMD_H
#define SIMD_H

////////////////////////////////////////////////////////////////////////////////

#include <immintrin.h>

////////////////////////////////////////////////////////////////////////////////

// This code selects the SIMD implementation that is going to be used.
// In case that no SIMD is available, a no SIMD implementation is provided
// as well.

// NOTE!: AVX512 is untested. The PC used for the development does not support
// AVX512.

#define USE_AVX

#if defined(USE_AVX512)
#include "simd_avx512.h"
	namespace	SIMD	= AVX512;
#elif defined(USE_AVX)
#include "simd_avx.h"
	namespace	SIMD	= AVX;
#elif defined(USE_SSE)
#include "simd_sse.h"
	namespace	SIMD	= SSE;
#else
#include "simd_base.h"
	namespace	SIMD	= NO_SIMD;
#endif

////////////////////////////////////////////////////////////////////////////////

#endif // SIMD_H
