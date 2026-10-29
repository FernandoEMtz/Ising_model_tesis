#include <stdio.h>

#include "gestor_red.h"
#include "gestor_observables.h"
//Los .c van en /src y los .h en /include

static int validar_configuraciones_red(void);

#define OK    1
#define FALLO 0

int main(void) {
    if (validar_configuraciones_red() == OK) {
        printf("\nOK (Todas las condiciones se cumplen)\n");
    } else {
        printf("\n FALLO (Alguna condicion fallo)\n");
    }
    return 0;
}

static int validar_configuraciones_red(void) {

    caract_red_t red = genera_red(16);
    inicia_randomizador_pcg64(&red.generador_rng, 20260830, 1);
    inicio_red_spines(&red);
    int i = 0, aux3 = 0;
    double aux = 0.0, aux1 = 0.0, aux2 = 0.0;

    int estado_prueba = OK;

    for (i = 0; i < red.n_sp; i++) {
        aux += red.spin[i];
    }
    aux /= red.n_sp;
    printf("Promedio inicial %lf\n", aux);
    if (aux == 1.0 || aux == -1.0) estado_prueba = FALLO;
    printf("\tPrimera prueba: %d\n",estado_prueba);

    //Para todos los spines en +1
    for (i = 0; i < red.n_sp; i++) {
        red.spin[i] = 1;
    }
    aux1 = funcion_hamiltoniano(&red);
    aux2 = funcion_magnetizacion(&red);
    printf("Todo +1 \t energia %lf \tmagnetizacion por spin %lf\n", aux1, aux2);
    if (aux1 != -512.00 || aux2 != 1.0) estado_prueba = FALLO;
    printf("\tSegunda prueba: %d\n",estado_prueba);

    //Para todos los spines en -1
    for (i = 0; i < red.n_sp; i++) {
        red.spin[i] = -1;
    }
    aux1 = funcion_hamiltoniano(&red);
    aux2 = funcion_magnetizacion(&red);
    printf("Todo -1 \t energia %lf \tmagnetizacion por spin %lf\n", aux1, aux2);
    if (aux1 != -512.00 || aux2 != -1.0) estado_prueba = FALLO;
    printf("\tTercera prueba: %d\n",estado_prueba);

    //Spines en tablero
    for (i = 0; i < red.n_sp; i++) {
        if ((i/red.largo_red + i % red.largo_red) % 2 == 0) red.spin[i] = -1; else red.spin[i] = 1;
    }
    aux1 = funcion_hamiltoniano(&red);
    aux2 = funcion_magnetizacion(&red);
    printf("Tablero \t energia %lf \tmagnetizacion por spin %lf\n", aux1, aux2);
    if (aux1 != 512.00 || aux2 != 0.0) estado_prueba = FALLO;
    printf("\tCuarta prueba:  %d\n",estado_prueba);

    //Para todos los spines en +1 y el ultimo en -1
    for (i = 0; i < red.n_sp; i++) {
        red.spin[i] = 1;
        if (i == red.n_sp -1) red.spin[i] = -1;
    }
    aux1 = funcion_hamiltoniano(&red);
    aux2 = funcion_magnetizacion(&red);
    printf("255 | -1 \t energia %lf \tmagnetizacion por spin %lf\n", aux1, aux2);
    for (i = 0; i < red.n_sp; i++) {
        if (suma_de_vecinos(&red, i) != 4) {
            printf("Posicion: %d\n", i);
            aux3++;
        }
    }
    printf("Sitios que no dan 4: \t%d de %d\n", aux3, red.n_sp);
    if (aux1 != -504.00 || aux2 != 0.9921875 || aux3 != 4) estado_prueba = FALLO;
    printf("\tQuinta prueba: %d\n",estado_prueba);

    libera_memoria_red(&red);
    return estado_prueba;
}