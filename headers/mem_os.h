//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------

#ifndef MEM_OS_H
#define MEM_OS_H

#define align4(x) (((((x)-1)>>2)<<2)+4)
#define BLOCK_FREE_SIZE sizeof(struct mem_free_block_s)
#define BLOCK__SIZE sizeof(struct mem_free_block_s)
#define GBL_ALLOC_SIZE sizeof(struct mem_allocator_s)

//include stdlib pour definition du type size_t
#include <stdlib.h>

typedef struct mem_free_block_s mem_free_block_t;
typedef struct mem_allocated_block_s mem_allocated_block_t;


struct mem_free_block_s{
    size_t size_total;
    mem_free_block_t* next;
};

struct mem_allocated_block_s {
    size_t size_total;
};


typedef struct mem_allocator_s* mem_allocator_t;

typedef mem_free_block_t* (mem_fit_function_t)(mem_free_block_t*, size_t);

struct mem_allocator_s {
    mem_free_block_t *first_free_block;

    // i.e. function for searching appropriate block
    mem_fit_function_t *fit_function;
};
/* -----------------------------------------------*/
/* Interface de gestion de votre allocateur       */
/* -----------------------------------------------*/
// Initialisation
void mem_init(void);

// Définition du type mem_fit_function_t
// type des fonctions d'allocation

// Choix de la fonction d'allocation
// = choix de la stratégie de l'allocation
void mem_set_fit_handler(mem_fit_function_t *);

// Stratégies de base (fonctions) d'allocation
mem_fit_function_t mem_first_fit;
mem_fit_function_t mem_worst_fit;
mem_fit_function_t mem_best_fit;

#endif /* MEM_OS_H */
