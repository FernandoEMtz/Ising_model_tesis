//
// Created by Fernando on 30/10/2026.
//

#ifndef TESIS_MODELO_ISING_V1_GESTOR_DE_PRUEBAS_H
#define TESIS_MODELO_ISING_V1_GESTOR_DE_PRUEBAS_H

#include "gestor_de_red.h"
#include "gestor_tabla_boltzmann.h"

#define prueba_ok 1
#define prueba_fallo 0

typedef struct {
    caract_red_t red;
    tabla_boltzmann_t tabla;
}prueba_t;

prueba_t prepara_prueba(int L, double h, double T);
void inicio_red_ordenado(caract_red_t *ejecucion);
void libera_prueba(prueba_t *prueba);
void registra_prueba(const char *nombre, int resultado);
int resumen_pruebas(void);
static int prueba_num_5(void);

extern int pruebas_pasadas;
extern int pruebas_falladas;

#endif //TESIS_MODELO_ISING_V1_GESTOR_DE_PRUEBAS_H
