//
// Created by Fernando on 29/10/2026.
//

#include "gestor_de_ejecuciones.h"

#include <math.h>
#include <string.h>

#include "gestor_de_observables.h"
#include "gestor_de_red.h"
#include "gestor_metropolis.h"

//Vamos a meter aqui la gestion de temperatura, montecarlo y lo que nos falte de las ejecuciones
//El gestor metropolis queda entonces separado para que trabaje por su cuenta
//Si en algun momento queremos evaluar por otro medio como Wolff se puede hacer el cambio sin muchas modificaciones

void funcion_recorrido_temperaturas(uint64_t semilla_global, parametros_de_red_t *ejecucion, const tabla_boltzmann_t *tablas, result_montecarlo_t *resultado_mc) {
    int i_temp = 0;
    //Recorrido de temperaturas
    for (i_temp = 0; i_temp < ejecucion->n_temp; i_temp++) {
        //Cargamos la secuencia, de momento replica vale 0
        uint64_t secuencia = calcula_id_tarea(ejecucion->largo_red, i_temp, 0);
        //Sembrado de semilla con el RNG especifico de la temperatura
        inicia_randomizador_pcg64(&ejecucion->generador_rng, semilla_global, secuencia);
        //Inicio de red para la temperatura a evaluar
        inicio_red_spines(ejecucion);
        //Inicio de valores para los resultados, se reinicia para cada temperatura en valor cero
        memset(&resultado_mc[i_temp], 0, sizeof(result_montecarlo_t));
        //Energia total inicial del sistema
        ejecucion->energia_total = funcion_hamiltoniano(ejecucion);
        //Magnetizacion inicial del sistema
        ejecucion->mgntc_total = funcion_magnetizacion_total(ejecucion);
        //Inicio de metodo de Montecarlo, con parametros de energia y magnetizacion cargados
        funcion_montecarlo(ejecucion, &tablas[i_temp], &resultado_mc[i_temp]);
    }
}

void funcion_montecarlo(parametros_de_red_t *ejecucion, const tabla_boltzmann_t *tabla, result_montecarlo_t *resultado_mc) {
    int i_therm = 0, i_barr = 0, aceptados_barrido = 0;
    //Termalizacion, descartamos los pasos para la estadistica, pero buscamos que el sistema llegue a un punto de minima energia
    for(i_therm = 0; i_therm < ejecucion->num_therm_mc; i_therm++) funcion_metropolis_barrido(ejecucion, tabla);

    //Barrido, los pasos que tenemos aqui nos permiten hacer el barrido real de la simulacion
    for (i_barr = 0; i_barr < ejecucion->num_barr_med; i_barr++) {
        aceptados_barrido = funcion_metropolis_barrido(ejecucion, tabla);
        almacena_resultados_barrido(resultado_mc, ejecucion, aceptados_barrido);
    }
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