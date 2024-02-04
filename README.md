# Tests for Randomness (TFR)

A collection of tests for randomness.

## Overview

Arbitrary streams are passed through a set of tests which uses different statistical methods to check for randomness.

TFR is inspired by the excellent [PractRand](https://pracrand.sourceforge.net/) randomness test. For mixers Pelle Evensen's powerful [RRC](http://mostlymangling.blogspot.com/2019/01/better-stronger-mixer-and-test-procedure.html) test is used by default.

TFR uses `uint8_t`, `uint16_t`, `uint32_t` or `uint64_t` streams.

Uses c++20, cmake and is tested with msvc, clang and gcc.

## Install
Uses external dependency on [googletest-release-1.12.1](https://github.com/google/googletest/releases/tag/release-1.12.1) just place it directly under your cloned directory.

## Run
To run all the tests type `tfr-tool root-path -test` where `root-path` is a path where reports will be saved (e.g. `c:/tmp/tfr/`)

## Tests
- Mean
- Uniform
- Runs
- Serial avalanche
- Gap
- Coupon
- Divisibility
- Permutation
- Binary rank
- Linear complexity
- Strict Avalanche Criterion (for mixers)
- Bit Independence Criterion (for mixers)

Many of the tests are pretty standard and can be found on wikipedia or by googling.

### Divisibility test
The divisibility test is a variation on the coupon test, where we collect numbers that are divisible by some constant. For example every second number is divisible by two, every third by three and so on. Statistically this should hold for randomness too.

### Serial avalanche test
For mixers we can employ sac and bic since we can control the input. This is an attempt to do something similar for prngs (that uses an internal state). It basically creates a matrix of the consecutive stream outputs counting the number of bits set. Each row in the matrix (with enough information) should be approximately binomially distributed.

### Permutation test
This tests uniformity of increasing sizes of bit permutations. Appears to be able to detect mixer bias where other tests do not.

### Strong tests
Empirically it appears that `Uniform`, `Serial avalanche`, `Permutation`, `Binary rank`, `Linear complexity` and `Strict Avalanche Criterion` are good worthwhile tests.

The others can/should probably be disabled for data larger than ~`2^25`.

## Randomness
- PRNGs `f()`
- Mixers `f(x)`
- Combiners `f(x,y)`
- TODO Hashers `f(x1,...,xn)`

## Tools
- Test for randomness (`-test`)
- Search for better randomness (`-search`)
- Inspect tests (`-inspect-test`)

## Unittests
There is some coverage especially over the more complicated parts such as different special math functions.

## Results
I've added some initial results for [32-bits](results/result_32.md) and [64-bits](results/result_64.md).

## Contributions
This is nowhere finished work, for some unfinished tasks please see the incomplete issue list. Feel free to open issues and PRs, there is plenty of work to do. 

_Disclaimer: I'm not a statistician and have probably made a lot of mistakes..._
