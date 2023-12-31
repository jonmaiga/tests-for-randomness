#pragma once

#include <atomic>

#include "util/algo.h"
#include "util/fileutil.h"
#include "combiner.h"

namespace tfr {
template <typename T>
struct counter_stream {
	uint64_t increment{};
	uint64_t state{};

	T operator()() {
		state += increment;
		return static_cast<T>(state);
	}
};

template <typename T>
stream<T> create_counter_stream(uint64_t increment, uint64_t start) {
	return {"counter-" + std::to_string(increment), counter_stream<T>{increment, start}};
}

template <typename T>
stream<T> create_constant_stream(T c) {
	return {"constant-" + std::to_string(c), [c] { return c; }};
}

template <typename T>
stream<T> create_counter_stream(uint64_t increment) {
	return create_counter_stream<T>(increment, 0);
}

template <typename T>
stream<T> create_stream_from_data(const std::string& name, const std::vector<T>& data, std::size_t start_index = 0) {
	std::size_t index = start_index;
	return stream<T>{
		name, [data, index]() mutable -> T {
			return data[index++ % data.size()];
		}
	};
}

template <typename T>
stream<T> create_stream_from_data_by_ref(const std::string& name, const std::vector<T>& data, std::size_t start_index = 0) {
	std::size_t index = start_index;
	return stream<T>{
		name, [&data, index]() mutable -> T {
			return data[index++ % data.size()];
		}
	};
}

template <typename T>
stream<T> create_stream_from_data_by_ref_thread_safe(const std::string& name, const std::vector<T>& data) {
	static std::atomic_size_t index = 0;
	return stream<T>{
		name, [&data]() -> T {
			return data[index++ % data.size()];
		}
	};
}

template <typename T>
stream<T> create_stream_from_mixer(stream<T> source, const mixer<T>& mixer) {
	return {
		mixer.name + "(" + source.name + ")",
		[source, mixer]() mutable { return mixer(source()); }
	};
}

template <typename T>
stream<double> rescale_type_to_01(stream<T> source) {
	return {
		"rescale_type_to_01(" + source.name + ")",
		[source]() mutable { return rescale_type_to_01(source()); }
	};
}

template <typename T>
stream<T> create_combined_stream(stream<T> source_a, stream<T> source_b, combiner<T> combiner) {
	return {
		combiner.name + "(" + source_a.name + ", " + source_b.name + ")",
		[combiner, source_a, source_b]() mutable {
			return combiner(source_a(), source_b());
		}
	};
}

template <typename T>
stream<T> create_combined_incremental_stream(T seed, stream<T> source, combiner<T> combiner) {
	return {
		combiner.name + "(x, " + source.name + ")",
		[x = seed, source, combiner]() mutable {
			x = combiner(x, source());
			return x;
		}
	};
}
}
