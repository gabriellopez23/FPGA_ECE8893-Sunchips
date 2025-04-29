#include "dcl.h"
#include <stdint.h>

static void inline modmul(const spint *a, const spint *b, spint *c) {
  dpint t = 0;
  spint lo = 0;
  spint hi = 0;
  spint mask = ((spint)1 << Radix) - (spint)1;
  spint v[Nlimbs];

  for (int i = 0; i < Nlimbs; i++) {
    dpint tt = 0;

    // Calculate high terms (terms that are reduced modulo 2^255 - 19)
    for (int j = 1 + i; j < Nlimbs; j++) {
      tt += (dpint)a[j] * (dpint)b[Nlimbs - j + i];
    }

    lo = (spint)tt & mask;
    t += (dpint)(lo + hi) * ReductionFactor;
    hi = (spint)(tt >> Radix);

    // Calculate direct terms (terms that aren't reduced)
    for (int j = 0; j < i + 1; j++) {
      t += (dpint)a[j] * (dpint)b[i - j];
    }

    // Get the result bits
    v[i] = (spint)t & mask;
    // Get the carry bits
    t = t >> Radix;
  }

  // Second reduction pass
  udpint ut = (udpint)t;

  // Reduce the overflow bits of the last term
  if (Nbits < Radix * Nlimbs) {
    ut = (udpint)(ut << OverflowBits) + (spint)(v[Nlimbs - 1] >> (FinalLimbRadix));

    // Get the non-overflow bits
    v[Nlimbs - 1] &= ((sspint)1 << FinalLimbRadix) - 1;
  } 

  // Reduce the overflow bits of the last term
  ut *= CongruentFactor;

  // Add the reduced bits to the first term
  spint sum = v[0] + ((spint)ut & mask);

  c[0] = (spint)(sum & mask);

  // Get the carry bits from the sum and the overflow bits
  spint carry = (sum >> Radix) + (spint)(ut >> Radix);

  c[1] = v[1] + carry;

  // Set the rest of the limbs
  for (int i = 2; i < Nlimbs; i++) {
    c[i] = v[i];
  }
}

void mod_mul(spint x[Nlimbs], spint y[Nlimbs], spint z[Nlimbs]) {
#pragma HLS interface m_axi port = x offset = slave bundle = mem1
#pragma HLS interface m_axi port = y offset = slave bundle = mem2
#pragma HLS interface m_axi port = z offset = slave bundle = mem3
#pragma HLS interface s_axilite port = return

  modmul(x, y, z);
}
