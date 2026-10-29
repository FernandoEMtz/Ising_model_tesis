//
// Created by Fernando on 28/10/2026.
//

#include "gestor_observables.h"
#include "gestor_red.h"

double funcion_magnetizacion(caract_red_t *ejecucion) {
    int i = 0;
    double mom_magn = 0;
    for (i = 0; i < ejecucion->n_sp; i++) {
        mom_magn += ejecucion->spin[i];
    }
    return mom_magn/(double)ejecucion->n_sp;
}

double funcion_hamiltoniano(caract_red_t *ejecucion) {
    int i = 0;
    double inter_vec = 0; //Interaccion entre vecinos

    //Suma de la interaccion del spin con las cargas de sus vecinos
    for (i = 0; i < ejecucion->n_sp; i++) inter_vec += ejecucion->spin[i] * suma_de_vecinos(ejecucion, i);

    //Usamos 0.5 para no contar dos veces cada enlace
    double hamiltoniano = -ejecucion->J * 0.5 * inter_vec - ejecucion->h_campo * (double)ejecucion->n_sp * funcion_magnetizacion(ejecucion);
    return hamiltoniano;
}