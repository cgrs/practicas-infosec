/**
 * Seguridad Informática
 * Práctica 1. Esteganografía
 * Ocultación de mensajes de texto en ficheros de imágenes
 * Carlos González Recio - 70885474A
 * 3 de marzo, 2017
 *
 * Fichero de cabecera (librerías, macros y constantes)
 */

/** Librerías */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

/** Constantes */

#define BF_OFFSET 0x0A // posición del byte del offset (dónde empieza el array)
#define BF_BPP 0x1C // posición del byte de la profundidad de color

/** Funciones */

void uso(char *); // Imprime el uso del ejecutable

FILE *abrir(char *, char *); // Apertura de ficheros

void cerrar(FILE *); // Cierre de ficheros

long fsize(FILE *); // Tamaño de fichero

void *mapear(FILE *, size_t, int, int); // mapeo de ficheros en memoria

unsigned char *ulong_to_bytearray(unsigned long);

unsigned long bytearray_to_ulong(unsigned char *);

void encode(unsigned char **, size_t, unsigned char *, size_t);

unsigned char *decode(unsigned char *, size_t);