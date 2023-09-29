//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------

#include "mem.h"
#include "mem_space.h"
#include "mem_os.h"
#include <assert.h>

struct mem_allocator_s * gbl_allocator = NULL;
//-------------------------------------------------------------
// mem_init
//-------------------------------------------------------------
/**
 * Initialize the memory allocator.
 * If already init it will re-init.
**/
void mem_init() {
	// ou on place l'info alloc
	gbl_allocator = mem_space_get_addr();

	//Entete premier libre
	mem_free_block_s* first_free = mem_space_get_addr() + 1;
	// Calcul size_data du premier bloc
	int size_data_first_free = mem_space_get_size() - sizeof(mem_free_block_s*) - sizeof(mem_free_block_s*) + sizeof(size_t);

	first_free->size_data = size_data_first_free;
	first_free->next = NULL;

	gbl_allocator->first_libre = first_free;
    gbl_allocator->first_occupied = NULL;

	printf("%p\n",gbl_allocator);
	printf("%p\n",first_free);
	printf("%d\n",size_data_first_free);

	
}

//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
/**
 * Allocate a bloc of the given size.
**/
void *mem_alloc(size_t size) {
	//TODO: implement
	assert(! "NOT IMPLEMENTED !");
    return NULL;
}

//-------------------------------------------------------------
// mem_get_size
//-------------------------------------------------------------
size_t mem_get_size(void * zone)
{
    //TODO: implement
	assert(! "NOT IMPLEMENTED !");
    return 0;
}

//-------------------------------------------------------------
// mem_free
//-------------------------------------------------------------
/**
 * Free an allocaetd bloc.
**/
void mem_free(void *zone) {
    //TODO: implement
	assert(! "NOT IMPLEMENTED !");
}

//-------------------------------------------------------------
// Itérateur(parcours) sur le contenu de l'allocateur
// mem_show
//-------------------------------------------------------------}
void mem_show(void (*print)(void *, size_t, int free)) {

    mem_free_block_s* head_libre = gbl_allocator->first_libre;
    mem_block_occupied *head_occupied = gbl_allocator->first_occupied;

    while(head_libre || head_occupied) {
        if (head_occupied && head_libre && (int*) head_libre > (int*) head_occupied) {
            print(head_occupied, head_occupied->size_data, 0);
            head_occupied = (mem_block_occupied *) head_occupied->next;
        } else if (head_libre) {
            print(head_libre, head_libre->size_data, 1);
            head_libre = (mem_free_block_s *) head_libre->next;
        }
    }
}

//-------------------------------------------------------------
// mem_fit
//-------------------------------------------------------------
void mem_set_fit_handler(mem_fit_function_t *mff) {
	//TODO: implement
	assert(! "NOT IMPLEMENTED !");
}

//-------------------------------------------------------------
// Stratégies d'allocation
//-------------------------------------------------------------
mem_free_block_t *mem_first_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    //TODO: implement
	assert(! "NOT IMPLEMENTED !");
	return NULL;
}
//-------------------------------------------------------------
mem_free_block_t *mem_best_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    //TODO: implement
	assert(! "NOT IMPLEMENTED !");
	return NULL;
}

//-------------------------------------------------------------
mem_free_block_t *mem_worst_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    //TODO: implement
	assert(! "NOT IMPLEMENTED !");
	return NULL;
}
