#pragma once

namespace mixer {

template <typename T>
struct mixer {
	using value_type = T;

	std::string name;
	std::function<T(T)> mix;

	T operator()(T x) const {
		return mix(x);
	}

};

template <typename T>
std::vector<mixer<T>> get_mixers() = delete;

}
