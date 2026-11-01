//
// Created by Fernando on 31/10/2026.
//

#include "gestor_de_datos_de_entrada.h"

#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>

#include "cJSON.h"

static long long validar_parametro_int(cJSON *root, const char *nombre_dato, const char *tipo_dato, double lim_inf, double lim_sup);
static double validar_parametro_double(cJSON *root, const char *nombre_dato, const char *tipo_dato, double lim_inf, double lim_sup);
static void lectura_asignacion_arr_largo_red(cJSON *root, parametros_t *parametros);
static _Noreturn void mensaje_dato_nulo(cJSON *root, const char *nombre_dato);
static _Noreturn void mensaje_dato_erroneo(cJSON *root, const char *nombre_dato, const char *tipo_dato);
static _Noreturn void mensaje_dato_fuera_de_rango(cJSON *root, const char *nombre_dato, double lim_inf, double lim_sup);

parametros_t gestor_de_configuracion(const char *json_string) {
    //Inicio del struct parametro en ceros
    parametros_t parametros = {0};
    //Parseo (to parse) de datos de texto a cjson
    cJSON *root = cJSON_Parse(json_string);
    //Archivo vacio o llenado incorrectamente
    if (root == NULL) {
        const char *mensaje_error_ptr = cJSON_GetErrorPtr();
        if (mensaje_error_ptr != NULL) {
            long posicion = mensaje_error_ptr - json_string;
            int caract_atras = 15;
            const char *contexto_ptr = mensaje_error_ptr - caract_atras;
            //Puede salirse del arreglo para el contexto, si pasa, completar condicion para evitar desbordamiento
            if (contexto_ptr < json_string) {
                contexto_ptr = json_string;
            }
            int long_contexto = mensaje_error_ptr - contexto_ptr;
            printf("\n\tError de validacion en el archivo cJSON (caracter %ld)\n", posicion + 1);
            printf("\tTexto previo al error: \"%.*s\" <-[CONTEXTO]\n", long_contexto, contexto_ptr);
        }else {
            printf("\n\tError de validacion en el archivo cJSON: [ERROR DESCONOCIDO AL PARSEAR]\n");
        }
        exit(EXIT_FAILURE);
    }
    //Lectura y asignacion de la semilla global
    parametros.semilla_global = (uint64_t)validar_parametro_int(root, "semilla_global",
        "entero no negativo", 0.0, (double)((1ULL <<53)-1));
    //Lectura y asignacion de los largos de red
    lectura_asignacion_arr_largo_red(root, &parametros);
    //Lectura y asignacion de J (intensidad de acoplamiento)
    parametros.J_intns_aco = validar_parametro_double(root, "J_intns_aco",
        "Numero real, se sugiere 1.0 para un material ferromagnetico, caso J = 0 trivialidad de experimento",-1000,1000);
    //Lectura y asignacion de h (Campo magnetico externo)
    parametros.h_cpo_mgntc_extr = validar_parametro_double(root, "h_cpo_mgntc_extr",
        "numero real, se sugiere 0 para no considerar la aportacion del campo",-1000, 1000);
    //Lectura y asignacion de la temperatura inicial
    parametros.temp_ini = validar_parametro_double(root, "temp_ini",
        "numero real positivo", 0.0, 100.0);
    //Lectura y asignacion de la temperatura final
    parametros.temp_fin = validar_parametro_double(root, "temp_fin",
        "numero real positivo mayor al valor de la temperatura inicial", parametros.temp_ini,100.0);
    //Lectura y asignacion del número de intervalos de temperatura
    parametros.n_temp = (int)validar_parametro_int(root, "n_temp",
        "al menos dos puntos de temperatura", 2, 10000);
    //Lectura y asignacion del número de pasos de termalización (Montecarlo)
    parametros.num_therm_mc= (int)validar_parametro_int(root, "num_therm_mc",
        "entero no negativo", 0.0, INT_MAX);
    //Lectura y asignacion del número de barrido medio
    parametros.num_barr_med = (int)validar_parametro_int(root, "num_barr_med",
        "entero positivo", 1.0, INT_MAX);
    if (parametros.temp_ini == 0.0) {
        printf("Error en el valor de la temperatura inicial, tiene que ser mayor a 0.0\n");
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    if (parametros.temp_ini == parametros.temp_fin) {
        printf("Error en los valores de la temperatura, la temperatura inicial tiene que ser diferente a la temperatura final\n");
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);
    return parametros;
}

static long long validar_parametro_int(cJSON *root, const char *nombre_dato, const char *tipo_dato, double lim_inf, double lim_sup) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(root, nombre_dato);
    if (item == NULL || cJSON_IsNull(item)) {
        mensaje_dato_nulo(root,nombre_dato);
    }
    if (!cJSON_IsNumber(item)) {
        mensaje_dato_erroneo(root,nombre_dato,tipo_dato);
    }
    double aux = item->valuedouble;
    //Condiciones límite en el parametro
    if (!(aux >= lim_inf && aux <= lim_sup)) {
        mensaje_dato_fuera_de_rango(root,nombre_dato,lim_inf,lim_sup);
    }
    //Entero exacto
    if (aux != floor(aux)) {
        mensaje_dato_erroneo(root,nombre_dato,tipo_dato);
    }
    return aux;
}

static double validar_parametro_double(cJSON *root, const char *nombre_dato, const char *tipo_dato, double lim_inf, double lim_sup) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(root, nombre_dato);
    if (item == NULL || cJSON_IsNull(item)) {
        mensaje_dato_nulo(root,nombre_dato);
    }
    if (!cJSON_IsNumber(item)) {
        mensaje_dato_erroneo(root,nombre_dato,tipo_dato);
    }
    double aux = item->valuedouble;
    //Condiciones límite en el parametro
    if (!(aux >= lim_inf && aux <= lim_sup)) {
        mensaje_dato_fuera_de_rango(root,nombre_dato,lim_inf,lim_sup);
    }
    if (!isfinite(aux)) {
        mensaje_dato_erroneo(root,nombre_dato,tipo_dato);
    }
    return aux;
}

static _Noreturn void mensaje_dato_nulo(cJSON *root, const char *nombre_dato) {
    printf("\n\tError de validacion el archivo cJSON: [DATO VACIO]\n");
    printf("No se ha ingresado ningun dato para '%s'\n", nombre_dato);
    cJSON_Delete(root);
    exit(EXIT_FAILURE);
}

static _Noreturn void mensaje_dato_erroneo(cJSON *root, const char *nombre_dato, const char *tipo_dato) {
    printf("\n\tError de validacion el archivo cJSON: [TIPO DE DATO ERRONEO]\n");
    printf("El dato ingresado para '%s' no es del tipo solicitado [ %s ]\n", nombre_dato, tipo_dato);
    cJSON_Delete(root);
    exit(EXIT_FAILURE);
}

static _Noreturn void mensaje_dato_fuera_de_rango(cJSON *root, const char *nombre_dato, double lim_inf, double lim_sup) {
    printf("\n\t Error de validacion en el archivo cJSON: [VALOR FUERA DE RANGO]\n");
    printf("El valor asignado a '%s' excede los parametros permitidos. Se espera un valor entre %.16g y %.16g \n", nombre_dato, lim_inf, lim_sup);
    cJSON_Delete(root);
    exit(EXIT_FAILURE);
}

static void lectura_asignacion_arr_largo_red(cJSON *root, parametros_t *parametros) {

    cJSON *arr_largo_red_item = cJSON_GetObjectItemCaseSensitive(root, "arr_largo_red");
    if (arr_largo_red_item == NULL || cJSON_IsNull(arr_largo_red_item)) {
        mensaje_dato_nulo(root,"arr_largo_red");
    }
    if (!cJSON_IsArray(arr_largo_red_item)) {
        mensaje_dato_erroneo(root, "arr_largo_red", "se espera un arreglo del largo del tamanio de red");
    }
    if (cJSON_GetArraySize(arr_largo_red_item) < 1 || cJSON_GetArraySize(arr_largo_red_item) > 16) {
        printf("Error: el tamanio del arreglo 'arr_largo_red_item' debe ser mayor a 1 y menor a 16.\n");
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    parametros->cont_tam_red = cJSON_GetArraySize(arr_largo_red_item);
    parametros->arr_largo_red = malloc(parametros->cont_tam_red * sizeof(int));
    if (parametros->arr_largo_red == NULL) {
        printf("Error, no se pudo asignar memoria para arr_largo_red\n");
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < parametros->cont_tam_red; i++) {
        cJSON *elemento = cJSON_GetArrayItem(arr_largo_red_item, i);
        if (!cJSON_IsNumber(elemento) || elemento->valuedouble != (double)elemento->valueint) {
            printf("Error: El elemento en la posicion %d de 'arr_largo_red_item' debe ser un entero mayor o igual a 4 y menor a 1024\n",i);
            //Libera memoria que acabamos de recibir
            libera_memoria_arr_largo_red(parametros);
            cJSON_Delete(root);
            exit(EXIT_FAILURE);
        }
        //Considerar si reservo este if, creo que evitaria tiempo de computo innecesariamente largo, pero el codigo podria manejarlo
        if (elemento->valueint < 4 || elemento->valueint > 1024 ) {
            printf("Error: El valor %d en la posicion %d esta fuera del rango permitido (4 a 1024)\n", elemento->valueint, i);
            libera_memoria_arr_largo_red(parametros);
            cJSON_Delete(root);
            exit(EXIT_FAILURE);
        }
        parametros->arr_largo_red[i] = elemento->valueint;
    }

    //Ordenacion de datos por metodo burbuja
    for (int i = 0; i < parametros->cont_tam_red - 1; i++) {
        for (int j = 0; j < parametros->cont_tam_red - i - 1; j++) {
            if (parametros->arr_largo_red[j] > parametros->arr_largo_red[j + 1]) {
                int aux = parametros->arr_largo_red[j];
                parametros->arr_largo_red[j] = parametros->arr_largo_red[j + 1];
                parametros->arr_largo_red[j + 1] = aux;
            }
        }
    }
    //Rechazo de repetidos
    for (int i = 0; i < parametros->cont_tam_red - 1; i++) {
        if (parametros->arr_largo_red[i] == parametros->arr_largo_red[i + 1]) {
            printf("Error: El arreglo 'arr_largo_red_item' contiene el valor %d repetido\n",parametros->arr_largo_red[i]);
            libera_memoria_arr_largo_red(parametros);
            cJSON_Delete(root);
            exit(EXIT_FAILURE);
        }
    }
}

void libera_memoria_arr_largo_red(parametros_t *parametros) {
    free(parametros->arr_largo_red);
    parametros->arr_largo_red = NULL;
    parametros->cont_tam_red = 0;
}