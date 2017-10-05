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

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if(!clave)
		return false ;
	
	int pos_clave = posicion_clave(hash->tabla, clave) ;

	if(pos_clave == -1){
		int clave_hasheada = hashear_clave(clave, hash->tam) ;
		int pos_vacia = hallar_pos_vacia(hash->tabla, hash->tam, clave_hasheada) ;
		
		if(pos_vacia == -1)
			return false ;
		
		hash->tabla[pos_vacia]->clave = malloc(sizeof(char) * strlen(clave)+1) ;
		if(!hash->tabla[pos_vacia]->clave)
			return false ;
		strcpy(hash->tabla[pos_vacia]->clave,clave);
		hash->tabla[pos_vacia]->valor = dato ;
		hash->tabla[pos_vacia]->estado = OCUPADO ;
		hash->cant ++ ;
	}
	
	else{
		if(hash->destruir_dato)
			hash->destruir_dato(hash->tabla[pos_clave]->valor) ;
		hash->tabla[pos_clave]->valor = dato ;
	}

	return true ;
}

void *hash_borrar(hash_t *hash, const char *clave){
	int pos = posicion_clave(hash, clave) ;
	if(pos == -1)
		return NULL ;

	void* dato = hash->tabla[pos]-> valor ;
	hash->tabla[pos]->estado = BORRADO ;
	hash->cant -- ;

	//AGREGAR REDIMENSION
	
	return dato ;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	int pos = posicion_clave(hash, clave) ;
	if(pos == -1)
		return NULL ;

	void* dato = hash->tabla[pos]-> valor ;

	return dato ;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	int pos = posicion_clave(hash, clave) ;
	if(pos == -1)
		return false ;
	return true ;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cant ;
}



//FUNCIONES AUXILIARES//

//Devuelve la primera posicion vacia a partir de la posicion que arroja la funcion
//si  no la encuentra devuelve -1
int hallar_pos_vacia(hash_campo_t* tabla, size_t tam, int clave_h){
	int pos = clave_h ;
	 while(pos != clave_h -1){
	 	if(tabla[pos]->estado == VACIO)
	 		return pos ;
	 	//cuando buscó hasta el final de la tabla vuelve a empezar desde el ppio
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
	if(!clave)
		return -1 ;
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
