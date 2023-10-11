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
mem_allocator_ptr gbl_allocator = NULL;

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


mem_free_block_t *mem_split_block(mem_free_block_t *block, size_t size) {
    mem_free_block_t *new_free_block;
    mem_free_block_t *next = block->next;
    new_free_block = (mem_free_block_t *) ((char *) (block) + BLOCK_ALLOCATED_SIZE + size);
    new_free_block->size_total = block->size_total - size - BLOCK_ALLOCATED_SIZE;
    new_free_block->next = next;

    // first (left) part of block (which was free 5 lines ago) now is an allocated block.
    // the rest (right part) of the block will be new free_block
    block->size_total = size + BLOCK_ALLOCATED_SIZE;

    // will be used in free() for verifying
    ((mem_allocated_block_t *) block)->ptr = (char *) (block) + BLOCK_ALLOCATED_SIZE;
    return new_free_block;
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

mem_free_block_t *mem_get_previous_free_block(void *target) {
    mem_free_block_t *current_block = (mem_free_block_t *) (gbl_allocator->first_free_block);
    while (current_block != NULL && (void *) current_block < (void *) target) {
        if (current_block->next && (void *) current_block->next >= target) {
            return current_block;
        } else if (current_block->next == NULL && (void *)current_block < target) {
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
    mem_free_block_t *first_free = gbl_allocator->first_free_block;
    mem_free_block_t *fitting_block = NULL;

    mem_allocated_block_t *allocated_block = NULL;

    size_t aligned_size = align8(size);

    if (!first_free) return NULL;

    fitting_block = gbl_allocator->fit_function(first_free, aligned_size + BLOCK_ALLOCATED_SIZE);
    allocated_block = (mem_allocated_block_t *) fitting_block;

    mem_free_block_t *prev_free = mem_get_previous_free_block(fitting_block);

    if (!fitting_block) return NULL;

    mem_free_block_t *next_free_block = fitting_block->next;

    // can split founded block into smaller
    if (fitting_block->size_total > aligned_size + BLOCK_ALLOCATED_SIZE + BLOCK_FREE_SIZE) {
        next_free_block = mem_split_block(fitting_block, aligned_size);
    } else {
        ((mem_allocated_block_t *) fitting_block)->ptr = (char *) (fitting_block) + BLOCK_ALLOCATED_SIZE;
    }

    if (prev_free) {
        prev_free->next = next_free_block;
    } else {
        gbl_allocator->first_free_block = next_free_block;
    }

    // +BLOCK_ALLOCATED_SIZE for header info of the allocated block (normally 16 bytes)
    return (char *) (allocated_block) + BLOCK_ALLOCATED_SIZE;
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
 * Utility function for returning start of the real
 * block by provided client-level address.
 */
mem_allocated_block_t *mem_get_allocated_block(void *ptr) {
    return ptr - BLOCK_ALLOCATED_SIZE;
}

/**
 * Check whether if ptr points to really allocated block.
 */
int mem_is_valid_address(void *ptr) {
    void *right_boundary = mem_space_get_addr() + mem_space_get_size();
    if (gbl_allocator && ptr > (void *) gbl_allocator && ptr < right_boundary) {
        mem_allocated_block_t *block = mem_get_allocated_block(ptr);
        return block->ptr == ptr;
    }
    return 0;
}

/**
 * Anti-fragmentation strategy:
 * merge neighbours of block being free'd if they free also.
 */

void mem_fusion(void *block) {
    mem_free_block_t *to_free_block = (mem_free_block_t *) block;
    mem_free_block_t *right_block = (mem_free_block_t *) ((char *) (block + to_free_block->size_total));

    if (!mem_is_in_free(right_block)) return;

    to_free_block->size_total += right_block->size_total;
    to_free_block->next = right_block->next;
}


/**
 * Free an allocated bloc.
**/
void mem_free(void *zone) {
    if (!mem_is_valid_address(zone)) return;

    mem_allocated_block_t *zone_block = mem_get_allocated_block(zone);

    mem_free_block_t *prev_free_block = mem_get_previous_free_block(zone_block);
    mem_free_block_t *freed_zone_block = (mem_free_block_t *) zone_block;

    if (prev_free_block != NULL) {
        mem_free_block_t *right_free_block = prev_free_block->next;

        freed_zone_block->next = right_free_block;
        prev_free_block->next = freed_zone_block;
    } else {
        freed_zone_block->next = gbl_allocator->first_free_block;
        gbl_allocator->first_free_block = freed_zone_block;
    }

    mem_fusion(freed_zone_block);

    // merge neighbour left block if is free also
    if (prev_free_block && (char *) prev_free_block + prev_free_block->size_total == (void *) zone_block) {
        mem_fusion(prev_free_block);
    }
}

//-------------------------------------------------------------
// Itérateur(parcours) sur le contenu de l'allocateur
// mem_show
//-------------------------------------------------------------}
void mem_show(void (*print)(void *, size_t, int free)) {
    void *head = gbl_allocator + 1;

    int libre = 1;
    int prise = 0;

    while ((head + BLOCK_FREE_SIZE < mem_space_get_addr() + mem_space_get_size())) {
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
    mem_free_block_t *current_free_block = first_free_block;
    while (current_free_block != NULL && current_free_block->size_total < wanted_size) {
        current_free_block = current_free_block->next;
    }
    return current_free_block;
}

//-------------------------------------------------------------
mem_free_block_t *mem_best_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    mem_free_block_t *current_free_block = first_free_block;
    mem_free_block_t *best_free_block = current_free_block;
    size_t min = first_free_block->size_total;
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
    mem_free_block_t *current_free_block = first_free_block;
    mem_free_block_t *worst_free_block = current_free_block;
    size_t max = first_free_block->size_total;
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
