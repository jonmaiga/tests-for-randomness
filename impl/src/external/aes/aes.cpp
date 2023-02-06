#include "aes.h"

#include <immintrin.h>
#include "mixers32.h"

namespace tfr {

uint32_t aes_mix(uint32_t x) {
	// from: https://www.jandrewrogers.com/2019/02/12/fast-perfect-hashing/
	static const __m128i key = _mm_set1_epi32(static_cast<int>(2471660141));

	auto a = _mm_set1_epi32(x);
	auto r = _mm_aesenc_si128(a, key);
	return _mm_extract_epi32(r, 0);
}

uint32_t aes_prng128(std::array<uint32_t, 4>& state32) {
	// from: https://github.com/dragontamer/AESRand/blob/master/AESRand_Linux/AESRand.cpp
	static const __m128i increment = _mm_set_epi8(0x2f, 0x2b, 0x29, 0x25, 0x1f, 0x1d, 0x17, 0x13,
	                                              0x11, 0x0D, 0x0B, 0x07, 0x05, 0x03, 0x02, 0x01);

	__m128i s = _mm_setr_epi32(state32[0], state32[1], state32[2], state32[3]);
	s = _mm_add_epi64(s, increment);
	s = _mm_aesenc_si128(s, increment);
	_mm_storeu_si128((__m128i*)&state32[0], s);
	return _mm_extract_epi32(s, 0);
}

}
