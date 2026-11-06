#include <stdio.h>
#include <stdlib.h>

#include "gestor_de_ejecuciones.h"
#include "gestor_de_red.h"

#include "gestor_de_pruebas.h"

#define N_REPLICAS 1

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

    //Número total de casillas del arreglo de resultados
    //Largos de red * Temperatura * Número total de réplicas a simular
    int total_casillas = parametros.cont_tam_red * parametros.n_temp * N_REPLICAS;
    //Asignacion del número total de casillas
    resultados = malloc(total_casillas * sizeof(result_montecarlo_t));
    if (resultados == NULL) {
        printf("Error: no se pudo asignar memoria para 'resultados'\n");
        libera_memoria_temp_tabla_boltzmann(&parametros, tablas);
        free(tablas);
        libera_memoria_arr_largo_red(&parametros);
        return 1;
    }
    printf("Resultados\n");
    printf("L\t\tT\t\tE/N\t\t|M|/N\t\tAceptacion\n");

    //Recorrido para los tamaños de la red
    for (int i_largo = 0; i_largo < parametros.cont_tam_red; i_largo++) {
        //Obtenemos el largo actual de la red en turno
        int largo_act = parametros.arr_largo_red[i_largo];
        //reporte_de_simulacion(&parametros, largo_act);
        //Recorrido sobre las temperaturas de cada red
        for (int i_temp = 0; i_temp < parametros.n_temp; i_temp++ ) {
            //Recorrido para las réplicas configuradas
            for (int i_rep = 0; i_rep < N_REPLICAS; i_rep++) {
                //Obtencion del valor de la casilla de montecarlo exacta de almacenamiento
                int i_mc = (i_largo * parametros.n_temp + i_temp ) * N_REPLICAS + i_rep;
                funcion_recorrido_simulacion(&parametros,  &tablas[i_temp], &resultados[i_mc],largo_act, i_temp, i_rep);
                double muestras = (double)resultados[i_mc].muestras;
                double ener_prom = resultados[i_mc].sum_ener/muestras;
                double magn_abs = resultados[i_mc].sum_magn_abs / muestras;
                double aceptacion = resultados[i_mc].aceptados / (muestras * (double)largo_act*largo_act);
                double temp_actual = calcula_temp_star(&parametros, i_temp);
                if (i_temp == 3 || i_temp == 5) {
                    printf("%d\tIndice %d (%.4f)\t%f\t%f\t%f\n", largo_act, i_temp, temp_actual, ener_prom, magn_abs, aceptacion);
                }
            }
        }
    }

    libera_memoria_temp_tabla_boltzmann(&parametros, tablas);
    free(tablas);
    tablas = NULL;
    libera_memoria_arr_largo_red(&parametros);
    free(resultados);
    resultados = NULL;

    return 0;
}