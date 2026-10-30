//
// Created by Fernando on 27/10/2026.
//

#include "gestor_de_red.h"
#include "pcg_variants.h"

#include <stdio.h>
#include <stdlib.h>

//Recibimos los datos para crear cada red, será de tamaño dinamico, con base al largo que recibiremos

caract_red_t genera_red(int largo_red) {
    caract_red_t alm_red;

    if (largo_red < 4) {
        printf("Error al crear la red, largo muy pequenio\n");
        exit(EXIT_FAILURE);
    }

    alm_red.largo_red = largo_red;//Después podemos cambiar este 16 con el sentido del tamaño creciente de la red

    alm_red.J = 1.0;//Despues podemos cambiar de dirección al cjson
    alm_red.h_campo = 0.0; //Mismo caso que con J
    alm_red.n_vec = 4; //Sirve para las redes simples bidimensionales, podría ser una constante
    alm_red.temp_i = 3.0;
    alm_red.temp_f = 5.0;
    alm_red.n_temp = 2;
    alm_red.energia_total = 0.0;
    alm_red.magn_total = 0;
    alm_red.n_mc = 10;
    alm_red.n_med = 5000;

    alm_red.n_sp = alm_red.largo_red*alm_red.largo_red; //Determinamos que el tamaño del arreglo sea largo*largo
    alm_red.spin= (int8_t *)malloc(alm_red.n_sp * sizeof(int8_t)); //Asignamos memoria dinamica para el arreglo de spines

    if (alm_red.spin == NULL) {
        printf("Error al asignar memoria para %d entradas de la red\n", alm_red.n_sp);
        exit(EXIT_FAILURE);
    }
    return alm_red;
}

void libera_memoria_red(caract_red_t *red) {
    free(red->spin);
    red->spin = NULL;
    red->n_sp = 0;
}

//Se controla la semilla de la red
void inicia_randomizador_pcg64(pcg64_random_t *random_local, uint64_t state_init, uint64_t state_seq) {
    pcg64_srandom_r(random_local, (pcg128_t)state_init, (pcg128_t)state_seq);
}

double ejecuta_randomizador_pcg64(pcg64_random_t *random_local) {
    uint64_t bits_53 = pcg64_random_r(random_local) >> 11;
    return (double)bits_53/(1ULL << 53);
}

void inicio_red_spines(caract_red_t *ejecucion) {
    int i = 0;
    double randomcito = 0.0;

    for (i = 0; i < ejecucion->n_sp; i++) {
        randomcito = ejecuta_randomizador_pcg64(&ejecucion->generador_rng);
        //printf("%f\n", randomcito);
        ejecucion->spin[i] = (randomcito < 0.5) ? -1 : 1;
    }
}

int funcion_suma_de_vecinos(const caract_red_t *ejecucion, int i) {
    int j = 0, k = 0, up = 0, dw = 0, lf = 0, rg = 0;
    int largo = ejecucion->largo_red;

    j = i/largo; //Se obtiene la fila
    k = i%largo; //Se obtiene la columna

    up = (j == 0) ? ejecucion->spin[i + largo * (largo - 1)] : ejecucion->spin[i - largo];
    dw = (j == largo - 1) ? ejecucion->spin[i - largo * (largo - 1)] : ejecucion->spin[i + largo];
    lf = (k == 0) ? ejecucion->spin[i + (largo - 1)] : ejecucion->spin[i -1];
    rg = (k == largo - 1) ? ejecucion->spin[i - (largo - 1)] : ejecucion->spin[i + 1];

    return (up + dw + lf + rg);
}