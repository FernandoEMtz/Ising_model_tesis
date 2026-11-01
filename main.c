#include <stdio.h>
#include <stdlib.h>

#include "gestor_de_ejecuciones.h"
#include "gestor_de_red.h"

#include "gestor_de_pruebas.h"

static const char *json = R"({
    "semilla_global": 20260830,
    "arr_largo_red": [16,32],
    "J_intns_aco": 1,
    "h_cpo_mgntc_extr": 0,
    "temp_ini": 2.0,
    "temp_fin": 2.5,
    "n_temp": 41,
    "num_therm_mc": 10,
    "num_barr_med": 5000
})";

int main(void) {
    //Inicio de estructuras
    parametros_t parametros = gestor_de_configuracion(json);
    tabla_boltzmann_t *tablas = NULL;
    result_montecarlo_t *resultados = NULL;

    tablas = malloc(parametros.n_temp * sizeof(tabla_boltzmann_t));
    if (tablas == NULL) {
        printf("Error: no se pudo asignar memoria para 'tablas'\n");
        libera_memoria_arr_largo_red(&parametros);
        return 1;
    }
    genera_tabla_boltzmann(&parametros, tablas);
    resultados = malloc(parametros.n_temp * sizeof(result_montecarlo_t));
    if (resultados == NULL) {
        printf("Error: no se pudo asignar memoria para 'resultados'\n");
        libera_memoria_temp_tabla_boltzmann(&parametros, tablas);
        free(tablas);
        libera_memoria_arr_largo_red(&parametros);
        return 1;
    }

    for (int i = 0; i < parametros.cont_tam_red; i++) {
        parametros_de_red_t red = genera_red(&parametros, parametros.arr_largo_red[i]);
        inicia_randomizador_pcg64(&red.generador_rng, parametros.semilla_global, i +1); //i +1 reemplazar por id
        funcion_recorrido_temperaturas(&red, tablas, resultados);

        printf("Resultados: %d\n", red.largo_red);
        for (int i_temp = 0 ; i_temp < red.n_temp ; i_temp++) {
            double muestras = (double)resultados[i_temp].muestras;
            double ener_prom = resultados[i_temp].sum_ener/muestras;
            double magn_abs = resultados[i_temp].sum_magn_abs / muestras;
            double aceptacion = resultados[i_temp].aceptados / (muestras * (double)red.n_sp);

            printf("Temperatura indice [%d]:\n", i_temp);
            printf("E/N    = %f\n", ener_prom);
            printf("|M|/N  = %f\n", magn_abs);
            printf("Acept. = %f\n\n", aceptacion);
        }
        libera_memoria_red(&red);
    }
    libera_memoria_temp_tabla_boltzmann(&parametros, tablas);
    free(tablas);
    tablas = NULL;
    libera_memoria_arr_largo_red(&parametros);
    free(resultados);
    resultados = NULL;

    return 0;
}