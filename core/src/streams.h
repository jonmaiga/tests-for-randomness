#pragma once

#include <atomic>

#include "types.h"
#include "util/algo.h"
#include "util/fileutil.h"

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
stream<T> create_counter_stream(T increment) {
	return {"counter-" + std::to_string(increment), counter_stream<T>{increment}};
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

}
