#include <iostream>
#include <cstdint>
#include <stdio.h>

#ifdef BIT16
using sspint = int16_t;
using spint = uint16_t;
using udpint = uint32_t;
using dpint = uint32_t;	

constexpr spint Wordlength = 16;
constexpr spint Nlimbs = 20;
constexpr spint Radix = 13;
constexpr spint Nbits = 255;
constexpr spint Nbytes = 32;

#elif BIT32
using sspint = int32_t;
using spint = uint32_t;
using udpint = uint64_t;
using dpint = uint64_t;

constexpr spint Wordlength = 32;
constexpr spint Nlimbs = 9;
constexpr spint Radix = 29;
constexpr spint Nbits = 255;
constexpr spint Nbytes = 32;

#else
using sspint = int64_t;
using spint = uint64_t;
using udpint = __uint128_t;
using dpint = __uint128_t;

constexpr spint Wordlength = 64;
constexpr spint Nlimbs = 5;
constexpr spint Radix = 51;
constexpr spint Nbits = 255;
constexpr spint Nbytes = 32;
#endif

constexpr spint OverflowBits = Radix * Nlimbs - Nbits;
constexpr spint FinalLimbRadix = Radix - OverflowBits; 
constexpr spint LastLimbOverflows = Nbits < Radix * Nlimbs;

constexpr spint CongruentFactor = 0x13;
constexpr spint ReductionFactor = CongruentFactor * (1 << OverflowBits);

void mod_mul(spint x[20], spint y[20], spint z[20]);
