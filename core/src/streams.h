#pragma once

#include <atomic>
#include <iostream>

#include "types.h"
#include "util/fileutil.h"

namespace mixer {

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

inline stream_uint64 create_counter_stream(uint64_t increment) {
	return {"counter-" + std::to_string(increment), counter_stream{increment}};
}

#define FUNC(exp) [=](uint64_t i) { return exp; }

inline stream_uint64 create_gray_code(uint64_t d) {
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

inline stream_uint64 create_stream_from_data_by_ref(const std::string& name, const std::vector<uint64_t>& data) {
	std::size_t index = 0;
	return stream_uint64{
		name, [&data, index]() mutable -> uint64_t {
			return data[index++ % data.size()];
		}
	};
}

inline stream_uint64 create_stream_from_data_by_ref_thread_safe(const std::string& name, const std::vector<uint64_t>& data) {
	static std::atomic_size_t index = 0;
	return stream_uint64{
		name, [&data]() -> uint64_t {
			return data[index++ % data.size()];
		}
	};
}

inline stream_uint64 create_stream_from_mixer(const stream_uint64& source, const mixer& mixer) {
	return {
		mixer.name + "(" + source.name + ")",
		[source, mixer]() { return mixer(source()); }
	};
}

}
