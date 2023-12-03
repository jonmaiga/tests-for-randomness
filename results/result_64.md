# 64-bit results
_While TFR is new you should take the results with a grain of salt._

Tests stop at 2^35 stream elements have been tested.

Source|TFR|Failures|
-|-|-|
file64::trng|>22|
file64::drng|>25|
mix64::mx1|10|bic, coupon, divisibility, gap, mean, permutation, runs, sac, uniform
mix64::mx2|21|sac
mix64::mx3|>35|
mix64::nasam|33|sac
mix64::lea64|12|sac
mix64::degski64|15|coupon, gap, runs
mix64::splitmix\_v13|17|sac
mix64::splitmix|17|sac
mix64::murmur3|10|gap, runs, uniform
mix64::xxh3|10|bic, coupon, divisibility, gap, mean, permutation, runs, sac, uniform
mix64::fast\_hash|10|bic, coupon, divisibility, gap, mean, permutation, runs, sac, uniform
rng64::xmx|30|bc2d
rng64::xm2x|>35|
rng64::xm3x|>35|
rng64::pcg|>35|
rng64::xoroshiro128\+|>35|
rng64::xorshift128\+|>35|


# Old versions
Source|TFR|Failures|Config
-|-|-|-
mix64::xm2x|17|sac|32-0xe9846af9b1a615d-32-0xe9846af9b1a615d-28 (now mx2)