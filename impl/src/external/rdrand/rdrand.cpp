#include "rdrand.h"

#include <immintrin.h>
#include <iostream>
#include <string.h>

#include "util/assertion.h"

namespace tfr {

uint32_t rdrand_32() {
	uint32_t x = 0;
	for (int i = 0; i < 10; i++) {

		if (_rdrand32_step(&x))
			return x;
	}
	assertion(false, "rdrand failed");
	return 0;
}

uint64_t rdrand_64() {
	static_assert(sizeof(unsigned long long) == sizeof(uint64_t));
	unsigned long long x = 0;
	for (int i = 0; i < 10; i++) {
		if (_rdrand64_step(&x)) {
			return x;
		}
	}
	assertion(false, "rdrand failed");
	std::cout << "FAILURE";
	return 0;
}

}
