#include "math.h"

namespace tfr {

double log_gamma(double z) {
	// from: https://github.com/samtools/htslib/blob/develop/kfunc.c
	// which is from here: http://lib.stat.cmu.edu/apstat/245
	// another option is: https://www.johndcook.com/blog/cpp_gamma/ which will take it to 23.x before diverging slightly from std::lgamma
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
	return std::log(x) - 5.58106146679532777 - z + (z - 0.5) * std::log(z + 6.5);
}

}
