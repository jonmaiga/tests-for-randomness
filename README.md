# Tests for randomness

A collection of tests for randomness.

## Background
I've previously used PractRand for randomness evaluation. The goal of this project was to get a better understanding of how the actual tests and the statistics behind it work.

## Tests
- Mean
- Uniform
- Runs
- Bit correlation
- Gap
- Coupon
- Divisibility
- Permutation
- Strict Avalanche Criterion (for mixers)
- Bit Independence Criterion (for mixers)

## Randomness
- PRNGs
- Mixers
- Combiners
- TODO Hashers

## Tools
- Test random sources
- Search for better random sources
- Inspect tests

## Some 64-bit results

Mixer|TFR|PractRand RRC|
-|-|-|
xm2x|||
splitmix|17||
murmur3|10||

Prng|TFR|PractRand|
-|-|-|
xm2x|||
pcg|||
xoroshift128\+\+|||

Combiners|TFR|PractRand|
-|-|-|
xm2x|||


## Some 32-bit results

Mixer|TFR|PractRand RRC|
-|-|-|
xm2x|18|18|
prospector|18|16|
murmur|12||

Prng|TFR|PractRand|
-|-|-|
xm2x|||
pcg|>35||
xoroshift128\+\+|>35||


Combiners|TFR|PractRand|
-|-|-|
xm2x|||

