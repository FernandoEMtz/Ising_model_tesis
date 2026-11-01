//
// Created by Fernando on 29/10/2026.
//

#include "gestor_de_ejecuciones.h"

#include <math.h>

#include "gestor_de_observables.h"
#include "gestor_de_red.h"
#include "gestor_metropolis.h"

//Vamos a meter aqui la gestion de temperatura, montecarlo y lo que nos falte de las ejecuciones
//El gestor metropolis queda entonces separado para que trabaje por su cuenta
//Si en algun momento queremos evaluar por otro medio como Wolff se puede hacer el cambio sin muchas modificaciones

void funcion_recorrido_temperaturas(parametros_de_red_t *ejecucion, const tabla_boltzmann_t *tablas, result_montecarlo_t *resultado_mc) {
    int i_temp = 0;

    for (i_temp = 0; i_temp < ejecucion->n_temp; i_temp++) {

        inicio_red_spines(ejecucion);
        inicia_struct_resultados(&resultado_mc[i_temp]);

        ejecucion->energia_total = funcion_hamiltoniano(ejecucion);
        ejecucion->mgntc_total = funcion_magnetizacion_total(ejecucion);

        funcion_montecarlo(ejecucion, &tablas[i_temp], &resultado_mc[i_temp]);
    }
}

void funcion_montecarlo(parametros_de_red_t *ejecucion, const tabla_boltzmann_t *tabla, result_montecarlo_t *resultado_mc) {
    int i_termalizacion = 0, i_medicion = 0; //Termalizacion
    int aceptados_barrido = 0;

    //Termalizacion
    for(i_termalizacion = 0; i_termalizacion < ejecucion->num_therm_mc; i_termalizacion++) funcion_metropolis_barrido(ejecucion, tabla);

    //Medicion
    for (i_medicion = 0; i_medicion < ejecucion->num_barr_med; i_medicion++) {
        aceptados_barrido = funcion_metropolis_barrido(ejecucion, tabla);
        almacena_resultados_barrido(resultado_mc, ejecucion, aceptados_barrido);
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

void almacena_resultados_barrido(result_montecarlo_t *resultado_mc, const parametros_de_red_t *ejecucion, int aceptados_barrido) {
    double ener = 0.0, magn = 0.0, magn2 = 0.0;
    ener = (double)ejecucion->energia_total/ (double)ejecucion->n_sp;
    magn = fabs((double)ejecucion->mgntc_total / (double)ejecucion->n_sp);
    magn2 = magn * magn;
    resultado_mc->sum_ener += ener;
    resultado_mc->sum_ener_cuad += ener*ener;
    resultado_mc->sum_magn_abs += magn;
    resultado_mc->sum_magn_spn_2 += magn2;
    resultado_mc->sum_magn_spn_4 += magn2*magn2;
    resultado_mc->muestras += 1;
    resultado_mc->aceptados += aceptados_barrido;
}