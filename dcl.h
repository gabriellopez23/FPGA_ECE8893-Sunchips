#include <cmath>
#include <iostream>
#include <cstdint>
#include <stdio.h>
#include <ap_int.h>
#include <hls_vector.h>

#ifdef BIT16
using sspint = int16_t;
using spint = uint16_t;
using udpint = uint32_t;
using dpint = uint32_t;	

constexpr spint Wordlength = 16;
constexpr spint Nlimbs = 20;
constexpr spint Radix = 13;

#elif BIT32
using sspint = int32_t;
using spint = uint32_t;
using udpint = uint64_t;
using dpint = uint64_t;

constexpr spint Wordlength = 32;
constexpr spint Nlimbs = 9;
constexpr spint Radix = 29;

#else
using sspint = int64_t;
using spint = uint64_t;
using udpint = __uint128_t;
using dpint = __uint128_t;

constexpr spint Wordlength = 64;
constexpr spint Nlimbs = 5;
constexpr spint Radix = 51;
#endif

using spint_vec = hls::vector<spint, Nlimbs>;

// Only way to get number of bits at compile time in C++14
static constexpr unsigned numberOfBits(unsigned x)
{
    return x < 2 ? x : 1 + numberOfBits(x >> 1);
}



constexpr spint Nbits = 255;
constexpr spint Nbytes = 32;
constexpr spint OverflowBits = Radix * Nlimbs - Nbits;
constexpr spint FinalLimbRadix = Radix - OverflowBits; 
constexpr spint LastLimbOverflows = Nbits < Radix * Nlimbs;

constexpr dpint CongruentFactor = 0x13; // 19
constexpr dpint ReductionFactor = CongruentFactor * (1 << OverflowBits);

// Calculates the number of bits to avoid overflow 
constexpr spint TermWidth = 2 * Radix + numberOfBits(ReductionFactor) + numberOfBits(Nlimbs - 1) + 1;
using somethign = ap_uint<TermWidth>; 

void mod_mul(spint_vec& x, spint_vec& y, spint_vec& z);
