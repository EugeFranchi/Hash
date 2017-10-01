#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAMANIO_INICIAL 10

typedef struct hash_campo{
	char* clave;
	void* valor;
} hash_campo_t;

typedef struct hash{
	hash_campo_t* tabla;
	size_t tam;
	size_t cant;
} hash_t;

/* ************************************************************
 *                             HASH
 * ***********************************************************/ 
hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc( sizeof(hash_t));
	if (!hash){
		return NULL;
	}
	
	hash_campo_t* datos = malloc( sizeof(hash_campo_t) * TAMANIO_INICIAL);
	if (!datos){
		return NULL;
	}
	
	hash->tabla = datos;
	hash->cant = 0;
	hash->tam = TAMANIO_INICIAL;
	
	return hash;
}

