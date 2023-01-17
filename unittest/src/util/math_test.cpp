#include <util/math.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(math, rescale_basic) {
	EXPECT_EQ(rescale_to_01(0, 0, 0), 0.5);
	EXPECT_EQ(rescale_to_01(1, 2, 2), 0.5);
	EXPECT_EQ(rescale_to_01(-1, -4, -4), 0.5);

	EXPECT_EQ(rescale_to_01(0, 0, 1), 0);
	EXPECT_EQ(rescale_to_01(.5, 0, 1), 0.5);
	EXPECT_EQ(rescale_to_01(1, 0, 1), 1);
	EXPECT_EQ(rescale_to_01(33, 0, 66), .5);
	EXPECT_EQ(rescale_to_01(33, 3, 63), .5);
}

TEST(math, snap_to_01) {
	EXPECT_EQ(snap_to_01(0.5), 0.5);
	EXPECT_EQ(snap_to_01(0.1), 0.1);
	EXPECT_EQ(snap_to_01(0.9), 0.9);
	EXPECT_EQ(snap_to_01(0), 0);
	EXPECT_EQ(snap_to_01(1), 1);
	EXPECT_EQ(snap_to_01(1 - 1e-7), 1 - 1e-7);
	EXPECT_EQ(snap_to_01(1 + 1e-7), 1);
	EXPECT_EQ(snap_to_01(0 - 1e-7), 0);
	EXPECT_EQ(snap_to_01(0 + 1e-7), 0 + 1e-7);
}

TEST(math, stirling_second_kind) {
	EXPECT_EQ(stirling_second_kind(0, 0), 1);
	EXPECT_EQ(stirling_second_kind(0, 1), 0);
	EXPECT_EQ(stirling_second_kind(1, 0), 0);
	EXPECT_EQ(stirling_second_kind(1, 1), 1);
	EXPECT_EQ(stirling_second_kind(4, 2), 7);
	EXPECT_EQ(stirling_second_kind(4, 3), 6);
	EXPECT_EQ(stirling_second_kind(20, 9), 12011282644725);
	EXPECT_EQ(stirling_second_kind(20, 10), 5917584964655);
	EXPECT_EQ(stirling_second_kind(20, 11), 1900842429486);
}

TEST(math, stirling_second_kind_limits) {
	EXPECT_EQ(stirling_second_kind(25, 5), 2436684974110751);
	EXPECT_EQ(stirling_second_kind(25, 6), 37026417000002430);
	EXPECT_EQ(stirling_second_kind(25, 7), 227832482998716310);
	EXPECT_EQ(stirling_second_kind(25, 8), 690223721118368580);
	EXPECT_EQ(stirling_second_kind(25, 9), 1167921451092973005);
	EXPECT_EQ(stirling_second_kind(25, 10), 1203163392175387500);
	EXPECT_EQ(stirling_second_kind(25, 11), 802355904438462660);
	EXPECT_EQ(stirling_second_kind(25, 12), 362262620784874680);
}

TEST(math, harmonic_numerical) {
	EXPECT_NEAR(harmonic(1), 1., 1e-13);
	EXPECT_NEAR(harmonic(2), 3./2., 1e-13);
	EXPECT_NEAR(harmonic(3), 11./6., 1e-13);
	EXPECT_NEAR(harmonic(4), 25./12., 1e-13);
	EXPECT_NEAR(harmonic(5), 137./60., 1e-13);
	EXPECT_NEAR(harmonic(6), 49./20., 1e-13);
	EXPECT_NEAR(harmonic(10), 7381./2520., 1e-13);
	EXPECT_NEAR(harmonic(1000), 7.4854708605503449127, 1e-13);
}

TEST(math, lambert_w) {
	EXPECT_NEAR(lambert_w_approximation(1), 0.5600, 1e-4); //mma 0.5671
	EXPECT_NEAR(lambert_w_approximation(2), 1.4335, 1e-4); //mma 0.8526
	EXPECT_NEAR(lambert_w_approximation(10), 1.7390, 1e-4); //mma 1.7455
	EXPECT_NEAR(lambert_w_approximation(100), 3.3925, 1e-4);
	EXPECT_NEAR(lambert_w_approximation(1000), 5.2535, 1e-4);
	EXPECT_NEAR(lambert_w_approximation(1000000), 11.3840, 1e-4);
	EXPECT_NEAR(lambert_w_approximation(1000000000), 17.8419, 1e-4);
}

TEST(math, fishers_transformation_safe) {
	EXPECT_EQ(fishers_transformation_safe(0), 0);
	EXPECT_EQ(fishers_transformation_safe(.5), std::atanh(.5));
	EXPECT_EQ(fishers_transformation_safe(-.5), std::atanh(-.5));
	EXPECT_NEAR(fishers_transformation_safe(.99), 2.6466, 1e-4);
	EXPECT_NEAR(fishers_transformation_safe(-1), -18.715, 1e-4);
	EXPECT_NEAR(fishers_transformation_safe(1), 18.715, 1e-4);
}

}
