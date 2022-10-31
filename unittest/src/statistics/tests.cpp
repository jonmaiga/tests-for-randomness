#include <statistics/tests.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(z_test, basic) {
	EXPECT_NEAR(z_test(10, 0.5, 0.5, 0.1), 1., 1e-4);
	EXPECT_NEAR(z_test(10, 0.51, 0.5, 0.1), 0.9203, 1e-3);
	EXPECT_NEAR(z_test(10, 0.49, 0.5, 0.1), 0.9203, 1e-3);
	EXPECT_NEAR(z_test(10, 0.40, 0.5, 0.1), 0.317, 1e-3);
	EXPECT_NEAR(z_test(10, 0.55, 0.5, 0.2), 0.7236, 1e-4);
}

TEST(t_test, basic) {
	EXPECT_NEAR(t_test(10, 0.5, 0.1*0.1, 10, 0.5, 0.1*0.1), 1., 1e-4);
	EXPECT_NEAR(t_test(10, 0.49, 0.1*0.1, 10, 0.5, 0.1*0.1), .8255, 1e-4);
	EXPECT_NEAR(t_test(10, 0.49, 0.05*0.05, 10, 0.5, 0.1*0.1), .7809, 1e-3);
	EXPECT_NEAR(t_test(10, 0.49, 0.05*0.05, 12, 0.5, 0.1*0.1), .7771, 1e-3); // https://www.omnicalculator.com/statistics/t-test uses different when n 
	EXPECT_NEAR(t_test(10, 0.4, 0.15*0.15, 10, 0.51, 0.1*0.1), .0705, 1e-3);
	EXPECT_NEAR(t_test(10, 0.1, 0.15*0.15, 10, 0.51, 0.1*0.1), .0000, 1e-4);
}

TEST(f_test, basic) {
}

}
