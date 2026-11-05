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

//Se controla la semilla de la red con un desplazamiento unico para cada tarea
void inicia_randomizador_pcg64(pcg64_random_t *random_local, pcg128_t semilla_global, pcg128_t id_tarea) {
    pcg64_srandom_r(random_local, (pcg128_t)semilla_global, (pcg128_t)PGC_INIT_SEQ);
    //Desplazamiento calculado para el id
    pcg128_t salto = ((pcg128_t)id_tarea) << 64;
    //Avance en el generador con salto calculado
    pcg64_advance_r(random_local, salto);
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

void reporte_de_simulacion(const parametros_t *parametros, int largo_red) {
    printf("Reporte de configuracion\n\n");
    printf("Semilla global de simulacion:\t\t%llu\n", (unsigned long long)parametros->semilla_global);
    printf("Total de redes a simular:\t\t%d\n", parametros->cont_tam_red);
    printf("Lista de redes a simular:\t\t[");
    for (int i = 0; i < parametros->cont_tam_red; i++) printf(" %d ", parametros->arr_largo_red[i]);
    printf("]\n");
    printf("Tamanio actual de la red:\t\t%d\n", largo_red);
    printf("Total de spines a simular:\t\t%d\n", largo_red*largo_red);
    printf("Constante de acoplamiento J:\t\t%.4f\n",parametros->J_intns_aco);
    printf("Intensidad del campo magnetico h:\t%.4f\n", parametros->h_cpo_mgntc_extr);
    printf("Temperatura inicial:\t\t\t%.4f\n", parametros->temp_ini);
    printf("Temperatura final:\t\t\t%.4f\n", parametros->temp_fin);
    printf("Numero de temperaturas a simular:\t%d\n",parametros->n_temp);
    printf("Tamanio del intervalo de temperatura:\t%.6g\n", (double)(parametros->temp_fin - parametros->temp_ini)/(double)(parametros->n_temp - 1));
    printf("Total barridos a realizar:\t\t%d\n\n", parametros->num_therm_mc + parametros->num_barr_med);
}

uint64_t calcula_id_tarea(int largo_red, int i_temp, int replica) {
    if (largo_red <= L_MAX && i_temp <= N_TEMP_MAX && replica <= R_MAX) {
        return ((uint64_t)largo_red * N_TEMP_MAX+ i_temp) * R_MAX + replica;
    } else {
        fprintf(stderr,"Error al calcular el id de la tarea\n");
        if (largo_red > L_MAX) fprintf(stderr, "El largo de la red es mayor al permitido\n");
        if (i_temp > N_TEMP_MAX)fprintf(stderr, "El i_temp de la red es mayor al permitido\n");
        if (replica > R_MAX) fprintf(stderr, "La replica es mayor a la permitida\n");
        exit(EXIT_FAILURE);
    }
}