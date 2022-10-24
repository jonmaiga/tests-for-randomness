#pragma once

namespace mixer {

struct stream {
	std::string name;
	std::function<uint64_t()> next;

	uint64_t operator()() const {
		return next();
	}
};

struct mixer {
	std::string name;
	std::function<uint64_t(uint64_t)> mix;

	uint64_t operator()(uint64_t x) const {
		return mix(x);
	}
};

struct test_config {
	stream stream;
	mixer mixer;
};

using test_factory = std::function<test_config()>;

}
