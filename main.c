#include <stdio.h>
#include <stdlib.h>

#include "gestor_de_ejecuciones.h"
#include "gestor_de_red.h"

#include "gestor_de_pruebas.h"



int main(int argc, char *argv[]) {

    //Validacion de que el primer argumento es parametros_simulacion.json
    if (argc < 2) {
        fprintf(stderr,"Uso %s <parametros_simulacion.json>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    //Validacion de datos, mensajes de error, lectura de archivo
    char *archivo_entrada = lectura_archivo_cjson(argv[1]);
    parametros_t parametros = gestor_de_parametros(archivo_entrada);
    //Liberacion memoria del cJSON
    free(archivo_entrada);
    //Inicio de tablas de Boltzmann como estructura vacia
    tabla_boltzmann_t *tablas = NULL;
    //Inicio de resultados de la simulacion como estructura vacia
    result_montecarlo_t *resultados = NULL;
    //Asignacion de tamaño del arreglo para la tabla de boltzmann
    tablas = malloc(parametros.n_temp * sizeof(tabla_boltzmann_t));
    if (tablas == NULL) {
        printf("Error: no se pudo asignar memoria para 'tablas'\n");
        libera_memoria_arr_largo_red(&parametros);
        return 1;
    }
    //Generacion de la tabla de Boltzmann unica para toda la ejecucion
    genera_tabla_boltzmann(&parametros, tablas);
    resultados = malloc(parametros.n_temp * sizeof(result_montecarlo_t));
    if (resultados == NULL) {
        printf("Error: no se pudo asignar memoria para 'resultados'\n");
        libera_memoria_temp_tabla_boltzmann(&parametros, tablas);
        free(tablas);
        libera_memoria_arr_largo_red(&parametros);
        return 1;
    }
    //Recorrido de la simulacion con base al largo actual de la red asignado por el arreglo de largos de red
    printf("Resultados\n");
    printf("L\t\tT\t\tE/N\t\t|M|/N\t\tAceptacion\n");
    for (int i = 0; i < parametros.cont_tam_red; i++) {
        //Generacion de la red actual
        parametros_de_red_t red_i = genera_red(&parametros, parametros.arr_largo_red[i]);
        //Reporte de parametros de simulacion cargados para la ejecucion en curso
        //reporte_de_simulacion(&parametros, parametros.arr_largo_red[i]);
        //Recorrido de temperaturas para el tamaño de red actual, carga los resultados al struct resultados
        funcion_recorrido_temperaturas(parametros.semilla_global, &red_i, tablas, resultados);
        //printf("Resultados: %d\n", red_i.largo_red);
        for (int i_temp = 0; i_temp <red_i.n_temp; i_temp++ ) {
            double muestras = (double)resultados[i_temp].muestras;
            double ener_prom = resultados[i_temp].sum_ener/muestras;
            double magn_abs = resultados[i_temp].sum_magn_abs / muestras;
            double aceptacion = resultados[i_temp].aceptados / (muestras * (double)red_i.n_sp);
            double temp_actual = calcula_temp_star(&parametros, i_temp);
            if (i_temp == 3 || i_temp == 5) {
                printf("%d\tIndice %d (%.4f)\t%f\t%f\t%f\n", red_i.largo_red, i_temp, temp_actual, ener_prom, magn_abs, aceptacion);
            }
        }
        /*
        for (int i_temp = 0 ; i_temp < red_i.n_temp ; i_temp++) {
            double muestras = (double)resultados[i_temp].muestras;
            double ener_prom = resultados[i_temp].sum_ener/muestras;
            double magn_abs = resultados[i_temp].sum_magn_abs / muestras;
            double aceptacion = resultados[i_temp].aceptados / (muestras * (double)red_i.n_sp);
            double temp_actual = calcula_temp_star(&parametros, i_temp);

            printf("Temperatura indice [%d] -> T: [%.4f] \n", i_temp, temp_actual);
            printf("E/N    = %f\n", ener_prom);
            printf("|M|/N  = %f\n", magn_abs);
            printf("Acept. = %f\n\n", aceptacion);
        }*/
        libera_memoria_red(&red_i);
    }
    libera_memoria_temp_tabla_boltzmann(&parametros, tablas);
    free(tablas);
    tablas = NULL;
    libera_memoria_arr_largo_red(&parametros);
    free(resultados);
    resultados = NULL;

    return 0;
}