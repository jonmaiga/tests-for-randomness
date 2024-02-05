# 64-bit results
_While TFR is new you should take the results with a grain of salt._

Tests stop when 2^35 stream elements have been tested. A failed test with '*' suffix indicates the meta analysis failed (non uniform p-values).

Source|TFR|Failures|
-|-|-|
file64::trng|>22|
file64::drng|>25|
mix64::mx1|10|runs, uniform, mean*
mix64::mx2|19|permutation:13*
mix64::mx3|29|permutation:21*
mix64::nasam|21|permutation:15*
mix64::moremur|10|runs
mix64::lea64|14|sac*
mix64::degski64|15|runs
mix64::splitmix\_v13|19|sac*
mix64::splitmix|19|sac*
mix64::murmur3|10|runs
mix64::xxh3|10|runs, uniform, mean*
mix64::fast\_hash|10|runs, uniform, mean*
rng64::mx1|28|uniform*
rng64::mx2|>35|
rng64::mx3|>35|
rng64::splitmix|>35|
rng64::pcg|>35|
rng64::xorshift|17|serial-avalanche:bit(25)*
rng64::xorshift128\+\_128|26|binary-rank:128:bit(0), linear-complexity:380:bit(0)
rng64::xoroshiro128\+\_128|26|binary-rank:128:bit(0), linear-complexity:380:bit(0)
rng64::sfc\_256|>35|
