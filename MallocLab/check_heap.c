#include <assert.h>
#include "umalloc.h"

//Place any variables needed here from umalloc.c as an extern.
extern memory_block_t *free_head;
extern unsigned long sumOfFreeBlocks;

/*
 * check_heap -  used to check that the heap is still in a consistent state.
 *
 * STUDENT TODO:
 * Required to be completed for checkpoint 1:
 *
 *      - Ensure that the free block list is in the order you expect it to be in.
 *        As an example, if you maintain the free list in memory-address order,
 *        lowest address first, ensure that memory addresses strictly ascend as you
 *        traverse the free list.
 *
 *      - Check if any free memory_blocks overlap with each other. 
 *
 *      - Ensure that each memory_block is aligned. 
 * 
 * Should return 0 if the heap is still consistent, otherwise return a non-zero
 * return code. Asserts are also a useful tool here.
 */
int check_heap() {
    // if we have a free block
    bool isFree = true;
    // count the number of free blocks
    unsigned long countBlocks = 1;
    // check if free head is not null
    assert(free_head != NULL);
    // keep track of the previous block
    memory_block_t *prev = free_head;
    // keep track of the current block
    memory_block_t *current = prev->next;
    // make sure we have the alignment size that's in multiples of 16
    assert(get_size(prev) % ALIGNMENT == 0);
    while (current != NULL){
        // for the current block, make sure we have the alignment size that's 
        // in multiples of 16
        assert(get_size(current) % ALIGNMENT == 0);
        // if our current block or previous block is allocated or used, 
        // then it's not free, otherwise, it is
        if (is_allocated(current) || is_allocated(prev)){
            isFree = false;
        } else {
            countBlocks++;
        }
        // Returns -1 when we don't pass heap check
        if ((memory_block_t*)((char *) prev + get_size(prev) + ALIGNMENT) == current || prev >= current){
            return -1;
        }
        // make sure current block is not the same as the next block
        assert(current != current->next);
        prev = current;
        current = current->next;
    }

    // if we have the block is free and number of free blocks is the same as total blocks
    // 0 indicates we passed heap check
    if (isFree && countBlocks == sumOfFreeBlocks){
        return 0;
    }
    // fails the heap check in the end
    return -1;
}