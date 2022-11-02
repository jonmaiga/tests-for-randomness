#include <statistics/avalanche.h>

#include <gtest/gtest.h>

#include "mixers.h"

namespace mixer {

TEST(avalanche, sac_no_change) {
	const auto bit_counts = avalanche_generate_sac(10, create_counter_stream(1), mx3);
	uint64_t h = 0;
	for (const auto v : bit_counts) {
		h += mx3(v);
	}
	EXPECT_EQ(h, 7766637459189417547ull);
}

TEST(avalanche, bic_no_change) {
	const auto bit_counts = avalanche_generate_bic(10, create_counter_stream(1), mx3);
	uint64_t h = 0;
	for (const auto v : bit_counts) {
		h += mx3(v);
	}
	EXPECT_EQ(h, 7466099936495255145ull);
}

}
