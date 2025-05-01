#include "dcl.h"

void mod_mul(spint_vec& x, spint_vec& y, spint_vec& z) {
#pragma HLS interface mode = axis port = x // offset = slave bundle = mem1
#pragma HLS interface mode = axis port = y // offset = slave bundle = mem2
#pragma HLS interface mode = axis port = z // offset = slave bundle = mem3
#pragma HLS interface s_axilite port = return

  dpint t = 0;
  // spint lo = 0;
  // spint hi = 0;
  spint mask = ((spint)1 << Radix) - (spint)1;

  spint_vec v;
#pragma HLS array_partition variable = v complete

  spint_vec x_buf = x;
  spint_vec y_buf = y;
#pragma HLS array_partition variable = x_buf complete
#pragma HLS array_partition variable = y_buf complete

REDUCTION:
  for (int i = 0; i < Nlimbs; i++) {
#pragma HLS unroll

    dpint tt[Nlimbs];
#pragma HLS array_partition variable = tt complete

  // Calculate high terms (terms that are reduced modulo 2^255 - 19)
  HIGH_TERMS:
    for (int j = 1 + i; j < Nlimbs; j++) {
#pragma HLS unroll factor = (Nlimbs - 1)
      tt[j] = (dpint)x_buf[j] * (dpint)y_buf[Nlimbs - j + i];
    }

    // lo = (spint)tt & mask;
    // t += (dpint)(lo + hi) * ReductionFactor;

  SUM:
    for (int j = 1 + i; j < Nlimbs; j++) {
#pragma HLS unroll factor = (Nlimbs - 1)
      t += (dpint)tt[j] * (dpint)ReductionFactor;
    }
  // t += tt * ReductionFactor;
  // hi = (spint)(tt >> Radix);

  // Calculate direct terms (terms that aren't reduced)
  DIRECT_TERMS:
    for (int j = 0; j < i + 1; j++) {
#pragma HLS unroll factor = Nlimbs
      t += (dpint)x_buf[j] * (dpint)y_buf[i - j];
    }

    v[i] = (spint)t & mask;
    t = t >> Radix;
  }

  // Second reduction pass
  udpint ut = (udpint)t;

  // Reduce the overflow bits of the last term
  if (Nbits < Radix * Nlimbs) {
    ut = (udpint)(ut << OverflowBits) +
         (spint)(v[Nlimbs - 1] >> (FinalLimbRadix));

    // Get the non-overflow bits
    v[Nlimbs - 1] &= ((sspint)1 << FinalLimbRadix) - 1;
  }

  // Reduce the overflow bits of the last term
  ut *= CongruentFactor;

  // Add the reduced bits to the first term
  spint sum = v[0] + ((spint)ut & mask);

  spint_vec z_buf;
#pragma HLS array_partition variable = z_buf complete

  z_buf[0] = (spint)(sum & mask);

  // Get the carry bits from the sum and the overflow bits
  spint carry = (sum >> Radix) + (spint)(ut >> Radix);

  z_buf[1] = v[1] + carry;

// Set the rest of the limbs
SET_LIMBS:
  for (int i = 2; i < Nlimbs; i++) {
#pragma HLS unroll
    z_buf[i] = v[i];
  }

  z = z_buf;
}
