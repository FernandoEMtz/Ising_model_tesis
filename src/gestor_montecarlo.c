//
// Created by Fernando on 29/10/2026.
//

#include "gestor_montecarlo.h"
#include "gestor_de_red.h"
#include "gestor_tabla_boltzmann.h"

//Intento de metropolis en el sitio, devuelve 1 si fue aceptado el cambio o 0 si no
int funcion_metropolis_intento(caract_red_t * ejecucion, const tabla_boltzmann_t *tabla, int i) {
    int selec_spin = 0, suma_de_vecinos = 0;
    double delta_ener = 0.0, factor_boltzmann = 0.0;
    //i determinado por pcg64, en este caso, lo determinamos para hacer pruebas
    selec_spin = ejecucion->spin[i];
    suma_de_vecinos = funcion_suma_de_vecinos(ejecucion, i);

    factor_boltzmann = consulta_tabla_boltzmann(tabla, suma_de_vecinos, selec_spin);
    //Aceptacion o rechazo del movimiento
    //¿Que diferencia tiene con (randomizador_paralelo(&ejecucion->generador_rng) < factor_boltzmann) de V4?
    //¿Porqué factor >= 1 es equivalente a Delta E <=0?
    //factor >= 1 equivale a ΔE <= 0: factor = exp(−ΔE/T), y exp(x) >= 1 si y solo si x >= 0.
    //Con T > 0, eso es −ΔE/T >= 0, es decir, ΔE <= 0. Para s = −1 se cumple igual,
    //porque el recíproco que aplica la tabla es exp(+2q/T), que es justo exp(−ΔE/T) para ese caso.
    if (factor_boltzmann >= 1 || ejecuta_randomizador_pcg64(&ejecucion->generador_rng) < factor_boltzmann) {
        //Cambio en el spin
        delta_ener = 2.0 * selec_spin * (ejecucion->J * suma_de_vecinos + ejecucion->h_campo);
        ejecucion->spin[i] = -selec_spin;
        ejecucion->energia_total += delta_ener;
        ejecucion->magn_total += -2 * selec_spin;
        return 1;
    }else {
        return 0;
    }
}

int funcion_metropolis_barrido(caract_red_t * ejecucion, const tabla_boltzmann_t *tabla) {
    int i_accpt = 0, i_nsp = 0, rand_spin = 0;
    //Ciclo de spines
    for (i_nsp = 0; i_nsp < ejecucion->n_sp; i_nsp++) {
        //Obtencion de un spin aleatorio entre 0 y nsp
        rand_spin = (int)pcg64_boundedrand_r(&ejecucion->generador_rng, ejecucion->n_sp);
        //Acumulador de la cantidad de aceptados por cada intento de metropolis
        i_accpt += funcion_metropolis_intento(ejecucion, tabla, rand_spin);
    }
    return i_accpt;
}