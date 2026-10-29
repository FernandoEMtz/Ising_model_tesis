//
// Created by Fernando on 28/10/2026.
//

#include "gestor_tabla_boltzmann.h"

#include <math.h>

#include "gestor_de_red.h"

#include <stdlib.h>

//Tenemos que llamar a esta función antes de la zona paralelizada, para que funcione para todas las ejecuciones
void genera_tabla_boltzmann(caract_red_t *ejecucion, tabla_boltzmann_t *tabla_temp) {
    int i_temp = 0;
    double temp_star = 0.0;

    for (i_temp = 0; i_temp < ejecucion->n_temp; i_temp++) {
        temp_star = calcula_temp_star(ejecucion, i_temp); //Recordemos que tenemos capado a 41 temperaturas, hay que hacer el cambio respectivo
        tabla_temp[i_temp] = calculo_tabla_boltzmann(temp_star, ejecucion);
    }
}

tabla_boltzmann_t calculo_tabla_boltzmann(double temp_star, caract_red_t *ejecucion) {
    tabla_boltzmann_t tabla;
    int i = 0, prod_spin_vec = 0;   //Suma del producto de los vecinos, hay que pensar en un nombre más intuitivo
    double q = 0.0;

    tabla.tam = ejecucion->n_vec+1;
    tabla.valores = malloc(tabla.tam * sizeof(*tabla.valores));

    if (tabla.valores == NULL) {
        //Agregar la leyenda de que no se pudo escribir las entradas
        exit(EXIT_FAILURE);
    }
    //Calculo de los valores de la tabla
    for (i = 0; i < tabla.tam; i++) {
        prod_spin_vec = - ejecucion->n_vec + 2 * i;
        q = ejecucion->J * (double)prod_spin_vec + ejecucion->h_campo; //Recordemos que delta E queda implicito
        tabla.valores[i] = exp(-2.0 * q / temp_star);
    }
    return tabla;
}

double consulta_tabla_boltzmann(const tabla_boltzmann_t *tabla, int prod_spin_vec, int spin_actual) {
    int i = (prod_spin_vec + tabla->tam - 1)/2;
    return (spin_actual == 1) ? tabla->valores[i] : 1.0/tabla->valores[i];
}

//Liberacion de la memoria del struct, no confundir con el arreglo
void libera_memoria_struct_tabla_boltzmann(tabla_boltzmann_t *tabla) {
    free(tabla->valores);
    tabla->valores = NULL;
    tabla->tam = 0;
}

void libera_memoria_temp_tabla_boltzmann(caract_red_t *ejecucion, tabla_boltzmann_t *tabla_temp) {
    int i_temp = 0;
    for (i_temp = 0; i_temp < ejecucion->n_temp; i_temp++) libera_memoria_struct_tabla_boltzmann(&tabla_temp[i_temp]);
}

//TEMPORAL, necesitamos ponerlo en su lugar, anteriormente, gestor de ejecuciones
double calcula_temp_star(caract_red_t*ejecucion, int i_temp) {
    return ejecucion->temp_i + i_temp * (ejecucion->temp_f - ejecucion->temp_i)/((double)ejecucion->n_temp - 1);
}