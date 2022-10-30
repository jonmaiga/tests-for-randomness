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

using stream_factory = std::function<stream()>;
using append_stream_factory = std::function<stream(const stream&)>;

struct test_config {
	uint64_t n{};
	stream source;
	mixer mixer;
	append_stream_factory append_stream_factory;
};

using test_factory = std::function<test_config()>;

}
