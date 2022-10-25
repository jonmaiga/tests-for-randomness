#pragma once

#include "util/fileutil.h"
#include "types.h"

namespace mixer {

using stream_factory = std::function<stream()>;

struct counter_stream {
	uint64_t increment{};
	uint64_t i{};

	uint64_t operator()() {
		i += increment;
		return i;
	}
};

struct function_stream {
	const std::function<uint64_t(uint64_t)> f;
	uint64_t i{};

	uint64_t operator()() {
		return f(++i);
	}
};

inline stream create_counter_stream(uint64_t increment) {
	return {"counter-" + std::to_string(increment), counter_stream{increment}};
}

#define FUNC(exp) [=](uint64_t i) { return exp; }

inline stream create_gray_code(uint64_t d) {
	return {"graycode-" + std::to_string(d), function_stream{FUNC(i ^ (i / d))}};
}

#undef FUNC

inline const std::vector<uint64_t>& get_trng_data() {
	static const auto trng_data = []() {
		std::cout << "Reading trng stream from disk...";
		const auto& data = readBinaryMustExist<uint64_t>(R"(C:\tmp\random.org\trng.bin)");
		std::cout << " done!\n";
		return data;
	}();

	return trng_data;
}


inline stream create_data_stream(const std::string& name, const std::vector<uint64_t>& data) {
	size_t index = 0;
	return stream{
		name, [index, &data]() mutable -> uint64_t {
			if (index >= data.size()) {
				throw std::runtime_error("No more stream data.");
			}
			return data[index++];
		}
	};
}

inline stream create_mixer_stream(const stream& source, const mixer& mixer) {
	return {
		mixer.name + "(" + source.name + ")",
		[source, mixer]() { return mixer(source()); }
	};
}


}
