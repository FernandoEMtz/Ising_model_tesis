//
// Created by Fernando on 27/10/2026.
//

#ifndef TESIS_MODELO_ISING_V1_GESTOR_RED_H //Estas cosas se llaman guardas de inclusion
#define TESIS_MODELO_ISING_V1_GESTOR_RED_H

#include <stdint.h>

#include "pcg_variants.h"
#include "cJSON.h"

typedef struct {
    int largo_red;      //Reemplaza a n_x y n_y este caso es siempre para una red cuadrada
    int n_sp;           //Tota de spines, se obtiene por largo_red*largo_red
    int8_t *spin;            //Será el puntero del arreglo de spines de longitud dinamica
    pcg64_random_t generador_rng;
    //pcg64_random_t state_ini; PCG64 primera semilla, seguimos la convención de la librería
    //pcg64_random_t state_seq; PCG64 segunda semilla, seguimos la convención de la librería
    double J;
    double h_campo;
    int n_vec; //Recordemos que tenemos a los vecinos, tambien si es necesario mantener el dato, supongo que si
    int n_temp; //Mismo caso, se va para el cjson,
    double temp_i;
    double temp_f;
    double energia_total;
    int magn_total;
    int n_mc;
    int n_med;
}caract_red_t;

typedef struct {
    int largo_red;
    int n_sp;
    double generador_rng;
    double J;
    double h_campo;
    int n_vec;//Condicionado a si permitimos que lo ingresen o lo tenemos fijo en el programa
    double temp_i;
    double temp_f;
    int n_temp;
    int n_mc;
    int n_med;
}parametros_t;

caract_red_t genera_red(int largo_red);
void libera_memoria_red(caract_red_t *red);
void inicia_randomizador_pcg64(pcg64_random_t *random_local, uint64_t state_init, uint64_t state_seq);
double ejecuta_randomizador_pcg64(pcg64_random_t *random_local);
void inicio_red_spines(caract_red_t *ejecucion);
int funcion_suma_de_vecinos(const caract_red_t *ejecucion, int i);

void inicia_struct_parametros(parametros_t *parametros);
void mensaje_dato_nulo(cJSON *root, const char *nombre_dato);
void mensaje_dato_erroneo(cJSON *root, const char *nombre_dato, const char *tipo_dato);

#endif //TESIS_MODELO_ISING_V1_GESTOR_RED_H