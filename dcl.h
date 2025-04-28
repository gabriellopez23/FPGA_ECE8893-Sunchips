#include <iostream>
#include <stdint.h>
#include <stdio.h>


/*#define sspint int16_t
#define spint uint16_t
#define udpint uint32_t
#define dpint uint32_t*/

#define sspint int64_t
#define spint uint64_t
#define udpint __uint128_t
#define dpint __uint128_t

void mod_mul(spint x[20], spint y[20], spint z[20]);


