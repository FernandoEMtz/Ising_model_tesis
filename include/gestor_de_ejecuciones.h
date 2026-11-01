//
// Created by Fernando on 29/10/2026.
//

#ifndef TESIS_MODELO_ISING_V1_GESTOR_DE_EJECUCIONES_H
#define TESIS_MODELO_ISING_V1_GESTOR_DE_EJECUCIONES_H

#include "gestor_tabla_boltzmann.h"
#include "gestor_de_red.h"

typedef struct {
    //recordatorio tenemos E/N , |M|/N , M^2 , M^4
    //energia por spin = e -> energia_total/ n_sp
    //magnetizacion por spin -> m, con signo
    double sum_ener;        //suma energia <e>, se obtiene con muestras
    double sum_ener_cuad;   //suma de los cuadrados de la energia <e^2> para calor especifico C_v = n_sp * (<e^2> - <e>^2)/T^2
    double sum_magn_abs;    // suma de <|m|>
    double sum_magn_spn_2;  //<m^2> para la susceptibilidad magnetica X = n_sp*(<m^2> - <|m|>^2)/T, se obtiene con muestras
    double sum_magn_spn_4;  //<m^4> para el cumulante de Binder U = 1 - <m^4>/(3<m^2>^2) se obtiene con muestras
    long long muestras;
    long long aceptados; //guardar solo los barridos medidos
}result_montecarlo_t;

void funcion_recorrido_temperaturas(parametros_de_red_t *ejecucion, const tabla_boltzmann_t *tablas, result_montecarlo_t *resultado_mc);
void funcion_montecarlo(parametros_de_red_t *ejecucion, const tabla_boltzmann_t *tabla, result_montecarlo_t *resultado_mc);
void inicia_struct_resultados(result_montecarlo_t *resultado_mc);
void almacena_resultados_barrido(result_montecarlo_t *resultado_mc, const parametros_de_red_t *ejecucion, int aceptados);

#endif //TESIS_MODELO_ISING_V1_GESTOR_DE_EJECUCIONES_H
