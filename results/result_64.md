# 64-bit results
_While TFR is new you should take the results with a grain of salt._

Tests stop at 2^30 stream elements have been tested.

Source|TFR|Failures|
-|-|-|
file64::trng|>22|
file64::drng|>25|
mix64::mx3|>30|
mix64::nasam|>30|
mix64::xm2x|17|sac
mix64::lea64|12|sac
mix64::degski64|15|coupon, gap, runs
mix64::splitmix\_v13|17|sac
mix64::splitmix|17|sac
mix64::murmur3|10|gap, runs, uniform
mix64::xmx|10|bic, coupon, divisibility, gap, mean, permutation, runs, sac, uniform
mix64::xxh3|10|bic, coupon, divisibility, gap, mean, permutation, runs, sac, uniform
mix64::fast\_hash|10|bic, coupon, divisibility, gap, mean, permutation, runs, sac, uniform
rng64::pcg|>30|
rng64::xoroshiro128\+|>30|
rng64::xorshift128\+|>30|
rng64::xmx|30|bc2d
rng64::xm2x|>30|
rng64::xm3x|>30|
