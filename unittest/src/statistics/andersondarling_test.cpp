#include <statistics/andersondarling.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(anderson_darling, basic) {
	EXPECT_NEAR(anderson_darling({.1,.2,.3,.4,.5,.6,.7,.8,.9}), 0.1549, 1e-4);
}

}
