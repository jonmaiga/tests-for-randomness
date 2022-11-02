#include <statistics/avalanche.h>

#include <gtest/gtest.h>

#include "mixers.h"

namespace mixer {

TEST(avalanche, bic_matrix_no_change) {
	const auto bit_counts = run_avalanche(10, create_counter_stream(1), mx3);
	uint64_t h = 0;
	for (const auto v : bit_counts) {
		h += mx3(v);
	}
	EXPECT_EQ(h, 7466099936495255145ull);
}

}
