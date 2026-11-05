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
#include <stdbool.h>
#include <string.h>

#include "cJSON.h"

static void validacion_de_lectura_llave(cJSON *root);
static _Noreturn void error_de_lectura_llave(cJSON *root, const char *tipo_error);
static _Noreturn void error_de_lectura_archivo(FILE *archivo, const char *tipo_error);
static long long validacion_parametro_int(cJSON *root, const char *nombre_dato, const char *tipo_dato, double lim_inf, double lim_sup);
static double validacion_parametro_double(cJSON *root, const char *nombre_dato, const char *tipo_dato, double lim_inf, double lim_sup);
static void lectura_asignacion_arr_largo_red(cJSON *root, parametros_t *parametros);
static _Noreturn void mensaje_dato_nulo(cJSON *root, const char *nombre_dato);
static _Noreturn void mensaje_dato_erroneo(cJSON *root, const char *nombre_dato, const char *tipo_dato);
static _Noreturn void mensaje_dato_fuera_de_rango(cJSON *root, const char *nombre_dato, double lim_inf, double lim_sup);
static _Noreturn void error_root(cJSON *root);

//Lectura y validacion de los datos del archivo de entrada
char *lectura_archivo_cjson(const char *nombre_archivo) {
    //Apertura del archivo cJSON en modo binario para lectura
    FILE *archivo = fopen(nombre_archivo, "rb");
    //Validacion archivo nulo (no se pudo abrir archivo)
    if (archivo == NULL) error_de_lectura_archivo(archivo, "NO SE PUDO ABRIR EL ARCHIVO");
    //Recorrido del archivo hasta el final del mismo para determinar su tamaño
    if (fseek(archivo, 0, SEEK_END) != 0) error_de_lectura_archivo(archivo, "NO SE ENCUENTRA EL FINAL DEL ARCHIVO");
    //Obtencion del tamanio del archivo en bytes
    long tam = ftell(archivo);
    if (tam < 0) error_de_lectura_archivo(archivo, "ERROR AL OBTENER EL TAMANIO DEL ARCHIVO");
    //Validacion del archivo vacio (no tiene datos)
    if (tam == 0) error_de_lectura_archivo(archivo, "ARCHIVO VACIO");
    //Control tamaño maximo archivo, 1mb
    if (tam > (1024*1024)) error_de_lectura_archivo(archivo,"EL ARCHIVO SUPERA EL TAMANIO MAXIMO PERMITIDO (1 mb)");
    //Reposicionamiento del puntero al inicio del archivo para empezar lectura
    if (fseek(archivo, 0, SEEK_SET) != 0) error_de_lectura_archivo(archivo, "ERROR AL LEER EL ARCHIVO");
    //Asignacion de memoria dinamica temporal para la transferencia de los datos del archivo entrada a parametros
    char *buffer = malloc(tam +1);
    if (buffer == NULL) error_de_lectura_archivo(archivo, "ERROR DE MEMORIA AL CREAR EL BUFFER");
    //Comprobacion de los bytes reales del archivo, para evitar obtener basura
    size_t bytes_leidos = fread(buffer, 1, tam, archivo);
    if (bytes_leidos < (size_t)tam && ferror(archivo)) error_de_lectura_archivo(archivo, "ERROR AL LEER EL CONTENIDO DEL ARCHIVO");
    //Seguro para fin de cadena
    buffer[bytes_leidos] = '\0';
    fclose(archivo);
    return buffer;
}

void error_de_lectura_archivo(FILE *archivo, const char *tipo_error) {
    fprintf(stderr,"\nError de lectura de archivo cJSON: [%s]\n", tipo_error);
    if (archivo != NULL) fclose(archivo);
    exit(EXIT_FAILURE);
}

parametros_t gestor_de_parametros(const char *json_string) {
    //Inicio del struct parametro en ceros
    parametros_t parametros = {0};
    //Parseo (to parse) de datos de texto a cJSON (origen/raiz o root)
    cJSON *root = cJSON_Parse(json_string);
    //Archivo vacio o llenado incorrectamente
    if (root == NULL) {
        //Obtencion de error por parte de cJSON
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
            printf("\nError de validacion de archivo cJSON: [%ld]\n", posicion+1);
            printf("Texto previo al error: \"%.*s\"", long_contexto, contexto_ptr);
        }else {
            printf("\nError de validacion de archivo cJSON: [ERROR DESCONOCIDO]\n");
        }
        exit(EXIT_FAILURE);
    }
    //Validacion de que las llaves ingresadas en cJSON correspondan a las solicitadas
    validacion_de_lectura_llave(root);
    //Lectura y asignacion de la semilla global
    parametros.semilla_global = (uint64_t)validacion_parametro_int(root, "semilla_global",
        "entero no negativo", 0.0, (double)((1ULL <<53)-1));
    //Lectura y asignacion de los largos de red
    lectura_asignacion_arr_largo_red(root, &parametros);
    //Lectura y asignacion de J (intensidad de acoplamiento)
    parametros.J_intns_aco = validacion_parametro_double(root, "J_intns_aco",
        "Numero real, se sugiere 1.0 para un material ferromagnetico",-J_MAX,J_MAX);
    if(parametros.J_intns_aco == 0.0) {
        printf("\n\tAVISO de validacion de parametros en cJSON: [J = 0.0]\n");
        printf("Valor actual para la constante de intensidad de acoplamiento J = %f\n", parametros.J_intns_aco);
        printf("El experimento solo considerara la aportacion del campo magnetico externo\n");
    }
    //Lectura y asignacion de h (Campo magnetico externo)
    parametros.h_cpo_mgntc_extr = validacion_parametro_double(root, "h_cpo_mgntc_extr",
        "numero real, se sugiere 0 para no considerar la aportacion del campo",-H_MAX, H_MAX);
    if (parametros.J_intns_aco == 0.0 && parametros.h_cpo_mgntc_extr == 0.0) {
        printf("\n\tError de validacion de parametros en cJSON: [TRIVIALIDAD DE EXPERIMENTO]\n");
        printf("Valor actual para la constante de intensidad de acoplamiento J = %f\n", parametros.J_intns_aco);
        printf("Valor actual para la constante del campo magnetico externo h = %f\n", parametros.h_cpo_mgntc_extr);
        printf("El experimento se vuelve trivial\n");
        error_root(root);
    }
    //Lectura y asignacion de la temperatura inicial
    parametros.temp_ini = validacion_parametro_double(root, "temp_ini",
        "numero real positivo", 0.0, TEMP_MAX);
    if (parametros.temp_ini == 0.0) {
        printf("Error en el valor de la temperatura inicial, tiene que ser mayor a 0.0\n");
        error_root(root);
    }
    //Lectura y asignacion de la temperatura final
    parametros.temp_fin = validacion_parametro_double(root, "temp_fin",
        "numero real positivo mayor al valor de la temperatura inicial", parametros.temp_ini, TEMP_MAX);
    if (parametros.temp_ini == parametros.temp_fin) {
        printf("Error en los valores de la temperatura, la temperatura inicial tiene que ser diferente a la temperatura final\n");
        error_root(root);
    }
    //Lectura y asignacion del número de intervalos de temperatura
    parametros.n_temp = (int)validacion_parametro_int(root, "n_temp",
        "al menos dos puntos de temperatura", 2, N_TEMP_MAX);
    //Lectura y asignacion del número de pasos de termalización (Montecarlo)
    parametros.num_therm_mc= (int)validacion_parametro_int(root, "num_therm_mc",
        "entero no negativo", 0.0, INT_MAX);
    //Lectura y asignacion del número de barrido medio
    parametros.num_barr_med = (int)validacion_parametro_int(root, "num_barr_med",
        "entero positivo", 1.0, INT_MAX);
    //Número de vecinos, importante considerar si se quiere usar para más tamanios
    parametros.n_vec = 4;
    cJSON_Delete(root);
    return parametros;
}

static void validacion_de_lectura_llave(cJSON *root) {
    const char *llaves_validas[] = {"semilla_global", "arr_largo_red", "J_intns_aco", "h_cpo_mgntc_extr", "temp_ini",
        "temp_fin","n_temp", "num_therm_mc", "num_barr_med"};
    const int total_llaves = sizeof(llaves_validas)/sizeof(llaves_validas[0]);
    if (!cJSON_IsObject(root)) error_de_lectura_llave(root, "ESCRITURA NO VALIDA");
    //Recorrido lineal de todos los hijos del objeto raiz (root)
    cJSON *elemento = root->child;
    while (elemento != NULL) {
        bool llave_encontrada = false;
        //Comprobadion de que el elemento esté en la lista de las llaves
        for (int i = 0; i < total_llaves; i++) {
            if (strcmp(elemento->string, llaves_validas[i]) == 0) {
                llave_encontrada = true;
                break;
            }
        }
        if (!llave_encontrada){
            fprintf(stderr, "%s\n", elemento->string);
            error_de_lectura_llave(root, "LLAVE DESCONOCIDA");
        }
        //Pasamos a validar el siguiente elemento
        elemento = elemento->next;
    }
}

static void error_de_lectura_llave(cJSON *root, const char *tipo_error) {
    fprintf(stderr, "Error de validacion de llaves archivo cJSON: [%s]\n", tipo_error);
    if (!cJSON_IsObject(root)) fprintf(stderr, "El archivo debe contener un objeto {...} en su raiz\n");
    error_root(root);
}

static long long validacion_parametro_int(cJSON *root, const char *nombre_dato, const char *tipo_dato, double lim_inf, double lim_sup) {
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

static double validacion_parametro_double(cJSON *root, const char *nombre_dato, const char *tipo_dato, double lim_inf, double lim_sup) {
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
    fprintf(stderr, "Error de validacion en el archivo cJSON: [DATO VACIO]\n");
    fprintf(stderr, "NO SE HA INGRESADO NINGUN DATO PARA '%s'\n", nombre_dato);
    error_root(root);
}

static _Noreturn void mensaje_dato_erroneo(cJSON *root, const char *nombre_dato, const char *tipo_dato) {
    fprintf(stderr, "Error de validacion en el archivo cJSON: [TIPO DE DATO ERRONEO]\n");
    fprintf(stderr, "EL DATO INGRESADO PARA '%s' NO ES EL SOLICITADO: ['%s']\n", nombre_dato, tipo_dato);
    error_root(root);
}

static _Noreturn void mensaje_dato_fuera_de_rango(cJSON *root, const char *nombre_dato, double lim_inf, double lim_sup) {
    printf("\n\t Error de validacion en el archivo cJSON: [VALOR FUERA DE RANGO]\n");
    printf("El valor asignado a '%s' excede los parametros permitidos. Se espera un valor entre %.16g y %.16g \n", nombre_dato, lim_inf, lim_sup);
    error_root(root);
}

static void lectura_asignacion_arr_largo_red(cJSON *root, parametros_t *parametros) {
    //Asignacion item para validar llave
    cJSON *arr_largo_red_item = cJSON_GetObjectItemCaseSensitive(root, "arr_largo_red");
    //Dato no nulo y que sea numero
    if (arr_largo_red_item == NULL || cJSON_IsNull(arr_largo_red_item)) mensaje_dato_nulo(root,"arr_largo_red");
    //Dato debe de ser un arreglo
    if (!cJSON_IsArray(arr_largo_red_item)) mensaje_dato_erroneo(root, "arr_largo_red", "se espera un arreglo del largo del tamanio de red");
    //El arreglo debe de contener al menos un dato
    if (cJSON_GetArraySize(arr_largo_red_item) < 1 || cJSON_GetArraySize(arr_largo_red_item) > 16) {
        fprintf(stderr, "Error de validacion del archivo cJSON: [TAMANIO DEL ARREGLO 'arr_largo_red' DEBE SER MAYOR O IGUAL A 1 Y MENOR O IGUAL A 16]\n");
        error_root(root);
    }
    //Obtencion tamanio de red
    parametros->cont_tam_red = cJSON_GetArraySize(arr_largo_red_item);
    parametros->arr_largo_red = malloc(parametros->cont_tam_red * sizeof(int));
    if (parametros->arr_largo_red == NULL) {
        fprintf(stderr, "Error de validacion del archivo cJSON: [NO SE PUEDE ASIGNAR MEMORIA PARA 'arr_largo_red']\n");
        error_root(root);
    }
    for (int i = 0; i < parametros->cont_tam_red; i++) {
        cJSON *elemento = cJSON_GetArrayItem(arr_largo_red_item, i);
        if (!cJSON_IsNumber(elemento) || elemento->valuedouble != (double)elemento->valueint) {
            fprintf(stderr, "Error de validacion del archivo cJSON: [ERROR ELEMENTO DEL ARREGLO 'arr_largo_red']\n");
            fprintf(stderr, "EL ELEMENTO DE LA POSICION %d DE 'arr_largo_red' DEBE SER UN ENTERO MAYOR O IGUAL A 4 Y MENOR O IGUAL A 1024\n", i);
            //Libera memoria que acabamos de recibir
            libera_memoria_arr_largo_red(parametros);
            error_root(root);
        }
        //Considerar si reservo este if, creo que evitaria tiempo de cómputo innecesariamente largo, pero el codigo podria manejarlo
        if (elemento->valueint < 4 || elemento->valueint > L_MAX ) {
            fprintf(stderr, "Error de validacion del archivo cJSON: [ERROR ELEMENTO DEL ARREGLO 'arr_largo_red']\n");
            printf("EL VALOR %d EN LA POSICION %d esta fuera del rango permitido [4 : %d]\n", elemento->valueint, i, L_MAX);
            libera_memoria_arr_largo_red(parametros);
            error_root(root);
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
            fprintf(stderr, "Error de validacion del archivo cJSON: [ERROR ELEMENTO DEL ARREGLO 'arr_largo_red']\n");
            fprintf(stderr, "CONTIENE EL VALOR %d REPETIDO\n",parametros->arr_largo_red[i]);
            libera_memoria_arr_largo_red(parametros);
            error_root(root);
        }
    }
}
static _Noreturn void error_root(cJSON *root) {
    cJSON_Delete(root);
    exit(EXIT_FAILURE);
}
void libera_memoria_arr_largo_red(parametros_t *parametros) {
    free(parametros->arr_largo_red);
    parametros->arr_largo_red = NULL;
    parametros->cont_tam_red = 0;
}