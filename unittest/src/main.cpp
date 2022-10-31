#include <gtest/gtest.h>


class Environment : public ::testing::Environment {
public:
	void SetUp() override {
	}

	void TearDown() override {
	}
};

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(new Environment);
	return RUN_ALL_TESTS();
}
