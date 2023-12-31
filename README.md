# Defold - RNG

Multi-algorithm Random Number Generator Native Extension for the Defold Game Engine, using instances (not globals)

The idea for this extension came from [defold-random](https://github.com/selimanac/defold-random), but i decided to improve it and widen the scope.

This extension has 3 aspects:
* Provide instance based Random Number Generators, not a single global RNG. This benefits procedural generation
* Support for multiple algorithms (the best/fastest/safest). Currently we have PCG32 and TinyMT32
* Aimed at performance, implemented in minimal C (with a C++ wrapper for Lua binding)


## Supported algorithms

### PCG32

Regarding PCG32, this extension allow you to generate random numbers using minimal [C implementation of PCG](http://www.pcg-random.org/using-pcg-c-basic.html).

It uses [entropy](https://github.com/imneme/pcg-c/blob/master/extras/entropy.c) seed internally with fallback to time based seed. You can switch to Time based seed and remove the entropy by uncommenting/commenting a few lines on the source code, but I don't think it is necessary. 


### TinyMT32 (Tiny Mersenne Twister)

As the original Mersene Twister has a relatively large buffer (~2.5kb) for instance based RNG i decided to go for [TinyMT](https://github.com/MersenneTwister-Lab/TinyMT), in the 32 bit variant.

The seed takes one argument only, and if none provided, we seed using a table of 4 random values, taken from entropy (just like PCG32)


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

#### rng.pcg32(`init_state`, `init_seq`)

Creates a PCG generator instance given the initial state and seq for seed. You should use this to specify the seed.
`init_state` is the starting state for the RNG, you can pass any 64-bit value.  
`init_seq` selects the output sequence for the RNG, you can pass any 64-bit value, although only the low 63 bits are significant.

#### rng.tinymt32()

Creates a TinyMT32 generator instance with a entropy-based seed of 4 elements. You should use this if you don't care about explicitly providing a seed.

#### rng.tinymt32(`seed`)

Creates a TinyMT32 generator wth a given seed. You can pass any 64-bit value, but 0 will have the same result as a random seed.
You should use this to specify the seed.


**Caution:** I don't recommend using of 64-bit integers. Consider using 32-bit integers instead. 

### Common Methods (to all RNG instances)

#### number()

Returns a 32 bit unsigned integer.

#### range(`min`, `max`)

Returns a 32 bit unsigned integer between min and max values. Only for positive numbers(unsigned integers).
Same as **math.random(3,20)**  

#### double_num()

Returns a floating point between 0-1.  
Same as **math.random()**

####  double_range(`min`, `max`)

Returns a floating point between min - max.  
Not fast as `double()`

####  toss()

Toss a coin. Returns 0 or 1 (0 = 'H', 1 = 'T')

####  roll()

Roll the dice. Returns between 1-6


### PCG32

Besides the coomon methods, a PCG32 instance has the following methods:

#### seed(`init_state`, `init_seq`)

Sets the new seed for this instance, given state and seq.

### TinyMT32

Besides the coomon methods, a TinyMT32 instance has the following methods:

#### seed(`seed`)

Sets the new seed for this instance, given the actual seed value. If seed == 0, it uses a random seed based on entropy


## Release Notes

1.1

Added support for TinyMT32 (Tiny Mersenne Twister, 32 bits)

1.0

Initial release.

- rng: supports creation of pcg32 instance with random seed (based on entropy)
- rng: supports creation of pcg32 instance with initial state and increment seed
- pcg32 instance supports: seed, number, double_num, range, double_range, toss and roll
