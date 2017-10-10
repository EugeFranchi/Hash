#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAMANIO_INICIAL 10

typedef enum estado_campo {VACIO,BORRADO,OCUPADO} estado_t ;

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

struct hash_iter{
	hash_campo_t* act;
	const hash_t* hash;
};

/* ************************************************************
 *                     FUNCIONES AUXILIARES
 * ***********************************************************/

//Devuele una posicion segun la clave y el tamanio del hash.
size_t hashing (const char* clave, size_t tam){
	unsigned int num1 = 378551;
	unsigned int num2 = 63689;
	unsigned int clave_numerica = 0;
	for(int i = 0; *clave; clave++, i++){
		clave_numerica = clave_numerica * num2 + (*clave);
		num2 = num2 * num1;
	}
	return(clave_numerica%tam);
}

//Devuelve la primera posicion vacia a partir de la posicion que arroja la funcion
//si  no la encuentra devuelve -1
int hallar_pos_vacia(hash_campo_t* tabla, size_t tam, int clave_h){
	int pos = clave_h ;
	while(pos != clave_h -1){
		if(tabla[pos].estado_campo == VACIO)
	 		return pos ;
	 	//cuando buscó hasta el final de la tabla vuelve a empezar desde el ppio
	 	if(pos != tam-1)
	 		pos ++ ;
	 	else
	 		pos = 0 ;
	 }
	 return -1 ;
}

//Devuelve verdadero si esta vacio y false de lo contrario.
bool hash_esta_vacio(const hash_t* hash){
	return hash->cant == 0;
}

//Devuelve la siguiente posicion ocupada del hash, o -1 en caso de no haber.
int hallar_pos_ocupada(const hash_t* hash, size_t inicio){
	if (hash_esta_vacio(hash)){
		return -1;
	}
	for (size_t pos = inicio; pos < hash->tam; pos++){
		if(hash->tabla[pos].estado_campo == OCUPADO){
			return pos;
		}
	}
	return -1;
}

//Busca en la tabla la clave que le pasan por parametro
//si la encuentra, devuelve la posicion, sino devuelve -1
int posicion_clave(const hash_t* hash, const char*clave){
	if(!clave)
		return -1 ;
	int clave_hasheada = hashing(clave, hash->tam) ;
	int actual = clave_hasheada ;
	while(hash->tabla[actual].estado_campo != VACIO){
		if(strcmp(hash->tabla[actual].clave ,clave) == 0)
			return actual ;
	}
	return -1 ;
}


void destruir_contenido_tabla(hash_t* hash){
	for(int i = 0 ; i < hash->tam ; i++){
		if(hash->tabla[i].estado_campo == VACIO)
			continue ;
		if(hash->destruir_dato){
			hash->destruir_dato(hash->tabla[i].valor) ;
			free(hash->tabla[i].clave) ;
		}
	}
}


//Cambia el tamanio del hash por tam. Devuelve false si hay un error.
//Pre: el hash fue creado.
//Post: el hash tiene un tamanio tam.
bool hash_redimensionar(hash_t *hash, size_t tam){
	hash_campo_t* nueva_tabla = malloc(sizeof(hash_campo_t*)* tam);
	if (!nueva_tabla){
		return false;
	}
	
	hash_campo_t* tabla = hash->tabla;
	size_t tamanio = hash->tam;
	hash->tabla = nueva_tabla;
	hash->tam = tam;
	for (size_t pos = 0; pos < tamanio; pos++){
		hash_guardar(hash, tabla[pos].clave, tabla[pos].valor);
	}
	free(tabla);
	return true;
}

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
		hash->tabla[i].estado_campo = VACIO ;
	
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
	
	int pos_clave = posicion_clave(hash, clave) ;

	if(pos_clave == -1){
		int clave_hasheada = hashing(clave, hash->tam) ;
		int pos_vacia = hallar_pos_vacia(hash->tabla, hash->tam, clave_hasheada) ;
		
		if(pos_vacia == -1)
			return false ;
		
		hash->tabla[pos_vacia].clave = malloc(sizeof(char) * strlen(clave)+1) ;
		if(!hash->tabla[pos_vacia].clave)
			return false ;
		strcpy(hash->tabla[pos_vacia].clave,clave);
		hash->tabla[pos_vacia].valor = dato ;
		hash->tabla[pos_vacia].estado_campo = OCUPADO ;
		hash->cant ++ ;
	}
	
	else{
		if(hash->destruir_dato)
			hash->destruir_dato(hash->tabla[pos_clave].valor) ;
		hash->tabla[pos_clave].valor = dato ;
	}

	return true ;
}

void *hash_borrar(hash_t *hash, const char *clave){
	int pos = posicion_clave(hash, clave) ;
	if(pos == -1)
		return NULL ;

	void* dato = hash->tabla[pos].valor ;
	hash->tabla[pos].estado_campo = BORRADO ;
	hash->cant -- ;

	//AGREGAR REDIMENSION
	
	return dato ;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	int pos = posicion_clave(hash, clave) ;
	if(pos == -1)
		return NULL ;

	void* dato = hash->tabla[pos].valor ;

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


/* ************************************************************
 *                         ITERADOR
 * ***********************************************************/ 

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if (!iter){
		return NULL;
	}
	
	iter->act = &hash->tabla[hallar_pos_ocupada(hash, 0)];
	*iter->hash = *hash;
	return iter;
}


bool hash_iter_avanzar(hash_iter_t *iter){
	if (hash_esta_vacio(iter->hash) || hash_iter_al_final(iter)){
		return false;
	}
	
	int pos = posicion_clave(iter->hash, iter->act->clave);
	iter->act = &iter->hash->tabla[hallar_pos_ocupada(iter->hash, pos+1)];
	return true;
}


const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if (iter->act){
		return iter->act->clave;
	}
	return NULL;
}


bool hash_iter_al_final(const hash_iter_t *iter){
	int pos = posicion_clave(iter->hash, iter->act->clave);
	return hallar_pos_ocupada(iter->hash, pos+1) == -1;
}


void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}

