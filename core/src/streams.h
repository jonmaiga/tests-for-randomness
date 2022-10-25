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

struct function_stream {
	const uint64_t n{};
	const std::function<uint64_t(uint64_t)> f;
	uint64_t i{};

	uint64_t operator()() {
		if (i == n) {
			throw std::runtime_error("No more stream data.");
		}
		return f(++i);
	}
};

inline stream create_counter_stream(uint64_t increment, std::size_t n) {
	return {"counter-" + std::to_string(increment), counter_stream{increment, n}};
}

#define FUNC(exp) [=](uint64_t i) { return exp; }

inline stream create_gray_code(std::size_t n, uint64_t d) {
	return {"graycode-" + std::to_string(d), function_stream{n, FUNC(i ^ (i / d))}};
}

#undef FUNC

inline stream createStreamFromBinaryFile(const std::filesystem::path& path, int n) {
	auto data = readBinaryMustExist<uint64_t>(path.string());
	if (n < data.size()) {
		data.resize(n);
	}
	return {path.filename().string(), generated_stream{data}};
}

inline stream create_mixer_stream(const stream& source, const mixer& mixer) {
	return {
		mixer.name + "(" + source.name + ")",
		[source, mixer]() { return mixer(source()); }
	};
}


}
