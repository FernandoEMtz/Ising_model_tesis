#include <stdio.h>
#include <stdlib.h>

#include "gestor_de_red.h"
#include "gestor_de_observables.h"

#include "gestor_de_pruebas.h"

static int prueba_arranque_ordenado(void) {
    prueba_t p = prepara_prueba(8, 0.0, 2.0);
    int ok = 0;

    inicio_red_ordenado(&p.red);
    p.red.energia_total = funcion_hamiltoniano(&p.red);
    p.red.mgntc_total = funcion_magnetizacion_total(&p.red);

    ok = (p.red.energia_total == -2.0 * p.red.n_sp) && (p.red.mgntc_total == p.red.n_sp);
    libera_prueba(&p);
    return ok ? prueba_ok:prueba_fallo;
}


int main(void) {
    registra_prueba("arranque ordenado: E = -2N, M = N", prueba_arranque_ordenado());
    return resumen_pruebas() != 0 ? 1:0;
}