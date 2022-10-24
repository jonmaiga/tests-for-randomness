#include "chi2.h"

#include <cmath>

namespace chi2 {

constexpr double Pi = 3.141592653589793238463;
constexpr int MAX_X = 20;
const double LOG_SQRT_PI = log(sqrt(Pi));
const double I_SQRT_PI = 1 / sqrt(Pi);

double poz(double z) {
	double y, x, w;
	double Z_MAX = 6.0; // Maximum meaningful z value
	if (z == 0.0) {
		x = 0.0;
	}
	else {
		y = 0.5 * fabs(z);
		if (y >= (Z_MAX * 0.5)) {
			x = 1.0;
		}
		else if (y < 1.0) {
			w = y * y;
			x = ((((((((0.000124818987 * w
							- 0.001075204047) * w + 0.005198775019) * w
						- 0.019198292004) * w + 0.059054035642) * w
					- 0.151968751364) * w + 0.319152932694) * w
				- 0.531923007300) * w + 0.797884560593) * y * 2.0;
		}
		else {
			y -= 2.0;
			x = (((((((((((((-0.000045255659 * y
										+ 0.000152529290) * y - 0.000019538132) * y
									- 0.000676904986) * y + 0.001390604284) * y
								- 0.000794620820) * y - 0.002034254874) * y
							+ 0.006549791214) * y - 0.010557625006) * y
						+ 0.011630447319) * y - 0.009279453341) * y
					+ 0.005353579108) * y - 0.002141268741) * y
				+ 0.000535310849) * y + 0.999936657524;
		}
	}
	return z > 0.0 ? ((x + 1.0) * 0.5) : ((1.0 - x) * 0.5);
}

double ex(double x) { return (x < -MAX_X) ? 0.0 : exp(x); }

double inverse(double x, int df) {
	double a, s;
	double e, c, z;

	if (x <= 0.0 || df < 1) {
		return 1.0;
	}
	a = 0.5 * x;
	bool even = (df & 1) == 0;
	double y = 0;
	if (df > 1) {
		y = ex(-a);
	}
	s = (even ? y : (2.0 * poz(-sqrt(x))));
	if (df > 2) {
		x = 0.5 * (df - 1.0);
		z = (even ? 1.0 : 0.5);
		if (a > MAX_X) {
			e = (even ? 0.0 : LOG_SQRT_PI);
			c = log(a);
			while (z <= x) {
				e = log(z) + e;
				s += ex(c * z - a - e);
				z += 1.0;
			}
			return s;
		}
		else {
			e = (even ? 1.0 : (I_SQRT_PI / sqrt(a)));
			c = 0.0;
			while (z <= x) {
				e = e * (a / z);
				c = c + e;
				z += 1.0;
			}
			return c * y + s;
		}
	}
	else {
		return s;
	}
}


}
