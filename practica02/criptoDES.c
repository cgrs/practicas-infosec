/**
 * Seguridad Informática
 * Práctica 2. DES
 * Ernesto Boado Moreira
 * Carlos González Recio
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/des_crypt.h>

#define MAX 9223372036854775807 // 2^63 - 1

long fsize(
	FILE *file
	)
{
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	rewind(file);
	return size;
}

void uso(){
	printf("Uso: criptoDES [-e|-d|-a] [fichero]\n"
		   "  -e: cifrar fichero\n"
		   "  -d: descifrar fichero\n"
		   "  -a: atacar la clave del fichero por fuerza bruta\n"
		   "      se requiere conocer al menos cuatro caracteres\n"
		   "      del mensaje original (especificados por entrada estándar)\n"
		   "\n"
		   "  Si no se especifica el fichero, se asume la entrada estándar.\n");
}

void hexprint(unsigned char *msg, size_t msgsize)
{
	for (int i = 0; i < msgsize; ++i)
		printf("%02X ", msg[i]);
	printf("\n");
}

int cifrar(char *mensaje, size_t msgsize, char *clave)
{
	return ecb_crypt(clave, mensaje, msgsize, DES_ENCRYPT);
}

int descifrar(char *cifrado, size_t msgsize, char *clave)
{
	return ecb_crypt(clave, cifrado, msgsize, DES_DECRYPT);
}

char *read_mensaje(size_t tam) {
	char *buf = calloc(tam, sizeof(char));
	// fprintf(stderr, "Mensaje: ");
	fgets(buf, tam, stdin);
	buf[strcspn(buf, "\r\n")] = 0;
	return buf;
}

char *normalizar(char *mensaje, size_t *msgsize) {
	if (*msgsize % 8 == 0) return mensaje;
	size_t nsize = *msgsize / 8 + 1;
	*msgsize = 8 * nsize;
	return realloc(mensaje, *msgsize);

}

char *get_mensaje(char *ruta, size_t *msgsize) {
	FILE *fp = fopen(ruta, "r");
	if (fp == NULL) {
		perror(ruta);
		exit(1);
	}
	*msgsize = fsize(fp);
	char *buf = calloc(*msgsize, sizeof(char));
	fread(buf, 1, *msgsize, fp);
	// hexprint(buf, *msgsize);
	return buf;
}

char *get_clave() {
	char *clave = calloc(8, sizeof(char));
	// fprintf(stderr,"Clave (máx. 4 caracteres): ");
	fgets(clave, 8, stdin);
	clave[strcspn(clave, "\r\n")] = 0;
	// printf("%ld\n", strlen(clave));
	long len = strlen(clave);
	for (int i = 0; i < len; ++i)
	{
		clave[8-len+i] = clave[i];
		clave[i] = 0;
	}
	// hexprint(clave, 8);
	des_setparity(clave);
	return clave;
}

char *fuerza_bruta(char *cifrado, char *mensaje)
{
	char *clave = calloc(8, sizeof(char));
	char *copia = calloc(strlen(cifrado), sizeof(char));
	for (unsigned long long i = 0; i < MAX; ++i)
	{
		for (int j = 7; j >= 0; --j)
		{
			clave[7-j] = (i >> 8*j) & 0xFF;
		}
		des_setparity(clave);
		// hexprint(clave, 8);
		size_t msgsize = strlen(cifrado);
		strncpy(copia, cifrado, msgsize);
		descifrar(copia, msgsize, clave);
		if (strncmp(copia, mensaje, 4) == 0) {
			return clave;
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 2) { // no hay argumentos
		uso();
		return 1;
	}
	if (argc >= 2) { // dos o más argumentos
		char *clave = {0};
		char *mensaje = {0};
		size_t msgsize;
		if (argc == 2) { // mensaje -> stdin
			mensaje = read_mensaje(100); // tamaño máximo del mensaje: 100
			msgsize = strlen(mensaje);
		}
		else {
			mensaje = get_mensaje(argv[2], &msgsize);
			// msgsize = fsize(fopen(argv[2], "r"));
			// printf("%ld\n", msgsize);
		}
		mensaje = normalizar(mensaje, &msgsize);
		// hexprint(mensaje, msgsize);	
		if (strncmp(argv[1], "-e", 2) == 0) { // cifrar
			clave = get_clave();
			if (!DES_FAILED(cifrar(mensaje, msgsize, clave))) {
				// hexprint(mensaje);
				fprintf(stdout, "%s", mensaje);
				return 0;
			}
			else {
				fprintf(stderr, "Error al cifrar\n");
				return 1;
			}
		}
		else if (strncmp(argv[1], "-d", 2) == 0) { // descifrar
			clave = get_clave();
			// hexprint(clave, 8);
			if (!DES_FAILED(descifrar(mensaje, msgsize, clave))) {
				// hexprint(mensaje, msgsize);
				fprintf(stdout, "%s", mensaje);
				return 0;
			}
			else {
				fprintf(stderr, "Error al descifrar\n");
				return 1;
			}
		}
		else if (strncmp(argv[1], "-a", 2) == 0) { // atacar
			// fprintf(stderr, "Caracteres conocidos del mensaje: ");
			char *conocidos = read_mensaje(5);
			// hexprint(conocidos,4);
			clave = fuerza_bruta(mensaje, conocidos);
			if (!DES_FAILED(descifrar(mensaje, msgsize, clave))) {
				fprintf(stdout, "%s", mensaje);
				return 0;
			}
			else {
				fprintf(stderr, "Error al descifrar\n");
				return 1;
			}
		}
	}
	return 0;
}