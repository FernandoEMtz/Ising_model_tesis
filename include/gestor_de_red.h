//
// Created by Fernando on 27/10/2026.
//

#ifndef TESIS_MODELO_ISING_V1_GESTOR_RED_H
#define TESIS_MODELO_ISING_V1_GESTOR_RED_H

#include <stdint.h>
#include "pcg_variants.h"
#include "gestor_de_datos_de_entrada.h"


typedef struct {
    double J_intns_aco;         //J intensidad de acoplamiento, de preferencia en 1.0
    double h_cpo_mgntc_extr;    //h campo magnetico externo, de preferencia en 0.0
    double temp_ini;            //Temperatura inicial, no puede ser 0.0 debido a las tablas de Boltzmann
    double temp_fin;            //Temperatura final, no puede ser igual o menor a temperatura inicial
    double energia_total;       //Instanciado en cero al momento de generar la red
    int mgntc_total;            //Instanciado en cero al momento de generar la red
    int n_temp;                 //Número de temperaturas a simular
    int num_therm_mc;           //Número de pasos para termalización (Montecarlo)
    int num_barr_med;           //Numero de pasos para barrido medio
    int n_sp;                   //Número total de spines, se obtiene por largo_red*largo_red
    int8_t *arr_spin;           //Puntero del arreglo de spines de longitud dinamica
    int largo_red;              //Largo de la red para la ejecucion en turno
    pcg64_random_t generador_rng;//TEMPORAL, acomodar en donde va
}parametros_de_red_t;

//Constante para pgc64
#define PGC_INIT_SEQ 22011997

parametros_de_red_t genera_red(const parametros_t *parametros, int largo_red);
void libera_memoria_red(parametros_de_red_t *red);
void inicio_red_spines(parametros_de_red_t *ejecucion);
void inicia_randomizador_pcg64(pcg64_random_t *random_local, pcg128_t semilla_global, pcg128_t id_tarea);
double ejecuta_randomizador_pcg64(pcg64_random_t *random_local);
int funcion_suma_de_vecinos(const parametros_de_red_t *ejecucion, int i);
void reporte_de_simulacion(const parametros_t *parametros, int largo_red);
uint64_t calcula_id_tarea(int largo_red, int i_temp, int i_rep);
#endif //TESIS_MODELO_ISING_V1_GESTOR_RED_H