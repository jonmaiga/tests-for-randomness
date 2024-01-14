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
stream<T> create_bit_isolation_stream(stream<T> source, int bit) {
	return stream<T>{
		"bit-" + std::to_string(bit) + "(" + source.name + ")", [source, bit]() mutable -> T {
			return isolate_bit_by_ref(source, bit);
		}
	};
}

template <typename T>
stream<T> create_bit_reverse_stream(stream<T> source) {
	return stream<T>{
		"reverse-bits(" + source.name + ")", [source]() mutable -> T {
			return reverse_bits<T>(source());
		}
	};
}

template <typename T>
stream<T> create_byteswap_stream(stream<T> source) {
	return stream<T>{
		"byteswap(" + source.name + ")", [source]() mutable -> T {
			return byteswap<T>(source());
		}
	};
}

template <typename T>
stream<T> create_ror_stream(stream<T> source, int rotate_right) {
	if (rotate_right == 0) {
		return source;
	}
	return stream<T>{
		"ror(" + source.name + "," + std::to_string(rotate_right) + ")", [source, rotate_right]() mutable -> T {
			return ror(source(), rotate_right);
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
