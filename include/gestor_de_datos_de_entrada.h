//
// Created by Fernando on 31/10/2026.
//

#ifndef TESIS_MODELO_ISING_V1_GESTOR_DE_DATOS_DE_ENTRADA_H
#define TESIS_MODELO_ISING_V1_GESTOR_DE_DATOS_DE_ENTRADA_H

#include <stdint.h>

typedef struct {

    uint64_t semilla_global;
    int *arr_largo_red;
    int cont_tam_red;
    double J_intns_aco;
    double h_cpo_mgntc_extr;
    double temp_ini;
    double temp_fin;
    int n_temp;
    int num_therm_mc;
    int num_barr_med;
    int n_vec;
}parametros_t;

char *lectura_archivo_cjson(const char *nombre_archivo);
parametros_t gestor_de_configuracion(const char *json_string);
void libera_memoria_arr_largo_red(parametros_t *parametros);

#endif //TESIS_MODELO_ISING_V1_GESTOR_DE_DATOS_DE_ENTRADA_H
