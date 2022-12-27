#pragma once

#include <atomic>

#include "util/algo.h"
#include "util/fileutil.h"
#include "mixers32.h"
#include "combiner.h"

namespace mixer {

template <typename T>
struct counter_stream {
	T increment{};
	T i{};

	T operator()() {
		i += increment;
		return i;
	}
};

template <typename T>
struct function_stream {
	const std::function<T(T)> f;
	T i{};

	T operator()() {
		return f(++i);
	}
};


template <typename T>
stream<T> create_counter_stream(T increment, T start) {
	return {"counter-" + std::to_string(increment), counter_stream<T>{increment, start}};
}

template <typename T>
stream<T> create_counter_stream(T increment) {
	return create_counter_stream<T>(increment, 0);
}

#define FUNC(exp) [=](T i) { return exp; }

template <typename T>
stream<T> create_gray_code(T d) {
	return {"graycode-" + std::to_string(d), function_stream<T>{FUNC(i ^ (i / d))}};
}

#undef FUNC

template <typename T>
stream<T> create_stream_from_data_by_ref(const std::string& name, const std::vector<T>& data) {
	std::size_t index = 0;
	return stream<T>{
		name, [&data, index]() mutable -> T {
			return data[index++ % data.size()];
		}
	};
}

template <typename T>
stream<T> create_stream_from_data_by_ref(const std::string& name, const std::vector<T>& data, stream<T> indexer) {
	return stream<T>{
		name, [&data, indexer]() mutable -> T {
			return data[indexer() % data.size()];
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
stream_double rescale_type_to_01(stream<T> source) {
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
stream<T> create_combined_serial_stream(stream<T> source, combiner<T> combiner, int draws) {
	return {
		combiner.name + "(" + source.name + ", " + std::to_string(draws) + ")",
		[combiner, source, draws]() mutable {
			T x = source();
			for (int i = 0; i < draws; ++i) {
				x = combiner(x, source());
			}
			return x;
		}
	};
}

template <typename T>
stream<T> create_combined_constant_stream(combiner<T> combiner, T c, int draws) {
	return {
		combiner.name + "(" + std::to_string(c) + ")",
		[combiner, draws, c]() mutable {
			T x = c;
			for (int i = 0; i < draws; ++i) {
				x = combiner(x, c);
			}
			c = x;
			return x;
		}
	};
}


}
