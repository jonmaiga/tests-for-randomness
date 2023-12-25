#include <statistics/linear_complexity.h>

#include <gtest/gtest.h>

#include "testutil.h"
#include "util/timer.h"

namespace tfr {

// adapted from: https://mathematica.stackexchange.com/questions/21132/efficient-implementation-of-a-linear-complexity-measure-of-binary-sequences
int berlekamp_massey(const std::vector<int>& u) {
	const auto len = u.size();
	std::vector<int> b(len, 0);
	std::vector<int> c(len, 0);
	b[0] = c[0] = 1;

	int l = 0;
	int m = 0;

	for (int n = 1; n <= len; ++n) {
		int s = u[n-1];
		for (int j = 1; j <= l; ++j) {
			s ^= c[j] & u[n-j-1];  // instead of s += c[j] * u[n-j-1]; s toggles between 0 and 1
		}
		
		if (s != 0) {
			const int from = n - m;
			const int to = from + l;
			const std::vector<int> b_tmp(cbegin(b), cbegin(b) + l + 1);
			if (2 * l <= n) {
				l = n - l;
				m = n;
				b = c;
			}
			
			for (int j = from; j <= to; ++j) {
				c[j] ^= b_tmp[j - from]; // same as c[j] = (c[j] + bsub[j - from]) % 2;
			}
		}
	}
	return l;
}

template<typename T>
int berlekamp_massey_ref(const std::vector<T>& s) {
	using namespace std;
    int n = (int) s.size(), l = 0, m = 1;
    vector<T> b(n), c(n);
    T ld = b[0] = c[0] = 1;
    for (int i=0; i<n; i++, m++) {
        T d = s[i];
        for (int j=1; j<=l; j++)
            d += c[j] * s[i-j];
        if (d == 0)
            continue;
        vector<T> temp = c;
        T coef = d / ld;
        for (int j=m; j<n; j++)
            c[j] -= coef * b[j-m];
        if (2 * l <= i) {
            l = i + 1 - l;
            b = temp;
            ld = d;
            m = 0;
        }
    }
    return l;
}

template<int MOD>
struct ModInt {
    long long v;
    ModInt(long long _v = 0) {v = (-MOD < _v && _v < MOD) ? _v : _v % MOD; if (v < 0) v += MOD;}
    ModInt& operator += (const ModInt &other) {v += other.v; if (v >= MOD) v -= MOD; return *this;}
    ModInt& operator -= (const ModInt &other) {v -= other.v; if (v < 0) v += MOD; return *this;}
    ModInt& operator *= (const ModInt &other) {v = v * other.v % MOD; return *this;}
    ModInt& operator /= (const ModInt &other) {return *this *= inverse(other);}
    bool operator == (const ModInt &other) const {return v == other.v;}
    bool operator != (const ModInt &other) const {return v != other.v;}
    friend ModInt operator + (ModInt a, const ModInt &b) {return a += b;}
    friend ModInt operator - (ModInt a, const ModInt &b) {return a -= b;}
    friend ModInt operator * (ModInt a, const ModInt &b) {return a *= b;}
    friend ModInt operator / (ModInt a, const ModInt &b) {return a /= b;}
    friend ModInt operator - (const ModInt &a) {return 0 - a;}
    friend ModInt power(ModInt a, long long b) {ModInt ret(1); while (b > 0) {if (b & 1) ret *= a; a *= a; b >>= 1;} return ret;}
    friend ModInt inverse(ModInt a) {return power(a, MOD - 2);}
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
	{{0,0}, 0},
	{{0,1}, 2},
	{{1,0}, 1},
	{{1,1}, 1},
	{{0,0,1}, 3},
	{{1,0,1}, 2},
	{{1,1,1}, 1},
	{{1,1,0,1,0,1,1,1,1,0,0,0,1}, 4},
	{{1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0}, 2},
	{{1,0,1,0,0,1,1,1,0,1,0,0,1,1,1,0,1,0,0,1,1,1,0,1,0,0,1,1,1,0,1,0,0,1,1,1,0,1,0,0,1}, 3},
	{{1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1}, 4},
	{{1,0,1,1,0,1,0,1,0,1,1,0,1,0,1,0,1,1,0,1,0,1,0,1,1,0,1,0,1,0,1,1,0,1}, 6},
	{{1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1}, 7},
	{{0,1,0,0,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1}, 16}
}; 

TEST(linear_complexity, berlekamp_massey) {
	for (const auto& test : bm_tests) {
		EXPECT_EQ(bm_ref(test.bit_sequence), test.expected_complexity);
		EXPECT_EQ(berlekamp_massey(test.bit_sequence), test.expected_complexity);
	}
}

TEST(linear_complexity, berlekamp_massey_cross) {
	auto mix = get_default_mixer<uint64_t>();
	for (int i = 1; i <= 256; ++i) {
		std::vector<int> bs;
		for (int j = 1; j <= i; ++j) {
			bs.push_back(mix(i*j) % 2);
		}
		EXPECT_EQ(bm_ref(bs), berlekamp_massey(bs));
	}
}

template<typename T>
void benchmark_test(const T& impl) {
	auto mix = get_default_mixer<uint64_t>();
	uint64_t s = 0;
	uint64_t total_ms = 0;
	for (int i = 1; i <= 2*1024; ++i) {
		std::vector<int> bs;
		for (int j = 1; j <= i; ++j) {
			bs.push_back(mix(i*j) % 2);
		}
		timer t;
		s += impl(bs);
		total_ms += t.milliseconds();
	}
	std::cout << total_ms << "ms\n";
}

TEST(linear_complexity, berlekamp_massey_perf) {
	benchmark_test(bm_ref);
	benchmark_test(berlekamp_massey);
}

TEST(linear_complexity, no_change) {
	using T = uint64_t;
	const auto stat = linear_complexity_stats<T>(1 << 12, test_stream(), 500);
	EXPECT_NEAR(stat->value, 2.7871, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.24822, 1e-4);
}

}
