#pragma once

#include "util/fileutil.h"
#include "types.h"

namespace mixer {

using stream_factory = std::function<stream()>;

struct generated_stream {
	std::vector<uint64_t> data;
	std::size_t index = 0;

	uint64_t operator()() {
		if (index == data.size()) {
			throw std::runtime_error("No more stream data.");
		}
		return data[index++];
	}
};

struct counter_stream {
	uint64_t increment{};
	uint64_t n{};
	uint64_t index{};

	uint64_t operator()() {
		if (n == 0) {
			throw std::runtime_error("No more stream data.");
		}
		--n;
		index += increment;
		return index;
	}
};

struct test_stream {
	uint64_t n{};
	uint64_t index{};

	uint64_t operator()() {
		if (index == n) {
			throw std::runtime_error("No more stream data.");
		}
		return 2 ^ index++ + 1;
	}
};

inline stream create_counter_stream(uint64_t increment, std::size_t n) {
	return {"counter-" + std::to_string(increment), counter_stream{increment, n}};
}

inline stream create_xor_stream(std::size_t n) {
	return {"A256008", test_stream{n}};
}


inline stream createStreamFromBinaryFile(const std::filesystem::path& path, int n) {
	auto data = readBinaryMustExist<uint64_t>(path.string());
	if (n < data.size()) {
		data.resize(n);
	}
	return {path.filename().string(), generated_stream{data}};
}


}
