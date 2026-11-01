//
// Created by Fernando on 27/10/2026.
//

#include "gestor_de_red.h"
#include "pcg_variants.h"
#include "gestor_de_datos_de_entrada.h"

#include <stdio.h>
#include <stdlib.h>

parametros_de_red_t genera_red(const parametros_t *parametros, int largo_red) {
    //Inicio de struct parametros en cero
    parametros_de_red_t alm_red = {0};

    if (largo_red < 4) {
        printf("Error al crear la red, largo muy pequenio\n");
        exit(EXIT_FAILURE);
    }
    alm_red.largo_red = largo_red;
    alm_red.J_intns_aco = parametros->J_intns_aco;
    alm_red.h_cpo_mgntc_extr = parametros->h_cpo_mgntc_extr;
    alm_red.temp_ini = parametros->temp_ini;
    alm_red.temp_fin = parametros->temp_fin;
    alm_red.n_temp = parametros->n_temp;
    alm_red.num_therm_mc = parametros->num_therm_mc;
    alm_red.num_barr_med = parametros->num_barr_med;
    alm_red.n_sp = alm_red.largo_red*alm_red.largo_red;
    alm_red.arr_spin = (int8_t *)malloc(alm_red.n_sp * sizeof(int8_t));

    if (alm_red.arr_spin == NULL) {
        printf("Error al asignar memoria para %d entradas de la red\n", alm_red.n_sp);
        exit(EXIT_FAILURE);
    }
    return alm_red;
}

void libera_memoria_red(parametros_de_red_t *red) {
    free(red->arr_spin);
    red->arr_spin = NULL;
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

void inicio_red_spines(parametros_de_red_t *ejecucion) {
    int i = 0;
    double randomcito = 0.0;

    for (i = 0; i < ejecucion->n_sp; i++) {
        randomcito = ejecuta_randomizador_pcg64(&ejecucion->generador_rng);
        ejecucion->arr_spin[i] = (randomcito < 0.5) ? -1 : 1;
    }
}

int funcion_suma_de_vecinos(const parametros_de_red_t *ejecucion, int i) {
    int j = 0, k = 0, up = 0, dw = 0, lf = 0, rg = 0;
    int largo = ejecucion->largo_red;

    j = i/largo; //Se obtiene la fila
    k = i%largo; //Se obtiene la columna

    up = (j == 0) ? ejecucion->arr_spin[i + largo * (largo - 1)] : ejecucion->arr_spin[i - largo];
    dw = (j == largo - 1) ? ejecucion->arr_spin[i - largo * (largo - 1)] : ejecucion->arr_spin[i + largo];
    lf = (k == 0) ? ejecucion->arr_spin[i + (largo - 1)] : ejecucion->arr_spin[i -1];
    rg = (k == largo - 1) ? ejecucion->arr_spin[i - (largo - 1)] : ejecucion->arr_spin[i + 1];

    return (up + dw + lf + rg);
}