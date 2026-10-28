#include <stdio.h>
#include "pcg_variants.h"

int main(void) {
    pcg64_random_t rng;

    pcg64_srandom_r(&rng, PCG_128BIT_CONSTANT(0,20260830), PCG_128BIT_CONSTANT(0,1));

    for (int i=0; i<3; i++) {
        printf("%llu\n", (unsigned long long) pcg64_random_r(&rng));
    }

    return 0;
}
