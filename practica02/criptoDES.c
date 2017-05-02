/**
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/des_crypt.h>
#include <limits.h>
#include <stdint.h>
#define MAX 268435455 // 2^28 - 1

char claves[MAX][8]; 

void hexprint(unsigned char *msg)
{
	for (int i = 0; i < sizeof(msg); ++i)
		printf("%02X ", msg[i]);
	printf("\n");
}

int cifrar(char *mensaje, char *clave)
{
	return ecb_crypt(clave, mensaje, sizeof(mensaje), DES_ENCRYPT);
}

int descifrar(char *cifrado, char *clave)
{
	return ecb_crypt(clave, cifrado, sizeof(cifrado), DES_DECRYPT);
}

unsigned char *ullong_to_bytearray(
	unsigned long long n
	)
{
	unsigned char *ret = calloc(sizeof(long long), sizeof(char));
	if (ret == NULL) return NULL;

	for (int i = 7; i >= 0; --i)
		ret[7-i] = (int) (n >> (8*i)) & 0xFF;

	return ret;
}

void genera_claves()
{
	printf("Generando claves...");
	fflush(stdout);
	for (unsigned long long i = 0; i < MAX; ++i)
	{
		for (int j = 7; j >= 0; --j)
		{
			claves[i][7-j] = (i >> 8*j) & 0xFF;
		}
		// des_setparity(claves[i]);
	}
	printf(" hecho.\n");
}

int fuerza_bruta(char *cifrado, char *mensaje)
{

}

int main(int argc, char *argv[])
{
	genera_claves();
	char mensaje[8] = "aaaaaaa";
	char clave[8] = "qqqqqqq";
	// des_setparity(clave);
	// printf("Encriptar: \n");
	// printf("Mensaje: ");
	// hexprint(mensaje);
	// printf("Clave: ");
	// hexprint(clave);
	// // printf("%d\n", DES_FAILED(cifrar(mensaje, clave)));
	// printf("Mensaje cifrado: ");
	// hexprint(mensaje);

	// printf("Desencriptar: \n");
	// printf("Mensaje cifrado: ");
	// hexprint(mensaje);
	// printf("Clave: ");
	// hexprint(clave);
	// // printf("%d\n", DES_FAILED(descifrar(mensaje, clave)));
	// printf("Mensaje descifrado: ");
	// hexprint(mensaje);

	// printf("Ataque por fuerza bruta: \n");
	// fuerza_bruta(NULL,NULL);
	// for (int i = 0; i < 0x101; ++i)
	// 	hexprint(claves[i]);
	return 0;
}