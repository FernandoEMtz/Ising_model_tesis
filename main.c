#include <stdio.h>
#include "pcg_variants.h"

#include "gestor_red.h"
//Los .c van en /src y los .h en /include
int main(void) {

    /*
    caract_red_t red = genera_red();
    red.largo_red;
    inicia_randomizador_pcg64(&red.state_ini, 20260830, 1);
    inicio_red_spines(&red);
    printf("Pruebas de funcionamiento\n");
    //printf("%u", red.largo_red);
    double aux;
    for (int i = 0; i < red.n_sp; i++) {
        aux = aux + red.spin[i];
        aux=aux/red.n_sp;
        printf("%lf\n", aux);
    }
    libera_memoria_red(&red);

    pcg64_random_t rng;

    pcg64_srandom_r(&rng, PCG_128BIT_CONSTANT(0,20260830), PCG_128BIT_CONSTANT(0,1));

    for (int i=0; i<3; i++) {
        printf("%llu\n", (unsigned long long) pcg64_random_r(&rng));
    }*/

    return 0;
}
