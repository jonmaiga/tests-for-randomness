#include "distributions.h"

#include <algorithm>

#include "util/assertion.h"
#include "util/math.h"

namespace mixer {
// https://github.com/codeplea/incbeta
constexpr double STOP = 1.0e-8;
constexpr double TINY = 1.0e-30;

double beta_regularized(double a, double b, double x) {
	assertion(x >= 0 && x <= 1., "incomplete beta out of range");
	if (x < 0.0 || x > 1.0) return INFINITY;

	/*The continued fraction converges nicely for x < (a+1)/(a+b+2)*/
	if (x > (a + 1.0) / (a + b + 2.0)) {
		return (1.0 - beta_regularized(b, a, 1.0 - x)); /*Use the fact that beta is symmetrical.*/
	}

	/*Find the first part before the continued fraction.*/
	const double lbeta_ab = log_gamma(a) + log_gamma(b) - log_gamma(a + b);
	const double front = exp(log(x) * a + log(1.0 - x) * b - lbeta_ab) / a;

	/*Use Lentz's algorithm to evaluate the continued fraction.*/
	double f = 1.0, c = 1.0, d = 0.0;

	int i, m;
	for (i = 0; i <= 200; ++i) {
		m = i / 2;

		double numerator;
		if (i == 0) {
			numerator = 1.0; /*First numerator is 1.0.*/
		}
		else if (i % 2 == 0) {
			numerator = (m * (b - m) * x) / ((a + 2.0 * m - 1.0) * (a + 2.0 * m)); /*Even term.*/
		}
		else {
			numerator = -((a + m) * (a + b + m) * x) / ((a + 2.0 * m) * (a + 2.0 * m + 1)); /*Odd term.*/
		}

		/*Do an iteration of Lentz's algorithm.*/
		d = 1.0 + numerator * d;
		if (fabs(d) < TINY) d = TINY;
		d = 1.0 / d;

		c = 1.0 + numerator / c;
		if (fabs(c) < TINY) c = TINY;

		const double cd = c * d;
		f *= cd;

		/*Check for stop.*/
		if (fabs(1.0 - cd) < STOP) {
			return front * (f - 1.0);
		}
	}

	return INFINITY; /*Needed more loops, did not converge.*/
}

constexpr double KF_GAMMA_EPS = 1e-14;
constexpr double KF_TINY = 1e-290;

// regularized lower incomplete gamma function, by series expansion
double _kf_gammap(double s, double z) {
	double sum, x;
	int k;
	for (k = 1, sum = x = 1.; k < 100; ++k) {
		sum += (x *= z / (s + k));
		if (x / sum < KF_GAMMA_EPS) break;
	}
	return exp(s * log(z) - z - log_gamma(s + 1.) + log(sum));
}

// regularized upper incomplete gamma function, by continued fraction
double _kf_gammaq(double s, double z) {
	int j;
	double C, D, f;
	f = 1. + z - s;
	C = f;
	D = 0.;
	// Modified Lentz's algorithm for computing continued fraction
	// See Numerical Recipes in C, 2nd edition, section 5.2
	for (j = 1; j < 100; ++j) {
		double a = j * (s - j), b = (j << 1) + 1 + z - s, d;
		D = b + a * D;
		if (D < KF_TINY) D = KF_TINY;
		C = b + a / C;
		if (C < KF_TINY) C = KF_TINY;
		D = 1. / D;
		d = C * D;
		f *= d;
		if (fabs(d - 1.) < KF_GAMMA_EPS) break;
	}
	return exp(s * log(z) - z - log_gamma(s) - log(f));
}

double kf_gammap(double s, double z) {
	return z <= 1. || z < s ? _kf_gammap(s, z) : 1. - _kf_gammaq(s, z);
}

double kf_gammaq(double s, double z) {
	return z <= 1. || z < s ? 1. - _kf_gammap(s, z) : _kf_gammaq(s, z);
}

double gamma_regularized(double s, double z) {
	if (is_near(s, 0) && is_near(z, 0)) {
		return 0.;
	}
	return 1. - kf_gammap(s, z);
}


//https://github.com/odelalleau/PLearn/blob/76684e2a2af134859be2ef621e84af0dbd838f2e/plearn/math/stats_utils.cc
double kolmogorov_smirnov_cdf(double D, double df, int conv) {
	double res = 0.0;
	const double sn = sqrt(df);
	const double ks = D * (sn + 0.12 + 0.11 / sn);
	const double ks2 = ks * ks;
	for (int k = 1; k <= conv; k++) {
		double x = ((k % 2) ? 1 : -1) * exp(-2 * ks2 * k * k);
		if (k == conv)
			res += 0.5 * x;
		else
			res += x;
	}
	return std::clamp(2. * res, 0., 1.);
}


//https: //github.com/satanson/cpp_etudes/blob/fe727db385352ed235236ee6c16e968c3c6cecd6/third-party/abseil-cpp-91b7cd600a34cbd/absl/profiling/internal/exponential_biased_test.cc

// Tests of the quality of the random numbers generated
// This uses the Anderson Darling test for uniformity.
// See "Evaluating the Anderson-Darling Distribution" by Marsaglia
// for details.

// Short cut version of ADinf(z), z>0 (from Marsaglia)
// This returns the p-value for Anderson Darling statistic in
// the limit as n-> infinity. For finite n, apply the error fix below.
double AndersonDarlingInf(double z) {
	if (z < 2) {
		return exp(-1.2337141 / z) / sqrt(z) *
		(2.00012 +
			(0.247105 -
				(0.0649821 - (0.0347962 - (0.011672 - 0.00168691 * z) * z) * z) *
				z) *
			z);
	}
	return exp(
		-exp(1.0776 -
			(2.30695 -
				(0.43424 - (0.082433 - (0.008056 - 0.0003146 * z) * z) * z) * z) *
			z));
}

// Corrects the approximation error in AndersonDarlingInf for small values of n
// Add this to AndersonDarlingInf to get a better approximation
// (from Marsaglia)
double AndersonDarlingErrFix(int n, double x) {
	if (x > 0.8) {
		return (-130.2137 +
				(745.2337 -
					(1705.091 - (1950.646 - (1116.360 - 255.7844 * x) * x) * x) * x) *
				x) /
			n;
	}
	double cutoff = 0.01265 + 0.1757 / n;
	if (x < cutoff) {
		double t = x / cutoff;
		t = sqrt(t) * (1 - t) * (49 * t - 102);
		return t * (0.0037 / (n * n) + 0.00078 / n + 0.00006) / n;
	}
	else {
		double t = (x - cutoff) / (0.8 - cutoff);
		t = -0.00022633 +
			(6.54034 - (14.6538 - (14.458 - (8.259 - 1.91864 * t) * t) * t) * t) *
			t;
		return t * (0.04213 + 0.01365 / n) / n;
	}
}

// Returns the AndersonDarling p-value given n and the value of the statistic
double AndersonDarlingPValue(int n, double z) {
	double ad = AndersonDarlingInf(z);
	double errfix = AndersonDarlingErrFix(n, ad);
	return ad + errfix;
}

double anderson_darling_cdf(double A2, double df) {
	const auto p = 1. - std::clamp(AndersonDarlingPValue(df, A2), 0., 1.);
	assertion(is_valid(p), "anderson darling cdf is not valid");
	return p;
}


}
