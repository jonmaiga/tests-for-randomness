#include <statistics/basic.h>
#include <statistics/tests.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(z_test, unset) {
	EXPECT_FALSE(z_test(0, 10., 1., 0.1));
	EXPECT_FALSE(z_test(10, 10., 1., 0));
}

TEST(z_test, basic) {
	EXPECT_NEAR(z_test(10, 0.5, 0.5, 0.1)->p_value, 1., 1e-4);
	EXPECT_NEAR(z_test(10, 0.51, 0.5, 0.1)->p_value, 0.9203, 1e-3);
	EXPECT_NEAR(z_test(10, 0.49, 0.5, 0.1)->p_value, 0.9203, 1e-3);
	EXPECT_NEAR(z_test(10, 0.40, 0.5, 0.1)->p_value, 0.317, 1e-3);
	EXPECT_NEAR(z_test(10, 0.55, 0.5, 0.2)->p_value, 0.7236, 1e-4);
}

TEST(z_test, no_variance) {
	EXPECT_FALSE(z_test(10, 0, 0, 0));
}

TEST(t_test, basic) {
	EXPECT_NEAR(t_test(10, 0.5, 0.1*0.1, 0.5, 0.1*0.1), 1., 1e-4);
	EXPECT_NEAR(t_test(10, 0.49, 0.1*0.1, 0.5, 0.1*0.1), .8255, 1e-4);
	EXPECT_NEAR(t_test(10, 0.49, 0.05*0.05, 0.5, 0.1*0.1), .7809, 1e-3);
	EXPECT_NEAR(t_test(10, 0.49, 0.05*0.05, 0.5, 0.1*0.1), .7805, 1e-3);
	EXPECT_NEAR(t_test(10, 0.4, 0.15*0.15, 0.51, 0.1*0.1), .0705, 1e-3);
	EXPECT_NEAR(t_test(10, 0.1, 0.15*0.15, 0.51, 0.1*0.1), .0000, 1e-4);
}

TEST(f_test, basic) {
	EXPECT_NEAR(f_test(10, 0.1, 10, 0.1), 1., 1e-4);
	EXPECT_NEAR(f_test(10, 11, 10, 10), .8894, 1e-4);
	EXPECT_NEAR(f_test(10, 10, 10, 11), .8894, 1e-4);
	EXPECT_NEAR(f_test(12, 10, 10, 11), .8666, 1e-4);
}

TEST(ztf_test, from_data) {
	const auto data1 = {.61, .43, .51, .49, .49};
	const auto data2 = {.41, .55, .54, .45, .49};
	const auto s1 = basic_stats(data1);
	const auto s2 = basic_stats(data2);

	EXPECT_NEAR(z_test(s1.n, s1.mean, s2.mean, s2.variance())->p_value, 0.4481, 1e-4); // same as mma with population params
	EXPECT_NEAR(f_test(s1.n, s1.variance(), s2.n, s2.variance()), 0.8543, 1e-4);
	EXPECT_NEAR(t_test(s1.n, s1.mean, s1.variance(), s2.mean, s2.variance()), 0.6241, 1e-4);
}

}
