#pragma once

#include "util/bitvector.h"

namespace mixer {

//////////////////////////////////////////////////////////////////////
// xmx
//////////////////////////////////////////////////////////////////////
template <typename T>
struct xmx_constants {
	T C1;
	T C2;
	T m1;
};

template <typename T>
xmx_constants<T> to_xmx_constants(const bit_vector& bits) {
	constexpr auto s = shift_sizeof<T>();
	constexpr auto m = bit_sizeof<T>();
	return {bits.get<T>(0 * s, s), bits.get<T>(1 * s, s), bits.get<T>(2 * s, m)};
}

template <typename T>
mixer<T> create_xmx_mixer(const xmx_constants<T>& c) {
	return {
		"xmx", [c](T x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			return x;
		}
	};
}

template <typename T>
mixer<T> create_xmx_mixer(const bit_vector& bits) {
	return create_xmx_mixer<T>(to_xmx_constants<T>(bits));
}

//////////////////////////////////////////////////////////////////////
// xm2x
//////////////////////////////////////////////////////////////////////

template <typename T>
struct xm2x_constants {
	T C1;
	T C2;
	T C3;
	T m1;
};

template <typename T>
xm2x_constants<T> to_xm2x_constants(const bit_vector& bits) {
	constexpr auto s = shift_sizeof<T>();
	constexpr auto m = bit_sizeof<T>();
	return {bits.get<T>(0 * s, s), bits.get<T>(1 * s, s), bits.get<T>(2 * s, s), bits.get<T>(3 * s, m)};
}

template <typename T>
mixer<T> create_xm2x_mixer(const xm2x_constants<T>& c) {
	return mixer<T>{
		"xm2x", [c](T x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			x *= c.m1;
			x ^= (x >> c.C3);
			return x;
		}
	};
}

template <typename T>
mixer<T> create_xm2x_mixer(const bit_vector& bits) {
	return create_xm2x_mixer<T>(to_xm2x_constants<T>(bits));
}

//////////////////////////////////////////////////////////////////////
// xm3x
//////////////////////////////////////////////////////////////////////

template <typename T>
struct xm3x_constants {
	T C1;
	T C2;
	T C3;
	T C4;
	T m1;
};

template <typename T>
xm3x_constants<T> to_xm3x_constants(const bit_vector& bits) {
	constexpr auto s = shift_sizeof<T>();
	constexpr auto m = bit_sizeof<T>();
	return {bits.get<T>(0 * s, s), bits.get<T>(1 * s, s), bits.get<T>(2 * s, s), bits.get<T>(3 * s, s), bits.get<T>(4 * s, m)};
}

template <typename T>
mixer<T> create_xm3x_mixer(const xm3x_constants<T>& c) {
	return mixer<T>{
		"xm3x", [c](T x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			x *= c.m1;
			x ^= (x >> c.C3);
			x *= c.m1;
			x ^= (x >> c.C4);
			return x;
		}
	};
}

template <typename T>
mixer<T> create_xm3x_mixer(const bit_vector& bits) {
	return create_xm3x_mixer<T>(to_xm3x_constants<T>(bits));
}


}
