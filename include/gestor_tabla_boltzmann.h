//
// Created by Fernando on 28/10/2026.
//

#ifndef TESIS_MODELO_ISING_V1_TABLA_DE_BOLTZMANN_H
#define TESIS_MODELO_ISING_V1_TABLA_DE_BOLTZMANN_H

#include "gestor_de_red.h"

typedef struct {
    double *valores;      //Valores que obtendra la tabla de Boltzmann
    int tam;            //Tamanio de la tabla de Boltzmann
}tabla_boltzmann_t;

void genera_tabla_boltzmann(caract_red_t *ejecucion, tabla_boltzmann_t *tabla_temp);
tabla_boltzmann_t calculo_tabla_boltzmann(double temp_star, caract_red_t *ejecucion);
double consulta_tabla_boltzmann(const tabla_boltzmann_t *tabla, int prod_spin_vec, int spin_actual);
void libera_memoria_struct_tabla_boltzmann(tabla_boltzmann_t *tabla);
void libera_memoria_temp_tabla_boltzmann(caract_red_t *ejecucion, tabla_boltzmann_t *tabla_temp);

//TEMPORAL
double calcula_temp_star(caract_red_t*ejecucion, int i_temp);

#endif //TESIS_MODELO_ISING_V1_TABLA_DE_BOLTZMANN_H
