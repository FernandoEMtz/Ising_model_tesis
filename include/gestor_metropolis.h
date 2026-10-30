//
// Created by Fernando on 29/10/2026.
//

#ifndef TESIS_MODELO_ISING_V1_MONTECARLO_H
#define TESIS_MODELO_ISING_V1_MONTECARLO_H

#include "gestor_tabla_boltzmann.h"
#include "gestor_de_red.h"

int funcion_metropolis_intento(caract_red_t * ejecucion, const tabla_boltzmann_t *tabla, int i);
int funcion_metropolis_barrido(caract_red_t * ejecucion, const tabla_boltzmann_t *tabla);

#endif //TESIS_MODELO_ISING_V1_MONTECARLO_H
