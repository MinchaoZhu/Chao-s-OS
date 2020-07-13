#ifndef KLIB_MAP_H_
#define KLIB_MAP_H_
/**
 * hash table
 * uint32_t -> uint32_t
 * */

#include <stdint.h>
#include "kernel/kmm.h"

#define MAP_EMPTY 0
#define MAP_OCCUPIED 1
#define MAP_DELETED 2


#define LOAD_FACTOR_DIVIDEND_MAX 3 // 3/4
#define LOAD_FACTOR_DIVISOR_MAX 4  // 3/4

#define LOAD_FACTOR_DIVIDEND_MIN 1 // 1/4
#define LOAD_FACTOR_DIVISOR_MIN 4  // 1/4

typedef struct map_node {
    uint32_t key;
    uint32_t value;
    uint32_t status;
} map_node_t;

typedef struct map {
    uint32_t capacity;
    uint32_t capacity_level;
    uint32_t size;
    map_node_t *list;
} map_t;

// get a hash map with capacity of 2^bits
// return a new map pointer
map_t* map_init();

// insert a (key, value) pair
void map_insert(uint32_t key, uint32_t value, map_t *map);

void map_delete(uint32_t key, map_t *map);

/**
 * method to find key
 * if exists, *value = value, return 1
 * else       *value unchanged, return 0
 * */
int map_find(uint32_t key, uint32_t *value, map_t *map);

#endif


