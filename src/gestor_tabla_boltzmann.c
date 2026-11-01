//
// Created by Fernando on 28/10/2026.
//

#include "gestor_tabla_boltzmann.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "gestor_de_datos_de_entrada.h"

//Tenemos que llamar a esta función antes de la zona paralelizada, para que funcione para todas las ejecuciones
void genera_tabla_boltzmann(const parametros_t *parametros, tabla_boltzmann_t *tabla_temp) {
    int i_temp = 0;
    double temp_star = 0.0;

    for (i_temp = 0; i_temp < parametros->n_temp; i_temp++) {
        temp_star = calcula_temp_star(parametros, i_temp); //Recordemos que tenemos capado a 41 temperaturas, hay que hacer el cambio respectivo
        tabla_temp[i_temp] = calculo_tabla_boltzmann(parametros, temp_star);
    }
}

tabla_boltzmann_t calculo_tabla_boltzmann(const parametros_t *parametros, double temp_star) {
    tabla_boltzmann_t tabla;
    int i = 0, sum_prod_spin_vec = 0;
    double q = 0.0;

    tabla.tam = n_vec+1;
    tabla.valores = malloc(tabla.tam * sizeof(*tabla.valores));

    if (tabla.valores == NULL) {
        //Agregar la leyenda de que no se pudo escribir las entradas
        printf("Error al guardar memoria para %d entradas \n", tabla.tam);
        exit(EXIT_FAILURE);
    }
    //Calculo de los valores de la tabla
    for (i = 0; i < tabla.tam; i++) {
        sum_prod_spin_vec = - n_vec + 2 * i;
        q = parametros->J_intns_aco * (double)sum_prod_spin_vec + parametros->h_cpo_mgntc_extr; //Recordemos que delta E queda implicito
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

void libera_memoria_temp_tabla_boltzmann(const parametros_t *parametros, tabla_boltzmann_t *tabla_temp) {
    int i_temp = 0;
    for (i_temp = 0; i_temp < parametros->n_temp; i_temp++) libera_memoria_struct_tabla_boltzmann(&tabla_temp[i_temp]);
}

double calcula_temp_star(const parametros_t *parametros, int i_temp) {
    return parametros->temp_ini + i_temp * (parametros->temp_fin - parametros->temp_ini)/(double)(parametros->n_temp - 1);
}

