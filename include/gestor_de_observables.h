//
// Created by Fernando on 28/10/2026.
//

#ifndef TESIS_MODELO_ISING_V1_GESTOR_OBSERVABLES_H
#define TESIS_MODELO_ISING_V1_GESTOR_OBSERVABLES_H

#include "gestor_de_red.h"

double funcion_magnetizacion_por_spin(const parametros_de_red_t *ejecucion);
int funcion_magnetizacion_total(const parametros_de_red_t *ejecucion);
double funcion_hamiltoniano(const parametros_de_red_t *ejecucion);

#endif //TESIS_MODELO_ISING_V1_GESTOR_OBSERVABLES_H
