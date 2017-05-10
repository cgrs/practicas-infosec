/**
 * Seguridad Informática
 * Práctica 1. Esteganografía
 * Ocultación de mensajes de texto en ficheros de imágenes
 * Carlos González Recio - 70885474A
 * 3 de marzo, 2017
 *
 * Fichero fuente (código y funcionalidad)
 */

/** Librerías a utilizar */

#include "estego.h"

int main(
	int argc,
	char *argv[]
	)
{
	if(argc < 2) 
	{
		uso(argv[0]);
		return EXIT_FAILURE;
	}

	FILE *fp;
	unsigned char *bytes,
	              *mensaje = {0};
	
	fp = abrir(argv[1], "r+");
	size_t tam = fsize(fp);
	bytes = mapear(
		fp, 
		tam,
		PROT_WRITE,
		MAP_SHARED);

	if(bytes == NULL)
		return EXIT_FAILURE;

	if(strncmp(bytes, "BM", 2) == 0)
		{
			if(bytes[BF_BPP] == 24)
			{
				unsigned char *pixels = &bytes[bytes[BF_OFFSET]];
				size_t pixsize = (fsize(fp) - bytes[BF_OFFSET]);

				if (argc == 2)
				{
					mensaje = decode(pixels, pixsize);
					fprintf(stdout, "%s", mensaje);
				}
				else
				{
					size_t msg_size;

					if (strcmp(argv[2], "-") == 0)
					{
						msg_size = pixsize / 8;
						mensaje = calloc(msg_size, sizeof(char));

						if (mensaje == NULL) return EXIT_FAILURE;

						size_t msg_size2 = fread(mensaje, sizeof(char), msg_size, stdin);
						encode(&pixels, pixsize, mensaje, msg_size2);
					}
					else
					{
						FILE *fmsg;
						

						fmsg = abrir(argv[2], "r");
						msg_size = fsize(fmsg);

						mensaje = mapear(
							fmsg,
							msg_size,
							PROT_READ,
							MAP_PRIVATE);

						if (mensaje == NULL) return EXIT_FAILURE;

						encode(&pixels, pixsize, mensaje, msg_size);

						munmap(mensaje, msg_size);
						cerrar(fmsg);
					}

					printf("Datos codificados correctamente\n");
				}
				munmap(bytes, tam);
				cerrar(fp);
				return EXIT_SUCCESS;
			}
			fprintf(stderr, "Error: la imagen no tiene una profundidad de 24 bits/píxel\n");
			uso(argv[0]);
			return EXIT_FAILURE;
		}
		fprintf(stderr, "Error: la imagen no es un archivo BMP válido\n");
		uso(argv[0]);
		return EXIT_FAILURE;
}

/** Funciones de ficheros */

FILE *abrir(
	char *ruta,
	char *modo
	)
{
	FILE *fp;

	fp = fopen(ruta, modo);

	if (fp == NULL)
	{
		perror(ruta);
		exit(1);
	}
	else return fp;
}

void cerrar(
	FILE *fp
	)
{
	if ( fclose(fp) != 0 )
	{
		perror("Error al cerrar el fichero:");
		exit(1);
	}
}

off_t fsize(
	FILE *file
	)
{
	return lseek(fileno(file), 0, SEEK_END);
}

void *mapear(
	FILE *fp, 
	size_t tam,
	int prot,
	int flags
	)
{
	return mmap(
		NULL,
		tam,
		prot,
		flags,
		fileno(fp),
		0);
}

/** Funciones auxiliares */

unsigned char *ulong_to_bytearray(
	unsigned long n
	)
{
	unsigned char *ret = calloc(sizeof(long), sizeof(char));
	if (ret == NULL) exit(1);

	for (int i = 3; i >= 0; --i)
		ret[3-i] = (int) (n >> (8*i)) & 0xFF;

	return ret;
}

unsigned long bytearray_to_ulong(
	unsigned char *ba
	)
{
	unsigned long ret = 0;

	for (int i = 0; i < 4; ++i)
		ret += ba[i] << 8*(3-i);

	return ret;
}

void uso(
	char *ruta
	)
{
	printf(
		"Uso: %s imagen.bmp [mensaje]\n"
		"  imagen.bmp - Imagen portadora del mensaje \n"
		"               (debe tener una profundidad de 24 bits/píxel)\n"
		"     mensaje - Datos a codificar en la imagen\n"
		"               (si se pasa -, se recoge de la entrada estándar)\n"
		"\n"
		"  La imagen devuelve el mensaje si no se pasa el segundo argumento.\n"
		"  El mensaje se guarda en la imagen si se pasa.\n"
		"  La entrada y salida de datos es estándar."
		"\n"
		, ruta);
}

/** Funciones principales */

void encode(
	unsigned char **array,
	size_t array_size,
	unsigned char *msg,
	size_t msg_size
	)
{
	unsigned char *parray = *array;
	size_t lmax;

	lmax = array_size / 8; // 1bit LSB
	
	if(msg_size > lmax)
	{
		fprintf(stderr, "Error: El mensaje excede el máximo permitido (%lu).\n"
			, lmax);
		exit(EXIT_FAILURE);
	}
	
	unsigned char *lmsg = ulong_to_bytearray(msg_size);
	memcpy(parray,lmsg,4); // lmsg -> parray[0..3];

	for (int i = 0; i < msg_size; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			int ix = 4+j+8*i;
			parray[ix] = (parray[ix] & 0xFE) | ((msg[i] >> j) & 0x1);
		}
	}
	free(lmsg);
}

unsigned char *decode(
	unsigned char *array,
	size_t array_size
	)
{
	unsigned long lmsg = bytearray_to_ulong(array);
	if (lmsg > array_size) exit(EXIT_FAILURE);
	unsigned char *msg = calloc(lmsg, sizeof(char));
	if (msg == NULL)
		exit(EXIT_FAILURE);

	for (int i = 0; i < lmsg; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			int ix = 4+j+8*i;
			msg[i] |= (array[ix] & 0x1) << j;
		}
	}
	return msg;
}