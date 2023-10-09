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
    mem_free_block_t *first_free = (mem_free_block_t *) (gbl_allocator + 1);

    //Calcul size_data du premier bloc
    size_t size_data_first_free = mem_space_get_size() - BLOCK_FREE_SIZE - GBL_ALLOC_SIZE;

    first_free->size_total = size_data_first_free;
    first_free->next = NULL;

    gbl_allocator->first_free_block = first_free;

    //Initialisation de la fonction de recherche par défault
    mem_set_fit_handler(mem_first_fit);

    printf("%lu\n", (unsigned long) gbl_allocator);
    printf("%lu\n", (unsigned long) first_free);
    printf("%zu\n", size_data_first_free);
}
//
//void split_block(mem_block_t block, size_t size) {
//    mem_block_t new_block;
//    new_block = (mem_block_t) block->data + block->size_data;
//    new_block->size_data = block->size_data - size - BLOCK_SIZE;
//    new_block->next = block->next;
//    new_block->free = 1;
//    block->size_data = size;
//    block->next = new_block;
//}

mem_free_block_t* get_previous_free_block(mem_free_block_t *first_free_block, mem_free_block_t *target) {
    mem_free_block_t *current_block = first_free_block;
    while (current_block != NULL) {
        if (current_block->next == target) {
            return current_block;
        }
        current_block = current_block->next;
    }
    return NULL;
}

//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
/**
 * Allocate a bloc of the given size.
**/
void *mem_alloc(size_t size) {
    //mem_free_block_t *first_free = gbl_allocator->first_free_block;
    mem_allocated_block_t *new_block = NULL;
    /*
    mem_free_block_t *chosen_block = NULL;
    mem_free_block_t *next_free_block = NULL;
    mem_free_block_t *previous_free_block = NULL;
    mem_free_block_t *new_free_block = NULL;

    size_t free_block_size;
    //mem_fit_function_t* fit_function;
    size_t s = align4(size);
    if (first_free != NULL) {
        //fit_function = gbl_allocator->fit_function;
        //TODO cas par cas des fit fonctions
        chosen_block = mem_first_fit(first_free,s);
        next_free_block = chosen_block->next;
        free_block_size = chosen_block->size_total;
        previous_free_block = get_previous_free_block(first_free,chosen_block);
        new_free_block->next = next_free_block;
        new_free_block->size_total = free_block_size - s;
        previous_free_block->next = new_free_block;

        new_block = (mem_allocated_block_t *) chosen_block;
        new_block->size_total = size + BLOCK_ALLOCATED_SIZE;

    }
*/
    return new_block;
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

int mem_is_in_free(void *block) {
    mem_free_block_t *head = gbl_allocator->first_free_block;
    while (head) {
        if (head == block) {
            return 1;
        }
        head = head->next;
    }
    return 0;
}

//-------------------------------------------------------------
// Itérateur(parcours) sur le contenu de l'allocateur
// mem_show
//-------------------------------------------------------------}
void mem_show(void (*print)(void *, size_t, int free)) {
    void *head = gbl_allocator->first_free_block;
//    void *old_head = NULL;

    int libre = 1;
    int prise = 0;

    while ((head + BLOCK_FREE_SIZE  < mem_space_get_addr() + mem_space_get_size())) {
        if (mem_is_in_free(head)) {
            print(head, ((mem_free_block_t *) head)->size_total, libre);
            head += ((mem_free_block_t *) head)->size_total;
        } else {
            print(head, ((mem_allocated_block_t *) head)->size_total, prise);
            head += ((mem_allocated_block_t *) head)->size_total;
        }
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
mem_free_block_t *mem_first_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    mem_free_block_t* current_free_block = first_free_block;
    while (current_free_block != NULL && !(current_free_block->size_total >= wanted_size)) {
        current_free_block = current_free_block->next;
    }
    return current_free_block;
}

//-------------------------------------------------------------
mem_free_block_t *mem_best_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    mem_free_block_t* current_free_block = first_free_block;
    mem_free_block_t* best_free_block = current_free_block;
    int min = first_free_block->size_total;
    while (current_free_block != NULL) {
        if (current_free_block->size_total >= wanted_size) {
            if (min > current_free_block->size_total) {
                min = current_free_block->size_total;
                best_free_block = current_free_block;
            }
        }
        current_free_block = current_free_block->next;
    }
    return best_free_block;
}

//-------------------------------------------------------------
mem_free_block_t *mem_worst_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    mem_free_block_t* current_free_block = first_free_block;
    mem_free_block_t* worst_free_block = current_free_block;
    int max = first_free_block->size_total;
    while (current_free_block != NULL) {
        if (current_free_block->size_total >= wanted_size) {
            if (max < current_free_block->size_total) {
                max = current_free_block->size_total;
                worst_free_block = current_free_block;
            }
        }
        current_free_block = current_free_block->next;
    }
    return worst_free_block;
}
