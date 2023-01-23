#include "aes.h"

#include <immintrin.h>
#include "mixers32.h"

namespace mixer {

uint32_t aes32(uint32_t x) {
	// see https://www.jandrewrogers.com/2019/02/12/fast-perfect-hashing/
	// maybe try as prng: https://github.com/dragontamer/AESRand/blob/master/AESRand_Linux/AESRand.cpp
	static const __m128i key = _mm_set1_epi32(static_cast<int>(2471660141));

	auto a = _mm_set1_epi32(x);
	auto r = _mm_aesenc_si128(a, key);
	return _mm_extract_epi32(r, 0);
}

}
