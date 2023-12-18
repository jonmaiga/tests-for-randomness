#pragma once

#include <string>
#include <functional>

#include "util/assertion.h"

namespace tfr {
template <typename T>
class stream {
public:
	using value_type = T;

	stream(std::string name, std::function<value_type()> next)
		: name(std::move(name)), next(std::move(next)) {
	}

	std::string name;

	value_type operator()() {
		return next();
	}

private:
	std::function<value_type()> next;
};

template <typename T>
class stream_iterator {
public:
	using value_type = T;

	explicit stream_iterator(stream<T> s, uint64_t n, uint64_t index = 0)
		: s(std::move(s)),
		  current_value(this->s()),
		  index(index),
		  n(n) {
	}

	value_type operator *() const {
		assertion(index < n, "Out of range");
		return current_value;
	}

	stream_iterator& operator ++() {
		++index;
		current_value = s();
		return *this;
	}

	void operator ++(int) {
		++*this;
	}

	bool operator==(const stream_iterator& rhs) const {
		return index == rhs.index;
	}

	bool operator!=(const stream_iterator& rhs) const {
		return !(*this == rhs);
	}

private:
	stream<T> s;
	T current_value;
	uint64_t index;
	uint64_t n;
};


template <typename T>
class ranged_stream {
public:
	using value_type = T;

	ranged_stream(stream<T> s, uint64_t n) : s(std::move(s)), n(n) {
	}

	stream_iterator<T> begin() const {
		return stream_iterator(s, n);
	}

	stream_iterator<T> end() const {
		return stream_iterator(s, n, n);
	}

	std::size_t size() const {
		return n;
	}

	bool empty() const {
		return n == 0;
	}

private:
	stream<T> s;
	uint64_t n;
};
}
