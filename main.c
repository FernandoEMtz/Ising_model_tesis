#include <stdio.h>
#include <stdlib.h>

#include "gestor_de_ejecuciones.h"
#include "gestor_de_red.h"
#include "gestor_de_observables.h"
#include "gestor_tabla_boltzmann.h"
#include "gestor_de_ejecuciones.h"
//Los .c van en /src y los .h en /include

//static int validar_configuraciones_red(void);

//#define OK    1
//#define FALLO 0

int main(void) {

    caract_red_t red = genera_red(16);

    result_montecarlo_t *resultados = NULL;
    tabla_boltzmann_t *tabla_temp = NULL;

    uint64_t semilla_global = 20260830;
    int id_ejecucion = 1;
    inicia_randomizador_pcg64(&red.generador_rng, semilla_global, id_ejecucion);

    tabla_temp = malloc(red.n_temp * sizeof(tabla_boltzmann_t));
    resultados = malloc(red.n_temp *sizeof(result_montecarlo_t));

    if (tabla_temp == NULL || resultados == NULL) {
        printf("Error: fallo al asignar la memoria para las tablas o para los resultados\n");
        free(tabla_temp);
        free(resultados);
        libera_memoria_red(&red);
        return EXIT_FAILURE;
    }

    genera_tabla_boltzmann(&red, tabla_temp);
    funcion_recorrido_temperaturas(&red, tabla_temp,resultados);

    printf("Resultados\n");
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

    libera_memoria_temp_tabla_boltzmann(&red, tabla_temp);
    libera_memoria_red(&red);
    free(tabla_temp);
    free(resultados);

    return 0;
}