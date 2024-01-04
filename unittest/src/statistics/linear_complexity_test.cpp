#include <statistics/linear_complexity.h>

#include <gtest/gtest.h>

#include "testutil.h"
#include "util/timer.h"

namespace tfr {
// from: https://mzhang2021.github.io/cp-blog/berlekamp-massey/
template <typename T>
int berlekamp_massey_ref(const std::vector<T>& s) {
	using namespace std;
	int n = (int)s.size(), l = 0, m = 1;
	vector<T> b(n), c(n);
	T ld = b[0] = c[0] = 1;
	for (int i = 0; i < n; i++, m++) {
		T d = s[i];
		for (int j = 1; j <= l; j++)
			d += c[j] * s[i - j];
		if (d == 0)
			continue;
		vector<T> temp = c;
		T coef = d / ld;
		for (int j = m; j < n; j++)
			c[j] -= coef * b[j - m];
		if (2 * l <= i) {
			l = i + 1 - l;
			b = temp;
			ld = d;
			m = 0;
		}
	}
	return l;
}

template <int MOD>
struct ModInt {
	long long v;

	ModInt(long long _v = 0) {
		v = (-MOD < _v && _v < MOD) ? _v : _v % MOD;
		if (v < 0) v += MOD;
	}

	ModInt& operator +=(const ModInt& other) {
		v += other.v;
		if (v >= MOD) v -= MOD;
		return *this;
	}

	ModInt& operator -=(const ModInt& other) {
		v -= other.v;
		if (v < 0) v += MOD;
		return *this;
	}

	ModInt& operator *=(const ModInt& other) {
		v = v * other.v % MOD;
		return *this;
	}

	ModInt& operator /=(const ModInt& other) { return *this *= inverse(other); }
	bool operator ==(const ModInt& other) const { return v == other.v; }
	bool operator !=(const ModInt& other) const { return v != other.v; }
	friend ModInt operator +(ModInt a, const ModInt& b) { return a += b; }
	friend ModInt operator -(ModInt a, const ModInt& b) { return a -= b; }
	friend ModInt operator *(ModInt a, const ModInt& b) { return a *= b; }
	friend ModInt operator /(ModInt a, const ModInt& b) { return a /= b; }
	friend ModInt operator -(const ModInt& a) { return 0 - a; }

	friend ModInt power(ModInt a, long long b) {
		ModInt ret(1);
		while (b > 0) {
			if (b & 1) ret *= a;
			a *= a;
			b >>= 1;
		}
		return ret;
	}

	friend ModInt inverse(ModInt a) { return power(a, MOD - 2); }
	//friend istream& operator >> (istream &is, ModInt &m) {is >> m.v; m.v = (-MOD < m.v && m.v < MOD) ? m.v : m.v % MOD; if (m.v < 0) m.v += MOD; return is;}
	//friend ostream& operator << (ostream &os, const ModInt &m) {return os << m.v;}
};

int bm_ref(const std::vector<int>& binary_sequence) {
	std::vector<ModInt<2>> m2;
	for (auto x : binary_sequence) {
		m2.push_back(x);
	}
	return berlekamp_massey_ref(m2);
}

struct bm_test {
	std::vector<int> bit_sequence;
	int expected_complexity{};
};

std::vector<bm_test> bm_tests = {
	{{0}, 0},
	{{1}, 1},
	{{0, 0}, 0},
	{{0, 1}, 2},
	{{1, 0}, 1},
	{{1, 1}, 1},
	{{0, 0, 1}, 3},
	{{1, 0, 1}, 2},
	{{1, 1, 1}, 1},
	{{1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1}, 4},
	{{1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0}, 2},
	{{1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1}, 3},
	{{1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1}, 4},
	{{1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1}, 6},
	{{1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1}, 7},
	{{0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1}, 16}
};

TEST(linear_complexity, berlekamp_massey) {
	for (const auto& test : bm_tests) {
		EXPECT_EQ(bm_ref(test.bit_sequence), test.expected_complexity);
		EXPECT_EQ(berlekamp_massey(test.bit_sequence), test.expected_complexity);
	}
}

TEST(linear_complexity, berlekamp_massey_cross) {
	const auto mix = get_default_mixer<uint64_t>();
	for (int i = 1; i <= 256; ++i) {
		std::vector<int> bs;
		for (int j = 1; j <= i; ++j) {
			bs.push_back(mix(i * j) % 2);
		}
		EXPECT_EQ(bm_ref(bs), berlekamp_massey(bs));
	}
}

template <typename T>
void benchmark_test(const T& impl) {
	const auto mix = get_default_mixer<uint64_t>();
	uint64_t s = 0;
	uint64_t total_ms = 0;
	for (int i = 1; i <= 4 * 1024; ++i) {
		std::vector<int> bs;
		for (int j = 1; j <= i; ++j) {
			bs.push_back(mix(i * j) % 2);
		}
		timer t;
		s += impl(bs);
		total_ms += t.milliseconds();
	}
	std::cout << total_ms << "ms\n";
}

TEST(linear_complexity, berlekamp_massey_perf) {
	//benchmark_test(bm_ref);
	//benchmark_test(berlekamp_massey);
}

TEST(linear_complexity, no_change) {
	using T = uint64_t;
	const auto stat = linear_complexity_stats<T>(1 << 14, test_stream(), 16);
	EXPECT_NEAR(stat->value, 2.3105, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.8890, 1e-4);
}

TEST(linear_complexity, no_change_8) {
	using T = uint8_t;
	auto n = 1 << 15;
	const auto ss = linear_complexity_test<T>(n, test_stream_casted<uint8_t>(n));
	EXPECT_NEAR(ss[0].stats->value, 5.2734, 1e-4);
	EXPECT_NEAR(ss[0].stats->p_value, 0.5092, 1e-4);
}

TEST(linear_complexity, lfsr_log_probability) {
	EXPECT_EQ(lfsr_log_probability(1, 0), -1);
	EXPECT_EQ(lfsr_log_probability(1, 1), -1);
	
	EXPECT_EQ(lfsr_log_probability(2, 0), -2);
	EXPECT_EQ(lfsr_log_probability(2, 1), -1);
	EXPECT_EQ(lfsr_log_probability(2, 2), -2);

	EXPECT_EQ(lfsr_log_probability(3, 0), -3);
	EXPECT_EQ(lfsr_log_probability(3, 1), -2);
	EXPECT_EQ(lfsr_log_probability(3, 2), -1);
	EXPECT_EQ(lfsr_log_probability(3, 3), -3);

	EXPECT_EQ(lfsr_log_probability(4, 0), -4);
	EXPECT_EQ(lfsr_log_probability(4, 1), -3);
	EXPECT_EQ(lfsr_log_probability(4, 2), -1);
	EXPECT_EQ(lfsr_log_probability(4, 3), -2);
	EXPECT_EQ(lfsr_log_probability(4, 4), -4);

	EXPECT_EQ(lfsr_log_probability(5, 0), -5);
	EXPECT_EQ(lfsr_log_probability(5, 1), -4);
	EXPECT_EQ(lfsr_log_probability(5, 2), -2);
	EXPECT_EQ(lfsr_log_probability(5, 3), -1);
	EXPECT_EQ(lfsr_log_probability(5, 4), -3);
	EXPECT_EQ(lfsr_log_probability(5, 5), -5);

	EXPECT_EQ(lfsr_log_probability(6, 0), -6);
	EXPECT_EQ(lfsr_log_probability(6, 1), -5);
	EXPECT_EQ(lfsr_log_probability(6, 2), -3);
	EXPECT_EQ(lfsr_log_probability(6, 3), -1);
	EXPECT_EQ(lfsr_log_probability(6, 4), -2);
	EXPECT_EQ(lfsr_log_probability(6, 5), -4);
	EXPECT_EQ(lfsr_log_probability(6, 6), -6);	
}

TEST(linear_complexity, get_expected_probabilities) {
	using V = std::vector<double>;
	EXPECT_EQ(get_expected_probabilities(1), V({.5, .5}));
	EXPECT_EQ(get_expected_probabilities(2), V({.25, .5, .25}));
	EXPECT_EQ(get_expected_probabilities(3), V({.125, .25, .5, .125}));
	EXPECT_EQ(get_expected_probabilities(4), V({.0625, .125, .5, .25, .0625}));

	EXPECT_EQ(get_expected_probabilities(7), V({0.0078125, 0.015625, 0.0625, 0.25, 0.5, 0.125, 0.03125, 0.0078125}));
	EXPECT_EQ(get_expected_probabilities(8), V({0.00390625, 0.0078125, 0.03125, 0.125, 0.5, 0.25, 0.0625, 0.015625, 0.00390625}));
}

std::vector<uint32_t> get_fibonacci_data(uint64_t n, int state_size, uint64_t seed) {
	std::deque<uint32_t> state;
	for (int i = 0; i < state_size; ++i) {
		state.push_back(i+1); //mix64::mx3(seed+i));
	}

	auto fib = [state]() mutable {
		auto r = state.front() + state.back();
		state.pop_front();
		state.push_back(r);
		return r;
	};

	std::vector<uint32_t> data;
	for (uint64_t i = 0; i < n; ++i) {
		data.push_back(fib());
	}
	return data;
}

TEST(linear_complexity, fib_bm_mod) {
	for (auto l = 2; l < 100; ++l) {
		auto s = get_fibonacci_data(2*l, l, 123);
		std::vector<int> bits;
		for (const auto v : s) {
			bits.push_back(v % 2);
		}
		EXPECT_EQ(berlekamp_massey(bits), l);
	}
}

TEST(linear_complexity, fibonacci) {
	auto n = 1024;
	auto s = linear_complexity_stats(
		n,
		create_stream_from_data("test", get_fibonacci_data(n, 5, 123)),
		10
	);
	EXPECT_LT(s->p_value, 1e-9);
}

}
