#include <stdbool.h>
#include <stdlib.h>


/* ************************************************************
 *              DEFINICION DE LOS TIPOS DE DATO
 * ***********************************************************/ 

typedef struct hash hash_t;
typedef struct hash_iter hash_iter_t;
typedef void (*hash_destruir_dato_t)(void *);

/* ************************************************************
 *                     PRIMITIVAS HASH
 * ***********************************************************/ 

//Crea un hash. Devuelve NULL en caso de error.
//Devuelve un hash vacio.
hash_t *hash_crear(hash_destruir_dato_t destruir_dato);

//Guarda un dato y su clave en el hash. Devuelve falso en caso
// de error.
//Pre: el hash fue creado.
//Post: el elemento fue agregado.
bool hash_guardar(hash_t *hash, const char *clave, void *dato);

//Borra un elemento del hash segun la clave. De no existir, devuelve NULL.
//Pre: el hash fue creado.
//Post: Devuelve el valor correspondiente a la clave.
void *hash_borrar(hash_t *hash, const char *clave);

//Devuelve el valor asignado a la clave pasada por parametro, si
//pertenece, o NULL, de lo contrario.
//Pre: el hash fue creado.
void *hash_obtener(const hash_t *hash, const char *clave);

//Devuelve verdadero si la clave pertenece al hash y falso de lo contrario.
//Pre: el hash fue creado.
bool hash_pertenece(const hash_t *hash, const char *clave);

//Devuelve el la cantidad de elementos del hash.
//Pre: el hash fue creado.
size_t hash_cantidad(const hash_t *hash);

//Destruye el hash.
//Pre: el hash fue creado.
//Post: el hash y sus elementos fueron detruidos.
void hash_destruir(hash_t *hash);

/* ************************************************************
 *                    PRIMITIVAS ITERADOR
 * ***********************************************************/ 

//Crea un iterador para hash. Devuelve NULL en caso de error.
//Pre: el hash fue creado
//Post: el iterador fue creado.
hash_iter_t *hash_iter_crear(const hash_t *hash);

//Avanza un elemento del hash. Devuelve false en caso de error.
//Pre: el iterador fue creado.
bool hash_iter_avanzar(hash_iter_t *iter);

//Devuelve el valor actual en el hash.
//Pre: el iterador fue creado.
const char *hash_iter_ver_actual(const hash_iter_t *iter);

//Devuelve verdadero si el iterador esta al final del hash o falso
//de lo contrario.
//Pre:el iterador fue creado.
bool hash_iter_al_final(const hash_iter_t *iter);

//Destruye el iterador.
//Pre: el iterador fue creado.
//Post: el iterador fue destruido.
void hash_iter_destruir(hash_iter_t* iter);
