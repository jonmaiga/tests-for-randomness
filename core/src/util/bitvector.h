#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "assertion.h"

namespace tfr {

struct bit_vector {
	bit_vector() = default;

	bit_vector(int bits) : bits(bits) {
		data.resize(std::ceil(bits / 8.));
	}

	bool get_bit(int bit) const {
		assertion(bit < bits, "bit oob");
		const size_t index = bit / 8;
		const int shift = bit - 8 * index;
		return ((data[index] >> shift) & 1) == 1;
	}

	void set_bit(size_t bit, bool value) {
		assertion(bit < bits, "bit oob");
		const size_t index = bit / 8;
		assertion(index < data.size(), "bit data oob");

		const auto shift = bit - 8 * index;
		if (value)
			data[index] |= (value << shift);
		else
			data[index] &= ~(1 << shift);
	}

	int size() const { return bits; }

	int count() const {
		int sum = 0;
		for (int i = 0; i < bits; ++i) {
			if (get_bit(i)) sum++;
		}
		return sum;
	}

	template <typename T = uint64_t>
	T get(int from_bit, int count) const {
		T v = 0;
		int s = 0;
		const int to_bit = from_bit + count;
		for (int i = from_bit; i < to_bit; ++i, ++s) {
			if (get_bit(i)) {
				v |= static_cast<T>(1) << s;
			}
		}
		return v;
	}

	void add(uint64_t v, int count) {
		const int old_bits = bits;
		bits += count;
		data.resize(static_cast<std::size_t>(std::ceil(bits / 8.)));
		for (int i = 0; i < count; ++i) {
			set_bit(old_bits + i, ((v >> i) & 1) == 1);
		}
	}


private:
	int bits = 0;
	std::vector<unsigned char> data;
};

inline std::string to_string(const bit_vector& bv) {
	std::string s;
	for (int i = 0; i < bv.size(); ++i) {
		s += bv.get_bit(i) ? "1" : "0";
	}
	return s;
}

}
