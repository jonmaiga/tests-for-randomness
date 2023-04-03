# 32-bit results
_While TFR is new you should take the results with a grain of salt._

Tests stop at 2^35 stream elements have been tested.

Source|TFR|Failures|
-|-|-|
file32::trng|>22|
file32::drng|>25|
mix32::xm3x|27|sac, uniform
mix32::xm2x|18|bic, sac, uniform
mix32::prospector|18|bic, uniform
mix32::prospector\_boost|12|sac
mix32::h2sql|13|sac
mix32::murmur|12|sac
mix32::xmx|10|bic, coupon, divisibility, gap, mean, permutation, runs, sac, uniform
mix32::wang\_1|10|bic, coupon, gap, runs, sac
mix32::jenkins|10|bic, coupon, gap, mean, runs, sac
rng32::xm3x|26|bc2d, uniform
rng32::xm2x|22|bc2d, uniform
rng32::xmx|20|bc2d
rng32::pcg\-1|>35|
rng32::xoshiro128\+\+|>35|
rng32::mt19937|>35|
rng32::xorshift|16|bc2d
rng32::minstd\_rand|10|coupon, gap, mean, permutation, uniform
combine32::xmx|10|coupon, divisibility, gap, mean, permutation, runs, uniform
combine32::xm2x|10|coupon, divisibility, gap, runs, uniform
combine32::xm3x|25|uniform
combine32::boost|10|coupon, divisibility, gap, mean, permutation, runs, uniform
