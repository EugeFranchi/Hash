#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct hash_campo{
	char* clave;
	char* valor;
} hash_campo_t;

typedef struct hash{
	hash_campo_t* tabla;
	size_t tam;
	size_t cant;
} hash_t;

