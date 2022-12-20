#pragma once

namespace mixer {

template <typename T>
struct combiner {
	using value_type = T;

	std::string name;
	std::function<T(T, T)> combine;

	T operator()(T x, T y) const {
		return combine(x, y);
	}
};


}
