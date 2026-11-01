//
// Created by Fernando on 31/10/2026.
//

#ifndef TESIS_MODELO_ISING_V1_GESTOR_DE_DATOS_DE_ENTRADA_H
#define TESIS_MODELO_ISING_V1_GESTOR_DE_DATOS_DE_ENTRADA_H

#include <stdint.h>

typedef struct {

    uint64_t semilla_global;
    int *arr_largo_red;
    double J_intns_aco;
    double h_cpo_mgntc_extr;
    double temp_ini;
    double temp_fin;
    double n_temp;
    int num_therm_mc;
    int num_barr_med;
}parametros_t;

parametros_t gestor_de_configuracion(const char *json_string);


#endif //TESIS_MODELO_ISING_V1_GESTOR_DE_DATOS_DE_ENTRADA_H
