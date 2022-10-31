#pragma once

#include <cmath>


inline double normal_cdf(double z) {
	static const double sqrt_2 = std::sqrt(2);
	return std::erfc(-z / sqrt_2) / 2.;
}

inline double two_tailed_normal_cdf(double z) {
	return 2. * normal_cdf(-std::abs(z));
}


// https://github.com/codeplea/incbeta
constexpr double STOP = 1.0e-8;
constexpr double TINY = 1.0e-30;

inline double incomplete_beta(double a, double b, double x) {
	if (x < 0.0 || x > 1.0) return INFINITY;

	/*The continued fraction converges nicely for x < (a+1)/(a+b+2)*/
	if (x > (a + 1.0) / (a + b + 2.0)) {
		return (1.0 - incomplete_beta(b, a, 1.0 - x)); /*Use the fact that beta is symmetrical.*/
	}

	/*Find the first part before the continued fraction.*/
	const double lbeta_ab = lgamma(a) + lgamma(b) - lgamma(a + b);
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

inline double incomplete_beta_(double a, double b, double x) {
	if (x > (a + 1.0) / (a + b + 2.0)) {
		return (1.0 - incomplete_beta(b, a, 1.0 - x)); /*Use the fact that beta is symmetrical.*/
	}
	return 2 * incomplete_beta(a, b, x);
}

// gamma from: https://github.com/samtools/htslib/blob/develop/kfunc.c
inline double kf_lgamma(double z) {
	double x = 0;
	x += 0.1659470187408462e-06 / (z + 7);
	x += 0.9934937113930748e-05 / (z + 6);
	x -= 0.1385710331296526 / (z + 5);
	x += 12.50734324009056 / (z + 4);
	x -= 176.6150291498386 / (z + 3);
	x += 771.3234287757674 / (z + 2);
	x -= 1259.139216722289 / (z + 1);
	x += 676.5203681218835 / z;
	x += 0.9999999999995183;
	return log(x) - 5.58106146679532777 - z + (z - 0.5) * log(z + 6.5);
}

constexpr double M_SQRT2 = 1.4142135623730950488016887242096980785696718753769480731766797379;

inline double kf_erfc(double x) {
	const double p0 = 220.2068679123761;
	const double p1 = 221.2135961699311;
	const double p2 = 112.0792914978709;
	const double p3 = 33.912866078383;
	const double p4 = 6.37396220353165;
	const double p5 = .7003830644436881;
	const double p6 = .03526249659989109;
	const double q0 = 440.4137358247522;
	const double q1 = 793.8265125199484;
	const double q2 = 637.3336333788311;
	const double q3 = 296.5642487796737;
	const double q4 = 86.78073220294608;
	const double q5 = 16.06417757920695;
	const double q6 = 1.755667163182642;
	const double q7 = .08838834764831844;
	double expntl, z, p;
	z = fabs(x) * M_SQRT2;
	if (z > 37.) return x > 0. ? 0. : 2.;
	expntl = exp(z * z * - .5);
	if (z < 10. / M_SQRT2) // for small z
		p = expntl * ((((((p6 * z + p5) * z + p4) * z + p3) * z + p2) * z + p1) * z + p0)
			/ (((((((q7 * z + q6) * z + q5) * z + q4) * z + q3) * z + q2) * z + q1) * z + q0);
	else p = expntl / 2.506628274631001 / (z + 1. / (z + 2. / (z + 3. / (z + 4. / (z + .65)))));
	return x > 0. ? 2. * p : 2. * (1. - p);
}


constexpr double KF_GAMMA_EPS = 1e-14;
constexpr double KF_TINY = 1e-290;

// regularized lower incomplete gamma function, by series expansion
inline double _kf_gammap(double s, double z) {
	double sum, x;
	int k;
	for (k = 1, sum = x = 1.; k < 100; ++k) {
		sum += (x *= z / (s + k));
		if (x / sum < KF_GAMMA_EPS) break;
	}
	return exp(s * log(z) - z - kf_lgamma(s + 1.) + log(sum));
}

// regularized upper incomplete gamma function, by continued fraction
inline double _kf_gammaq(double s, double z) {
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
	return exp(s * log(z) - z - kf_lgamma(s) - log(f));
}

inline double kf_gammap(double s, double z) {
	return z <= 1. || z < s ? _kf_gammap(s, z) : 1. - _kf_gammaq(s, z);
}

inline double kf_gammaq(double s, double z) {
	return z <= 1. || z < s ? 1. - _kf_gammap(s, z) : _kf_gammaq(s, z);
}

inline double gamma_q(double s, double z) {
	return kf_gammap(s, z);
}


//https://github.com/odelalleau/PLearn/blob/76684e2a2af134859be2ef621e84af0dbd838f2e/plearn/math/stats_utils.cc
inline double kolmogorov_smirnov_probability(double D, double N, int conv) {
	double res = 0.0;
	const double sn = sqrt(N);
	const double ks = D * (sn + 0.12 + 0.11 / sn);
	const double ks2 = ks * ks;
	for (int k = 1; k <= conv; k++) {
		double x = ((k % 2) ? 1 : -1) * exp(-2 * ks2 * k * k);
		if (k == conv)
			res += 0.5 * x;
		else
			res += x;
	}
	return 2 * res;
}
