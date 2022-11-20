#pragma once

#include <string>
#include <functional>

namespace mixer {

class stream {
public:
	std::string name;
	std::function<uint64_t()> next;

	uint64_t operator()() const {
		return next();
	}
};

class stream_iterator {
public:
	using value_type = uint64_t;

	explicit stream_iterator(const stream& stream, uint64_t n, uint64_t index = 0)
		: stream(stream),
		  current_value(0),
		  index(index),
		  n(n) {
	}

	value_type operator *() const {
		return current_value;
	}

	stream_iterator& operator ++() {
		++index;
		current_value = stream();
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
	const stream& stream;
	uint64_t current_value;
	uint64_t index;
	uint64_t n;
};


class ranged_stream {
public:
	ranged_stream(const stream& s, uint64_t n) : s(s), n(n) {
	}

	stream_iterator begin() const {
		return stream_iterator(s, n);
	}

	stream_iterator end() const {
		return stream_iterator(s, n, n);
	}

	std::size_t size() const {
		return n;
	}

	bool empty() const {
		return n == 0;
	}

private:
	const stream& s;
	uint64_t n;
};

}
