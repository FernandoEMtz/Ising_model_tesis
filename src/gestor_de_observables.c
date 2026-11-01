//
// Created by Fernando on 28/10/2026.
//

#include "gestor_de_observables.h"
#include "gestor_de_red.h"

double funcion_magnetizacion_por_spin(const parametros_de_red_t *ejecucion) {

    /* Una modificacion para usar tambien la funcion total y dejar implicito el comportamiento
     * Podemos regresarla facilmente
     *     int i = 0;
     *     double mom_magn=0.0;
    for (i = 0; i < ejecucion->n_sp; i++) {
        mom_magn += ejecucion->spin[i];
    }
    return mom_magn/(double)ejecucion->n_sp;
    */
    return funcion_magnetizacion_total(ejecucion)/(double)ejecucion->n_sp;
}

//Esta funcion devuelve la suma total de los spines
int funcion_magnetizacion_total(const parametros_de_red_t *ejecucion) {
    int i = 0, magn = 0;
    for (i = 0; i < ejecucion->n_sp; i++) {
        magn += ejecucion->arr_spin[i];
    }
    return magn;
}

double funcion_hamiltoniano(const parametros_de_red_t *ejecucion) {
    int i = 0;
    double inter_vec = 0.0; //Interaccion entre vecinos

    //Suma de la interaccion del spin con las cargas de sus vecinos
    for (i = 0; i < ejecucion->n_sp; i++) inter_vec += ejecucion->arr_spin[i] * funcion_suma_de_vecinos(ejecucion, i);

    //Usamos 0.5 para no contar dos veces cada enlace
    //Recuerda comparar en las notas la diferencia de usar ahora el total y no por spin, ya que nos quitamos una multiplicacion
    //Recuerda comparar esto con las notas de V4 cuando escribas el funcionamiento, te tqm, ahi vamos
    //Recuerda eliminar estas anotaciones y solo dejar las estrictamente necesarias para el apartado en la tesis
    double hamiltoniano = -ejecucion->J_intns_aco * 0.5 * inter_vec - ejecucion->h_cpo_mgntc_extr * (double)funcion_magnetizacion_total(ejecucion);
    return hamiltoniano;
}

//Considerar reusar la funcion envoltorio para magnetizacion y energia (hamiltoniano) si llega a ser necesario