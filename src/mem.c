//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------

#include "mem.h"
#include "mem_space.h"
#include "mem_os.h"
#include <assert.h>

// the very beginning of the Data to be defined later
mem_allocator_t gbl_allocator = NULL;

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
    mem_block_t first_free = (mem_block_t) (gbl_allocator + GBL_ALLOC_SIZE);

    //Calcul size_data du premier bloc
    size_t size_data_first_free = mem_space_get_size() - BLOCK_SIZE - GBL_ALLOC_SIZE;

    first_free->size_data = size_data_first_free;
    first_free->next = NULL;
    first_free->free = 1;

    gbl_allocator->first = first_free;

    //Initialisation de la fonction de recherche par défault
    mem_set_fit_handler(mem_first_fit);

    printf("%lu\n", (unsigned long) gbl_allocator);
    printf("%lu\n", (unsigned long) first_free);
    printf("%zu\n", size_data_first_free);
}

void split_block(mem_block_t block, size_t size) {
    mem_block_t new_block;
    new_block = (mem_block_t) block->data + size;
    new_block->size_data = block->size_data - size - BLOCK_SIZE;
    new_block->next = block->next;
    new_block->free = 1;
    block->size_data = size;
    block->next = new_block;
}

//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
/**
 * Allocate a bloc of the given size.
**/
void *mem_alloc(size_t size) {
    mem_block_t b, first_free = NULL;
    size_t s = align4(size);

    b = gbl_allocator->fit_function(first_free, s);
    if (b) {
        if (b->size_data - s >= BLOCK_SIZE) {
            split_block(b, s);
        }
        b->free = 0;
    }

    return b->data;
}

//-------------------------------------------------------------
// mem_get_size
//-------------------------------------------------------------
size_t mem_get_size(void *zone) {
    //TODO: implement
    assert(!"NOT IMPLEMENTED !");
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
    assert(!"NOT IMPLEMENTED !");
}

//-------------------------------------------------------------
// Itérateur(parcours) sur le contenu de l'allocateur
// mem_show
//-------------------------------------------------------------}
void mem_show(void (*print)(void *, size_t, int free)) {
    mem_block_t head = gbl_allocator->first;
    while (head) {
        print(head, head->size_data, head->free);
        head = (mem_block_t) head->next;
    }
}

//-------------------------------------------------------------
// mem_fit
//-------------------------------------------------------------
void mem_set_fit_handler(mem_fit_function_t *mff) {
    gbl_allocator->fit_function = mff;
}

//-------------------------------------------------------------
// Stratégies d'allocation
//-------------------------------------------------------------
mem_block_t mem_first_fit(mem_block_t first_free_block, size_t wanted_size) {

    mem_block_t b = (mem_block_t) gbl_allocator->first;
    while (b && !(b->free && b->size_data >= wanted_size)) {
        first_free_block = b;
        b = b->next;
    }

    return b;
}

//-------------------------------------------------------------
mem_block_t mem_best_fit(mem_block_t first_free_block, size_t wanted_size) {
    //TODO: implement
    assert(!"NOT IMPLEMENTED !");
    return NULL;
}

//-------------------------------------------------------------
mem_block_t mem_worst_fit(mem_block_t first_free_block, size_t wanted_size) {
    //TODO: implement
    assert(!"NOT IMPLEMENTED !");
    return NULL;
}
