# Defold - RNG

Multi-algorithm Random Number Generator Native Extension for the Defold Game Engine, using instances (not globals)

The idea for this extension came from [defold-random](https://github.com/selimanac/defold-random), but i decided to improve it and widen the scope.

This extension has 3 aspects:
* Provide instance based Random Number Generators, not a single global RNG. This benefits procedural generation
* Support for multiple algorithms (the best/fastest/safest). Currently we have PCG32, TinyMT32 and Xoshiro128**
* Aimed at performance, implemented in minimal C (with a C++ wrapper for Lua binding)


## Supported algorithms

### PCG32

Regarding PCG32, this extension allow you to generate random numbers using minimal [C implementation of PCG](http://www.pcg-random.org/using-pcg-c-basic.html).

It uses [entropy](https://github.com/imneme/pcg-c/blob/master/extras/entropy.c) seed internally with fallback to time based seed. You can switch to Time based seed and remove the entropy by uncommenting/commenting a few lines on the source code, but I don't think it is necessary. 


### TinyMT32 (Tiny Mersenne Twister)

As the original Mersene Twister has a relatively large buffer (~2.5kb) for instance based RNG i decided to go for [TinyMT](https://github.com/MersenneTwister-Lab/TinyMT), in the 32 bit variant.

The seed takes one argument only, and if none provided, we seed using a table of 4 random values, taken from entropy (just like PCG32)

As the original Mersene Twister has a relatively large buffer (~2.5kb) for instance based RNG i decided to go for [TinyMT](https://github.com/MersenneTwister-Lab/TinyMT), in the 32 bit variant.

The seed takes one argument only, and if none provided, we seed using a table of 4 random values, taken from entropy (just like PCG32)

### Xoshiro128** (short for "xor, shift, rotate" 128 bits)

Xoshiro (https://prng.di.unimi.it/) is a high quality fast RNG, in the family of shift-register generators.
To save memory while keeping the best possible quality, the Xoshiro128** seems to be the best option.

To seed this PRG, you must initialize its 128-bit state array (four 32-bit integers) so that the entire state is not all zeros.
The creators of xoshiro (David Blackman and Sebastiano Vigna) recommend using a different generator—such as SplitMix32—to expand a smaller seed (like a 32-bit integer) into the full 128-bit state array
The seed takes one argument only, and if none provided, we seed using a random table of random values, taken from entropy (just like PCG32)

In addition to the usual methods, here we also have a jump method, which advances the state as-if 2^64 random numbers have been generated. This allows the original sequence to be split so that distinct segments can be used in each worker process.


## Installation
You can use defold-rng in your own project by adding this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/). Open your game.project file and in the dependencies field under project add:

	https://github.com/alchimystic/defold-rng/archive/master.zip
	
---


## Usage

### RNG

*rng* is the global table we can use to create all the generators, providing or not an initial seed.

So as we add new algorithms, we will also add methods to this table.

#### rng.pcg32()

Creates a PCG generator instance with a entropy-based seed. You should use this if you don't care about explicitly providing a seed.

#### rng.pcg32(init_state, init_seq)

Creates a PCG generator instance given the initial state and seq for seed. You should use this to specify the seed.
`init_state` is the starting state for the RNG, you can pass any 64-bit value.  
`init_seq` selects the output sequence for the RNG, you can pass any 64-bit value, although only the low 63 bits are significant.

#### rng.tinymt32()

Creates a TinyMT32 generator instance with a entropy-based seed of 4 elements. You should use this if you don't care about explicitly providing a seed.

#### rng.tinymt32(seed)

Creates a TinyMT32 generator wth a given seed. You can pass any 64-bit value, but 0 will have the same result as a random seed.
You should use this to specify the seed.

**Caution:** I don't recommend using 64-bit integers. Consider using 32-bit integers instead.  

#### rng.xoshiro128()

Creates a xoshiro128** generator instance with a entropy-based seed of 4 elements. You should use this if you don't care about explicitly providing a seed.

#### rng.xoshiro128(seed)



### Common Methods
  

| Method | Params | Description|
|--------|--------|------------|
| number | - | Returns a 32 bit unsigned integer |
| range | min, max | Returns a 32 bit unsigned integer between min and max values. Only for positive numbers. Same as **math.random(3,20)** |
| double | - | Returns a floating point between 0-1. Same as **math.random()** |
| double_range | min, max | Returns a floating point between min and max |
| toss       | - | Toss a coin. Returns 0 or 1
| roll | - | Roll the dice. Returns between 1 and 6   |



### Setting the seed


| Algorithm   | Method | Description|
|-------------|--------|------------|
| PCG32       | seed(`init_state`, `init_seq`) | Sets the new seed for this instance, given state and seq |
| TinyMT32    | seed(`seed`) | Sets the new seed for this instance, given the value. If seed == 0, it uses a random seed based on entropy |
| Xoshiro128**| seed(`seed`) | Sets the new seed for this instance, given the value |



## Release Notes

1.3

Added support for Xoshiro128**

1.2

Added actual examples to example/main.script

1.1

Added support for TinyMT32 (Tiny Mersenne Twister, 32 bits)

1.0

Initial release.

- rng: supports creation of pcg32 instance with random seed (based on entropy)
- rng: supports creation of pcg32 instance with initial state and increment seed
- pcg32 instance supports: seed, number, double, range, double_range, toss and roll
