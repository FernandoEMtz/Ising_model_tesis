//
// Created by Fernando on 27/10/2026.
//

#ifndef TESIS_MODELO_ISING_V1_GESTOR_RED_H //Estas cosas se llaman guardas de inclusion
#define TESIS_MODELO_ISING_V1_GESTOR_RED_H

#include <stdint.h>
#include "pcg_variants.h"

typedef struct {
    int largo_red;      //Reemplaza a n_x y n_y este caso es siempre para una red cuadrada
    int n_sp;           //Tota de spines, se obtiene por largo_red*largo_red
    int8_t *spin;            //Será el puntero del arreglo de spines de longitud dinamica
    pcg64_random_t generador_rng;
    double J;
    double h_campo;
    int n_vec; //Recordemos que tenemos a los vecinos, también si es necesario mantener el dato, supongo que si
    int n_temp; //Mismo caso, se va para el cjson,
    double temp_i;
    double temp_f;
    double energia_total;
    int magn_total;
    int n_mc;
    int n_med;
}caract_red_t;

caract_red_t genera_red(int largo_red);
void libera_memoria_red(caract_red_t *red);
void inicia_randomizador_pcg64(pcg64_random_t *random_local, uint64_t state_init, uint64_t state_seq);
double ejecuta_randomizador_pcg64(pcg64_random_t *random_local);
void inicio_red_spines(caract_red_t *ejecucion);
int funcion_suma_de_vecinos(const caract_red_t *ejecucion, int i);

#endif //TESIS_MODELO_ISING_V1_GESTOR_RED_H