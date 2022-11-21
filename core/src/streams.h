#pragma once

#include <atomic>

#include "types.h"
#include "util/algo.h"
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

inline stream_uint64 create_stream_from_mixer(stream_uint64 source, const mixer64& mixer) {
	return {
		mixer.name + "(" + source.name + ")",
		[source, mixer]() mutable { return mixer(source()); }
	};
}

inline stream_double rescale64_to_01(stream_uint64 source) {
	return {
		"rescale64_to_01(" + source.name + ")",
		[source]() mutable { return rescale64_to_01(source()); }
	};
}

}
