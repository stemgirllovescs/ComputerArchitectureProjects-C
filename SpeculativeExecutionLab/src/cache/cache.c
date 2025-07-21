/**************************************************************************
  * C S 429 system emulator
 * 
 * cache.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions, both dirty and clean.  The replacement policy is LRU. 
 *     The cache is a writeback cache. 
 * 
 * Copyright (c) 2021, 2023. 
 * Authors: M. Hinton, Z. Leeper.
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "cache.h"

#define ADDRESS_LENGTH 64

/* Counters used to record cache statistics in printSummary().
   test-cache uses these numbers to verify correctness of the cache. */

//Increment when a miss occurs
int miss_count = 0;

//Increment when a hit occurs
int hit_count = 0;

//Increment when a dirty eviction occurs
int dirty_eviction_count = 0;

//Increment when a clean eviction occurs
int clean_eviction_count = 0;

/* STUDENT TO-DO: add more globals, structs, macros if necessary */
uword_t next_lru;

// log base 2 of a number.
// Useful for getting certain cache parameters
static size_t _log(size_t x) {
  size_t result = 0;
  while(x>>=1)  {
    result++;
  }
  return result;
}

/*
 * Initialize the cache according to specified arguments
 * Called by cache-runner so do not modify the function signature
 *
 * The code provided here shows you how to initialize a cache structure
 * defined above. It's not complete and feel free to modify/add code.
 */
cache_t *create_cache(int A_in, int B_in, int C_in, int d_in) {
    /* see cache-runner for the meaning of each argument */
    cache_t *cache = malloc(sizeof(cache_t));
    cache->A = A_in;
    cache->B = B_in;
    cache->C = C_in;
    cache->d = d_in;
    unsigned int S = cache->C / (cache->A * cache->B);

    cache->sets = (cache_set_t*) calloc(S, sizeof(cache_set_t));
    for (unsigned int i = 0; i < S; i++){
        cache->sets[i].lines = (cache_line_t*) calloc(cache->A, sizeof(cache_line_t));
        for (unsigned int j = 0; j < cache->A; j++){
            cache->sets[i].lines[j].valid = 0;
            cache->sets[i].lines[j].tag   = 0;
            cache->sets[i].lines[j].lru   = 0;
            cache->sets[i].lines[j].dirty = 0;
            cache->sets[i].lines[j].data  = calloc(cache->B, sizeof(byte_t));
        }
    }

    next_lru = 0;
    return cache;
}

cache_t *create_checkpoint(cache_t *cache) {
    unsigned int S = (unsigned int) cache->C / (cache->A * cache->B);
    cache_t *copy_cache = malloc(sizeof(cache_t));
    memcpy(copy_cache, cache, sizeof(cache_t));
    copy_cache->sets = (cache_set_t*) calloc(S, sizeof(cache_set_t));
    for (unsigned int i = 0; i < S; i++) {
        copy_cache->sets[i].lines = (cache_line_t*) calloc(cache->A, sizeof(cache_line_t));
        for (unsigned int j = 0; j < cache->A; j++) {
            memcpy(&copy_cache->sets[i].lines[j], &cache->sets[i].lines[j], sizeof(cache_line_t));
            copy_cache->sets[i].lines[j].data = calloc(cache->B, sizeof(byte_t));
            memcpy(copy_cache->sets[i].lines[j].data, cache->sets[i].lines[j].data, sizeof(byte_t));
        }
    }
    
    return copy_cache;
}

void display_set(cache_t *cache, unsigned int set_index) {
    unsigned int S = (unsigned int) cache->C / (cache->A * cache->B);
    if (set_index < S) {
        cache_set_t *set = &cache->sets[set_index];
        for (unsigned int i = 0; i < cache->A; i++) {
            printf ("Valid: %d Tag: %llx Lru: %lld Dirty: %d\n", set->lines[i].valid, 
                set->lines[i].tag, set->lines[i].lru, set->lines[i].dirty);
        }
    } else {
        printf ("Invalid Set %d. 0 <= Set < %d\n", set_index, S);
    }
}

/*
 * Free allocated memory. Feel free to modify it
 */
void free_cache(cache_t *cache) {
    unsigned int S = (unsigned int) cache->C / (cache->A * cache->B);
    for (unsigned int i = 0; i < S; i++){
        for (unsigned int j = 0; j < cache->A; j++) {
            free(cache->sets[i].lines[j].data);
        }
        free(cache->sets[i].lines);
    }
    free(cache->sets);
    free(cache);
}

/* STUDENT TO-DO:
 * Get the line for address contained in the cache
 * On hit, return the cache line holding the address
 * On miss, returns NULL
 */
cache_line_t *get_line(cache_t *cache, uword_t addr) {
    //given a long long address, get the line associated with it
    //iterate through each cache set.
    //each set has A elements
    //what is the index for the set?

    //set index: from [b to b + s - 1]
    //the length of this is s
    unsigned int b = _log(cache->B); //little b
    unsigned int s = _log(cache->C / (cache->A * cache->B)); // get little s
    //from formula of S = C/BA
    //mask out 2 ^ s - 1 = (1 << s) - 1
    unsigned int setIndex = (addr >> b) & ((1 << s) - 1);
    //lets get the tag from addr. the bits starting from b + s.
    unsigned int tag = addr >> (b + s);


    //iterate through the cache lines in the set
    //there are A number of lines in the set
    for(unsigned int i = 0; i < cache->A; i++) {
        if(cache->sets[setIndex].lines[i].tag == tag && cache->sets[setIndex].lines[i].valid == 1) {
            return &cache->sets[setIndex].lines[i];
        }
    }
    return NULL;
}

/* STUDENT TO-DO:
 * Select the line to fill with the new cache line
 * Return the cache line selected to filled in by addr
 */
cache_line_t *select_line(cache_t *cache, uword_t addr) {
    /* your implementation */

    //this is called on 

    //first of all, its gonna be in the set index no matter what:
    unsigned int b = _log(cache->B); //little b
    unsigned int s = _log(cache->C / (cache->A * cache->B)); // get little s
    unsigned int setIndex = (addr >> b) & ((1 << s) - 1);
    //best line variable:
    cache_line_t* bestLine;

    for(unsigned int i = 0; i < cache->A; i++) {
        //also check valid
        if(i == 0 || cache->sets[setIndex].lines[i].lru < bestLine->lru) {
            bestLine = &cache->sets[setIndex].lines[i];
        }
        if(cache->sets[setIndex].lines[i].valid == 0) {
            return &cache->sets[setIndex].lines[i];
        } 
    }
    return bestLine;
}

/*  STUDENT TO-DO:
 *  Check if the address is hit in the cache, updating hit and miss data.
 *  Return true if pos hits in the cache.
 */
bool check_hit(cache_t *cache, uword_t addr, operation_t operation) {
    /* your implementation */
    //operation read and operation write
    //cache = 0;
    cache_line_t* line = get_line(cache, addr);
    if(line != NULL) {
        if(operation == WRITE) {
            line->dirty = 1;
        }
        hit_count++;
        next_lru++;
        line->lru = next_lru;
        return true;
    } else {
        miss_count++;
        return false;
    }
}

/*  STUDENT TO-DO:
 *  Handles Misses, evicting from the cache if necessary.
 *  Fill out the evicted_line_t struct with info regarding the evicted line.
 */
evicted_line_t *handle_miss(cache_t *cache, uword_t addr, operation_t operation, byte_t *incoming_data) {
    evicted_line_t *evicted_line = malloc(sizeof(evicted_line_t));
    evicted_line->data = (byte_t *) calloc(cache->B, sizeof(byte_t));
    unsigned int b = _log(cache->B); //little b
    unsigned int s = _log(cache->C / (cache->A * cache->B)); 
    unsigned int tag = addr >> (b + s);



        //case 1 cache miss, no replacement
    cache_line_t* sel_line = select_line(cache, addr);
    //get set index from sel _line
    if(sel_line->valid == 1) {
        //this is an eviction
        memcpy(evicted_line->data, sel_line->data, cache->B);
        //evicted_line->data = sel_line->data;
        uword_t sel_tag = sel_line->tag;
        evicted_line->addr = sel_tag << (b + s);
        evicted_line->addr |= (addr & ( (1 << (b + s) ) - 1));
        evicted_line->addr &= ~( (1 << b) - 1);
        
        evicted_line->dirty = sel_line->dirty;
        evicted_line->valid = sel_line->valid;

        if(sel_line->dirty) {
            dirty_eviction_count++;
        } else {
            clean_eviction_count++;
        }
    } 
    

    sel_line->tag = tag;
    sel_line->valid = 1;

    if(incoming_data != NULL) {
        memcpy(sel_line->data, incoming_data, cache->B);
    } 

    if(operation == WRITE) {
        sel_line->dirty = true;
    } else {
        sel_line->dirty = false;
    }
    
    sel_line->lru = next_lru;
    
    return evicted_line;
}

/* STUDENT TO-DO:
 * Get 8 bytes from the cache and write it to dest.
 * Preconditon: addr is contained within the cache.
 */
void get_word_cache(cache_t *cache, uword_t addr, word_t *dest) {
    /* Your implementation */
    //first lets get the cache line:


    cache_line_t* line = get_line(cache, addr);
    //offset is contained within bits 0 to b - 1
    unsigned long b = _log(cache->B);
    unsigned long blockOffset = addr & ( (1 << b) - 1);
    byte_t* bytePointer = line->data;
    bytePointer += blockOffset;
    //now at the right offset, make this get 8 bytes instead of 1
    memcpy(dest, bytePointer, 8);

    
    // long* longPointer = (long*) bytePointer;
    // *dest = *longPointer;
}

/* STUDENT TO-DO:
 * Set 8 bytes in the cache to val at pos.
 * Preconditon: addr is contained within the cache.
 */
void set_word_cache(cache_t *cache, uword_t addr, word_t val) {
    /* Your implementation */
    //first get the cache line
    cache_line_t* line = get_line(cache, addr);
    //offset is contained within bits 0 to b - 1
    unsigned long b = _log(cache->B);
    unsigned long blockOffset = addr & ( (1 << b) - 1);
    byte_t* bytePointer = line->data;
    bytePointer += blockOffset;
    memcpy(bytePointer, &val, 8);
    // long* longPointer = (long*) bytePointer;
    // *longPointer = val;
}

/*
 * Access data at memory address addr
 * If it is already in cache, increase hit_count
 * If it is not in cache, bring it in cache, increase miss count
 * Also increase eviction_count if a line is evicted
 *
 * Called by cache-runner; no need to modify it if you implement
 * check_hit() and handle_miss()
 */
void access_data(cache_t *cache, uword_t addr, operation_t operation)
{
    if(!check_hit(cache, addr, operation))
        free(handle_miss(cache, addr, operation, NULL));
}
