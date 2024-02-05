# 32-bit results
_While TFR is new you should take the results with a grain of salt._

Tests stop when 2^35 stream elements have been tested. A failed test with '*' suffix indicates the meta analysis failed (non uniform p-values).

Source|TFR|Failures|
-|-|-|
file32::trng|>22|
file32::drng|>25|
mix32::mx1|10|runs, uniform, mean*
mix32::mx2|18|uniform
mix32::mx3|27|uniform*
mix32::prospector|18|uniform*
mix32::prospector\_boost|13|uniform
mix32::murmur|14|sac*
mix32::h2sql|15|sac*
mix32::wang\_1|10|runs
mix32::jenkins|10|runs, mean*, uniform*
rng32::mx1|18|uniform*
rng32::mx2|27|uniform*
rng32::mx3|27|uniform*
rng32::mx1\_64|>35|
rng32::mx2\_64|>35|
rng32::mx3\_64|>35|
rng32::splitmix\_64|>35|
rng32::pcg\_64|>35|
rng32::xoshiro128\+\_128|26|linear-complexity:380:bit(0)
rng32::xoshiro128\+\+\_128|>35|
rng32::sfc\_128|>35|
rng32::mt19937|>35|
rng32::xorshift|16|serial-avalanche:bit(9)*
rng32::xorshift\_64|24|binary-rank:64, binary-rank:64:bit(0), linear-complexity:141:bit(0)
rng32::minstd\_rand|10|mean, uniform
combine32::mx1|10|runs
combine32::mx2|25|serial-avalanche:bit(23), serial-avalanche:bit(24), serial-avalanche:bit(8)
combine32::mx3|27|uniform
combine32::boost|10|mean, uniform
