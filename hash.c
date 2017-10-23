#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAMANIO_INICIAL 10
#define CRITERIO_AUMENTO 0.7
#define CRITERIO_ACHICAR 0.3
#define FACTOR_AUMENTO 2
#define FACTOR_ACHICAR 0.5

typedef enum estado_campo {VACIO,BORRADO,OCUPADO} estado_t ;
typedef struct hash_campo hash_campo_t ;

struct hash_campo{
	char* clave;
	void* valor;
	estado_t estado_campo ;
} ;

struct hash{
	hash_campo_t* tabla;
	size_t tam;
	size_t cant;
	hash_destruir_dato_t destruir_dato;
};

struct hash_iter{
	hash_t* hash;
	size_t indice;
};

/* ************************************************************
 *                     FUNCIONES AUXILIARES
 * ***********************************************************/

//Devuele una posicion segun la clave y el tamanio del hash.
size_t hashing (const char* clave, size_t tam){
	unsigned int num1 = 378551;
	unsigned int num2 = 63689;
	unsigned int clave_numerica = 0;
	unsigned int clave_como_int = *(unsigned int*)clave ;
	for(int i = 0; *clave; clave++, i++){
		clave_numerica = clave_numerica * num2 + clave_como_int;
		num2 = num2 * num1;
	}
	return(clave_numerica%tam);
}
//Devuelve la primera posicion vacia a partir de la posicion que arroja la funcion
//si  no la encuentra devuelve -1
bool hallar_pos_vacia(hash_campo_t* tabla, size_t tam, size_t clave_h, size_t* pos_vacia){
	size_t pos = clave_h ;
	while(pos != clave_h-1){
		if(tabla[pos].estado_campo == VACIO){
	 		*pos_vacia = pos ;
	 		return true ;
		}
	 	//cuando buscó hasta el final de la tabla vuelve a empezar desde el ppio
	 	if(pos != tam-1)
	 		pos ++ ;
	 	else
	 		pos = 0 ;
	 }
	 return false ;
}

//Devuelve verdadero si esta vacio y false de lo contrario.
bool hash_esta_vacio(const hash_t* hash){
	return hash->cant == 0;
}

//Devuelve la siguiente posicion ocupada del hash, o -1 en caso de no haber.
bool hallar_pos_ocupada(const hash_t* hash, size_t* pos_ocupada){
	if (hash_esta_vacio(hash)){
		return false;
	}
	for (size_t pos = 0; pos < hash->tam; pos++){
		if(hash->tabla[pos].estado_campo == OCUPADO){
			*pos_ocupada = pos ;
			return true;
		}
	}
	return false;
}

//Busca en la tabla la clave que le pasan por parametro
//si la encuentra, devuelve la posicion, sino devuelve -1
bool posicion_clave(const hash_t* hash, const char*clave, size_t* pos){
	size_t clave_hash = hashing(clave, hash->tam) ;
	for (size_t actual = clave_hash; hash->tabla[actual].estado_campo == OCUPADO; actual++){
		if (hash->tabla[actual].estado_campo == BORRADO){
			continue;
		}
		if(strcmp(hash->tabla[actual].clave ,clave) == 0){
			*pos = actual ;
			return true ;
		}
	}
	return false ;
}


void destruir_contenido_tabla(hash_campo_t* tabla, size_t tam, hash_destruir_dato_t destruir_dato){
	for(int i = 0 ; i < tam ; i++){
		if(tabla[i].estado_campo == VACIO)
			continue ;
		else{
			free(tabla[i].clave) ;
		}
		if(destruir_dato){
			destruir_dato(tabla[i].valor) ;
		}		
	}
}


//Cambia el tamanio del hash por tam. Devuelve false si hay un error.
//Pre: el hash fue creado.
//Post: el hash tiene un tamanio tam.
bool hash_redimensionar(hash_t *hash, size_t tam){
	hash_campo_t* nueva_tabla = malloc(sizeof(hash_campo_t)* tam );
	if (!nueva_tabla){
		return false;
	}
	for(int i = 0 ; i < tam ; i++)
		nueva_tabla[i].estado_campo = VACIO ;
	
	hash_campo_t* tabla_vieja = hash->tabla;
	size_t tamanio = hash->tam;
	hash->tabla = nueva_tabla;
	hash->tam = tam;
	size_t cant = hash->cant;
	for (size_t pos = 0; pos < tamanio; pos++){
		if (tabla_vieja[pos].estado_campo == OCUPADO){
			hash_guardar(hash, tabla_vieja[pos].clave, tabla_vieja[pos].valor);
			free(tabla_vieja[pos].clave) ;
		}
		if(tabla_vieja[pos].estado_campo == BORRADO)
			free(tabla_vieja[pos].clave) ;
	}
	hash->cant = cant;
	free(tabla_vieja);
	return true;
}

char* strdup(const char* str){
	char* duplicado;
	if(!str)
		return NULL ;
	size_t largo = strlen(str) ;
	duplicado = malloc(sizeof(char) * (largo+1)) ;
	if(!duplicado)
		return NULL ;
	strcpy(duplicado,str) ;
	return duplicado ;
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
	destruir_contenido_tabla(hash->tabla, hash->tam, hash->destruir_dato) ;
	free(hash->tabla) ;
	free(hash);
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){	
	if (!clave){
		return false;
	}	
	size_t pos_clave;
	char* clave_copia = strdup(clave) ; 
	
	if(!posicion_clave(hash, clave_copia, &pos_clave)){
		size_t clave_hasheada = hashing(clave_copia, hash->tam) ;
		size_t pos_vacia ;
		if(!hallar_pos_vacia(hash->tabla, hash->tam, clave_hasheada, &pos_vacia))
			return false ;
		
		hash->tabla[pos_vacia].clave = malloc(sizeof(char) * strlen(clave_copia)+1) ;
		if(!hash->tabla[pos_vacia].clave)
			return false ;
		strcpy(hash->tabla[pos_vacia].clave,clave_copia);
		hash->tabla[pos_vacia].valor = dato ;
		hash->tabla[pos_vacia].estado_campo = OCUPADO ;
		hash->cant++;
	}
	
	else{
		if(hash->destruir_dato)
			hash->destruir_dato(hash->tabla[pos_clave].valor) ;
		hash->tabla[pos_clave].valor = dato ;
	}
	
	if (hash->cant/hash->tam >= CRITERIO_AUMENTO){
		hash_redimensionar(hash, hash->tam * FACTOR_AUMENTO);
	}
	return true ;
}

void *hash_borrar(hash_t *hash, const char *clave){
	if(!clave){
		printf("clave nula en borrar \n") ;
		return NULL;
	}
	size_t pos;
	if(!posicion_clave(hash, clave, &pos)){
		printf("no se encontro posicion clave en borrar \n") ;
		return NULL ;
	}
	//la clave existe en la tabla
	void* dato = hash->tabla[pos].valor ;
	hash->tabla[pos].estado_campo = BORRADO ;
	hash->cant -- ;
	if ((hash->cant < (double)hash->tam * CRITERIO_ACHICAR) && (hash->tam != TAMANIO_INICIAL)){
		size_t nuevo_tam = (size_t)((float)hash->tam * FACTOR_AUMENTO) ;
		if(!hash_redimensionar(hash, nuevo_tam))
			return NULL ;
	}
	return dato ;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	if(!clave){
		return NULL;
	}
	size_t pos ; 
	if(!posicion_clave(hash, clave, &pos))
		return NULL ;
	return hash->tabla[pos].valor ;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	if(!clave){
		return NULL;
	}
	size_t pos ;
	if(!posicion_clave(hash, clave, &pos))
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
	iter->hash = (hash_t*)hash ;
	size_t pos = 0;
	hallar_pos_ocupada(hash,&pos);
		iter->indice = pos;
	return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if (hash_iter_al_final(iter))
		return false;
	iter->indice ++ ;
	while(iter->hash->tabla[iter->indice].estado_campo != OCUPADO && !hash_iter_al_final(iter)){
		iter->indice ++ ;
	}
	if(hash_iter_al_final(iter))
		return false ;
	return true ;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(hash_iter_al_final(iter))
		return NULL ;
	return iter->hash->tabla[iter->indice].clave ;
}


bool hash_iter_al_final(const hash_iter_t *iter){
	if (iter->indice == ((iter->hash->tam)-1) || iter->hash->cant == 0){
		return true;
	}
	return false;
}


void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}

