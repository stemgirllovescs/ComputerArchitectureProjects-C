#include "umalloc.h"
#include "csbrk.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "err_handler.h"
#include "ansicolors.h"

const char author[] = ANSI_BOLD ANSI_COLOR_RED "Katherine Liang xl5878" ANSI_RESET;


/*
 * The following helpers can be used to interact with the memory_block_t
 * struct, they can be adjusted as necessary.
 */

// A sample pointer to the start of the free list.
memory_block_t *free_head;
unsigned long sumOfFreeBlocks;
static size_t heapSize = 0;


/*
 * is_allocated - returns true if a block is marked as allocated.
 */
bool is_allocated(memory_block_t *block) {
    assert(block != NULL);
    return block->block_size_alloc & 0x1;
}

/*
 * allocate - marks a block as allocated.
 */
void allocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_size_alloc |= 0x1;
}


/*
 * deallocate - marks a block as unallocated.
 */
void deallocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_size_alloc &= ~0x1;
}

/*
 * get_size - gets the size of the block.
 */
size_t get_size(memory_block_t *block) {
    assert(block != NULL);
    return block->block_size_alloc & ~(ALIGNMENT-1);
}

/*
 * get_next - gets the next block.
 */
memory_block_t *get_next(memory_block_t *block) {
    assert(block != NULL);
    return block->next;
}

/*
 * put_block - puts a block struct into memory at the specified address.
 * Initializes the size and allocated fields, along with NUlling out the next 
 * field.
 */
void put_block(memory_block_t *block, size_t size, bool alloc) {
    assert(block != NULL);
    assert(size % ALIGNMENT == 0);
    assert(alloc >> 1 == 0);
    block->block_size_alloc = size | alloc;
    block->next = NULL;
}

/*
 * get_payload - gets the payload of the block.
 */
void *get_payload(memory_block_t *block) {
    assert(block != NULL);
    return (void*)(block + 1);
}

/*
 * get_block - given a payload, returns the block.
 */
memory_block_t *get_block(void *payload) {
    assert(payload != NULL);
    return ((memory_block_t *)payload) - 1;
}

/*
 * The following are helper functions that can be implemented to assist in your
 * design, but they are not required. 
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 */
memory_block_t *find(size_t size) {
    //* STUDENT TODO
    // As soon as we find a block that can be used, we will return it
    // set result to our free_head
    memory_block_t *result = free_head;
    // As long as result is not null
    while (result != NULL){
        // if the size of result is greater than or equal to size
        if (size <= get_size(result)){
            return result;
        }
        // Otherwise, just go to the next block
        result = result->next;    
    }
    return result;
}

/*
 * extend - extends the heap if more memory is required.
 */
memory_block_t *extend(size_t size) {
    // Make sure the alignment is correct
    size = ALIGN(size);
    // We are requesting more memory that fits the size and correct alignment
    // Now we have a new free block
    memory_block_t *result = (memory_block_t *) csbrk(ALIGNMENT + size);
    assert(result != NULL);
    // Set size of the result block equal to size
    result->block_size_alloc = size;
    // increment heapSize by size
    heapSize = heapSize + size;
    return result;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    //* STUDENT TODO
    // Check if the block is null
    assert(block != NULL);
    // Get size of the resulting block from split
    size_t sizeOfBlock = get_size(block) - size;
    // Check if it's in multiples of 16
    block->block_size_alloc = ALIGN(block->block_size_alloc);
    // We make a current block that's the size o
    // because block is in memory_block_t and size is in bytes
    // so block + 1 will be block + sizeOf(memory_block_t)
    // And that's incorrect because we want block + 1, so cast it
    memory_block_t *currBlock = (memory_block_t *)((char *) block + size);
    // If the size of block doesn't align
    if(sizeOfBlock % ALIGNMENT != 0){
        // we want to get rid off the extra bit
        sizeOfBlock = sizeOfBlock - (sizeOfBlock % ALIGNMENT);
    }
    // we put the current block there with the size of the block and it's not 
    // marked as allocated
    // current block is not allocated, but the old block is 
    put_block(currBlock, sizeOfBlock, false);
    // Replace the block
    assert(is_allocated(block));
    assert(!is_allocated(currBlock));
    // If there is only one block, then set the free_head as our current block
    if (sumOfFreeBlocks == 1){
        free_head = currBlock;
    } else {
        // Otherwise, go through the free list
        memory_block_t *tempPrev = free_head;
        memory_block_t *tempCurrent = free_head;
        // update the value of the current block
        while (tempCurrent!= NULL && tempCurrent != block){
            tempPrev = tempCurrent;
            tempCurrent = tempCurrent->next;
        
            if (tempCurrent == free_head){
                free_head = currBlock;
                currBlock = block->next;
            } else {
                tempPrev->next = currBlock;
                currBlock->next = tempCurrent->next;
            }
        }
        
    }
    // checking of the allocated block is correct
    assert(get_size(block) == size - ALIGNMENT);
    // check if the address of the free head is correct
    assert(free_head->next > free_head || free_head->next == NULL);

    return currBlock; 
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
memory_block_t *coalesce(memory_block_t *block) {
    //* STUDENT TODO
    assert(block != NULL);

    // Find the free block before the block in the free list
    memory_block_t *prev = free_head;
    memory_block_t *current = free_head;
    while (current != NULL && current != block){
        prev = current;
        current = current->next;
    }
    
    // Check whether the block after current in the list is neighboring
    size_t currSize;
    if ((memory_block_t *) ((char *) current + get_size(current) + ALIGNMENT) == current->next) {
        // If it is, add its size to the current block's size
        currSize = get_size(current) + ALIGNMENT + get_size(current->next);

        // Make sure the size is aligned
        if (currSize % ALIGNMENT == 0) {
            current->block_size_alloc = currSize | false;
            current->next = current->next->next;
            sumOfFreeBlocks--;
        } else {
            currSize -= currSize % ALIGNMENT;
        }
        
    }

    // Check if the block before current is neighboring
    if ((memory_block_t *) ((char*) prev + get_size(prev) + ALIGNMENT) == current) {
        // If it is, add the size to the current block's size
        currSize = get_size(prev) + ALIGNMENT + get_size(current);

        // Make sure the size is aligned
        if (currSize % ALIGNMENT == 0) {
            prev->block_size_alloc = currSize | false;
            prev->next = current->next;
            sumOfFreeBlocks--;
        } else {
            current = current - (currSize % ALIGNMENT);
        }
        
    }

    return current;
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    // Getting the initial block to set free head equal to
    // get_block, gets a pointer and returns it
    // csbrk it takes a chunk of memory
    memory_block_t *result = get_block(csbrk(PAGESIZE));
    // If csbrk fails to find any memory to allocate
    if (result == NULL){
        return -1;
    }
    // Otherwise, we found memory to allocate
    free_head = result;
    // set free head's size as page size
    free_head->block_size_alloc = PAGESIZE;
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size) {
    //* STUDENT TODO
    // Align the size to be malloc'd
    size = ALIGN(size);
    
    // Find the block to allocate
    memory_block_t *result = find(size);
    if (result == NULL){
        // If there's no block that fits, extend the free list
        result = extend(size);
    } else {
        // If there's only 1 block in the free list, return it
        if (sumOfFreeBlocks == 1) {
            return get_payload(result);
        }

        // Update the previous free block's next pointer to point at the block
        // we found
        memory_block_t *prev = free_head;

        if (prev == result) {
            // If the block we want to allocate is the head, update the head
            // to be the next free block in the list
            free_head = free_head->next;
        } else {
            // Otherwise, iterate throughout the free list until we're at the
            // previous free block
            while (prev != NULL && prev->next != result) {
                prev = prev->next;
            }
            // Update the previous block's next pointer
            prev->next = result->next;
        }
    }    
    sumOfFreeBlocks--;
    
    // Allocate the block we found
    allocate(result);
    
    // Return the block's payload
    return get_payload(result); 
}

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 */
void ufree(void *ptr) {
    //* STUDENT TODO
    memory_block_t *result= get_block(ptr);
    // If the memory block is marked as allocated
    if (is_allocated(result)) {
        // Now we want to deallocate
        deallocate(result);
        sumOfFreeBlocks++;

        if (result < free_head || free_head == NULL) {
            memory_block_t *tempBlock = free_head;
            free_head = result;
            result->next = tempBlock;
        } else {
            memory_block_t *current = free_head;
            memory_block_t *trailer = NULL;
            while (current != NULL && result > current) {
                trailer = current;
                current = current->next;
            }
            result->next = current;
            trailer->next = result;
        }
         
    }  
}