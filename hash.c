#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAMANIO_INICIAL 10

typedef struct hash_campo{
	char* clave;
	void* valor;
	estado_t estado_campo ;
} hash_campo_t;

typedef struct hash{
	hash_campo_t* tabla;
	size_t tam;
	size_t cant;
	hash_destruir_dato_t destruir_dato;
} hash_t;

typedef enum estado_campo = {VACIO,BORRADO,OCUPADO} estado_t ;

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
	hash->destruir_dato = destruir_dato;
	for(int i = 0 ; i < hash->tam ; i++)
		hash->tabla[i]->estado_campo = VACIO ;
	
	return hash;
}

void hash_destruir(hash_t *hash){
	destruir_contenido_tabla(hash) ;
	free(hash->tabla) ;
	free(hash);
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato){

	int pos_clave = posicion_clave(hash->tabla, clave) ;

	if(pos_clave == -1){
		int clave_hasheada = hashear_clave(clave, hash->tam) ;
		int pos = hallar_pos_vacia(hash->tabla, hash->tam, clave_hasheada) ;
		if(pos==-1)
			return false ;
		hash->tabla[pos]->clave = clave ;
		hash->tabla[pos]->valor = dato ;
		hash->tabla[pos]->estado = OCUPADO ;
	}
	else
		hash->tabla[pos_clave]->valor = dato ;
	
	return true ;
}

//FUNCIONES AUXILIARES//

//Devuelve la primera posicion vacia a partir de la posicion que arroja la funcion
//si  no la encuentra devuelve -1
int hallar_pos_vacia(hash_campo_t* tabla, size_t tam, int clave_h){
	int pos = clave_h ;
	 while(pos != clave_h -1){
	 	if(tabla[pos]->estado == VACIO)
	 		return pos ;
	 	if(pos != tam-1)
	 		pos ++ ;
	 	else
	 		pos = 0 ;
	 }
	 return -1 ;
}

//Busca en la tabla la clave que le pasan por parametro
//si la encuentra, devuelve la posicion, sino devuelve -1
int posicion_clave(hash_t* hash, char*clave){
	int clave_hasheada = hashear_clave(clave, hash->tam) ;
	int actual = clave_hasheada ;
	while(hash->tabla[actual]->estado != VACIO){
		if(strcmp(hash->tabla[actual]->clave ,clave) == 0)
			return actual ;
	}
	return -1 ;
}

int hashear_clave(char* clave, size_t tam){

}

void destruir_contenido_tabla(hash_t* hash){
	for(int i = 0 ; i < hash->tam ; i++){
		if(hash->tabla[i]->estado_campo == VACIO)
			continue ;
		if(hash->destruir_dato){
			destruir_dato(hash->tabla[i]->valor) ;
			free(hash->tabla[i]->clave) ;
		}
	}
}
