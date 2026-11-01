//
// Created by Fernando on 30/10/2026.
//

#include <stdlib.h>
#include <stdio.h>

#include "gestor_de_pruebas.h"
#include "gestor_de_red.h"
#include "gestor_tabla_boltzmann.h"
#include "gestor_de_ejecuciones.h"
#include "gestor_de_observables.h"

#define semilla_global 20260830ULL
#define id_ejecucion 1ULL

int pruebas_pasadas = 0;
int pruebas_falladas = 0;

void inicio_red_ordenado(parametros_de_red_t *ejecucion) {
    int i = 0;
    for (i = 0; i < ejecucion->n_sp; i++) ejecucion->arr_spin[i] = 1;
}

prueba_t prepara_prueba(int L, double h, double T) {

    prueba_t prueba;

    prueba.red = genera_red(L);
    prueba.red.J_intns_aco = 1.0;
    prueba.red.h_cpo_mgntc_extr = h;
    prueba.red.temp_ini = T;
    prueba.red.temp_fin = T;
    prueba.red.n_temp = 1;

    inicia_randomizador_pcg64(&prueba.red.generador_rng, semilla_global, id_ejecucion);
    inicio_red_spines(&prueba.red);
    prueba.red.energia_total = funcion_hamiltoniano(&prueba.red);
    prueba.red.mgntc_total = funcion_magnetizacion_total(&prueba.red);

    prueba.tabla = calculo_tabla_boltzmann(T, &prueba.red);
    return prueba;
}

void libera_prueba(prueba_t *prueba) {
    libera_memoria_struct_tabla_boltzmann(&prueba.tabla);
    libera_memoria_red(&prueba.red);
}

void registra_prueba(const char *nombre, int resultado) {
    if (resultado == prueba_ok) {
        pruebas_pasadas++;
        printf("Ok %s\n", nombre);
    }else {
        pruebas_falladas++;
        printf("Fallo %s\n", nombre);
    }
}

int resumen_pruebas(void) {
    printf("\nPruebas %d Ok, %d fallo, %d en total\n", pruebas_pasadas, pruebas_falladas, pruebas_pasadas+pruebas_falladas);
    return pruebas_falladas;
}

static int prueba_num_5(void) {
    parametros_de_red_t red = genera_red(16);

    result_montecarlo_t *resultados = NULL;
    tabla_boltzmann_t *tabla_temp = NULL;

    inicia_randomizador_pcg64(&red.generador_rng, semilla_global, id_ejecucion);

    tabla_temp = malloc(red.n_temp * sizeof(tabla_boltzmann_t));
    resultados = malloc(red.n_temp *sizeof(result_montecarlo_t));

    if (tabla_temp == NULL || resultados == NULL) {
        printf("Error: fallo al asignar la memoria para las tablas o para los resultados\n");
        free(tabla_temp);
        free(resultados);
        libera_memoria_red(&red);
        return EXIT_FAILURE;
    }

    genera_tabla_boltzmann(&red, tabla_temp);
    funcion_recorrido_temperaturas(&red, tabla_temp,resultados);

    printf("Resultados\n");
    for (int i_temp = 0 ; i_temp < red.n_temp ; i_temp++) {
        double muestras = (double)resultados[i_temp].muestras;
        double ener_prom = resultados[i_temp].sum_ener/muestras;
        double magn_abs = resultados[i_temp].sum_magn_abs / muestras;
        double aceptacion = resultados[i_temp].aceptados / (muestras * (double)red.n_sp);

        printf("Temperatura indice [%d]:\n", i_temp);
        printf("E/N    = %f\n", ener_prom);
        printf("|M|/N  = %f\n", magn_abs);
        printf("Acept. = %f\n\n", aceptacion);
    }

    libera_memoria_temp_tabla_boltzmann(&red, tabla_temp);
    libera_memoria_red(&red);
    free(tabla_temp);
    free(resultados);
    return 1;
}