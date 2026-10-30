//
// Created by Fernando on 29/10/2026.
//

#include "gestor_de_ejecuciones.h"

#include "gestor_de_observables.h"
#include "gestor_de_red.h"
#include "gestor_metropolis.h"


//Vamos a meter aqui la gestion de temperatura, montecarlo y lo que nos falte de las ejecuciones
//El gestor metropolis queda entonces separado para que trabaje por su cuenta
//Si en algun momento queremos evaluar por otro medio como Wolff se puede hacer el cambio sin muchas modificaciones

void recorrido_temperaturas(caract_red_t *ejecucion, const tabla_boltzmann_t *tablas, result_montecarlo_t *resultado_mc) {
    int i_temp = 0;

    for (i_temp = 0; i_temp < ejecucion->n_temp; i_temp++) {

        inicio_red_spines(ejecucion);
        inicia_struct_resultados(resultado_mc);
        ejecucion->energia_total = funcion_hamiltoniano(ejecucion);
        ejecucion->magn_total = funcion_magnetizacion_total(ejecucion);

        funcion_montecarlo(ejecucion, &tablas[i_temp]);
    }
}

void funcion_montecarlo(caract_red_t *ejecucion, const tabla_boltzmann_t *tabla) {
    int i_term; //Termalizacion

    for(i_term = 0; i_term < ejecucion->n_mc; i_term++) {
        funcion_metropolis_barrido(ejecucion, tabla);
    }
}

void inicia_struct_resultados(result_montecarlo_t *resultado_mc) {
    resultado_mc->sum_ener = 0.0;
    resultado_mc->sum_ener_cuad = 0.0;
    resultado_mc->sum_magn_abs = 0.0;
    resultado_mc->sum_magn_spn_2 = 0.0;
    resultado_mc->sum_magn_spn_4 = 0.0;
    resultado_mc->muestras = 0;
    resultado_mc->aceptados = 0;
}