#include "klib/map.h"
#include "kernel/kmm.h"
#include <string.h>
#include <limits.h>

// largest prime less than 2^index
static const uint32_t primes[33] = {0, 1, 3, 7, 13, 31, 53, 97, \
                                    193, 389, 769, 1543, 3079, 6151, 12289, 24593, \
                                    49157, 98317, 196613, 393241, 786433, 1572869, 3145739, 6291469, \
                                    12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741, \ 
                                    2136745621};

map_t* map_init() {
    map_t* map = (map_t*) kmalloc(sizeof(map_t));
    map->capacity_level = 3;
    map->capacity = primes[3]; // 7
    map->size = 0;
    map_node_t* list = (map_node_t*) kmalloc(sizeof(map_node_t) * map->capacity);
    memset(list, 0, sizeof(map_node_t) * map->capacity);
    map->list = list;
    return map;
}



static inline uint32_t probe(uint32_t i) {
    return i * i;
}

static inline uint32_t hash(uint32_t key, map_t *map) {
    return key % map->capacity;
}



static inline void map_rehash(map_t *map, uint32_t new_capacity_level) {
    uint32_t  new_capacity= primes[new_capacity_level];
    map_node_t* new_list = (map_node_t*) kmalloc(sizeof(map_node_t) * new_capacity);
    memset(new_list, 0, sizeof(map_node_t) * new_capacity);

    map_node_t* old_list = map->list;
    uint32_t old_capacity = map->capacity;
    
    map->list = new_list;
    map->capacity_level = new_capacity_level;
    map->capacity = new_capacity;
    map->size = 0;

    for(uint32_t i = 0; i < old_capacity; ++i) {
        if(old_list[i].status == MAP_OCCUPIED)
            map_insert(old_list[i].key, old_list[i].value, map);
    }
}

/**
 * method to find key
 * if exists, return index in list
 * else,      return UINT32_MAX
 * */
static int _map_find(uint32_t key, map_t *map) {
    uint32_t probe_level = 0, h = hash(key, map);
    uint32_t index = h + probe(probe_level++);
    while(map->list[index%map->capacity].status == MAP_OCCUPIED || \
          map->list[index%map->capacity].status == MAP_DELETED){
        if(map->list[index%map->capacity].key == key && \
           map->list[index%map->capacity].status == MAP_OCCUPIED) {
            return index%map->capacity;
        }
        index = h + probe(probe_level++);
    }

    return UINT32_MAX;
}

/**
 * method to find key
 * if exists, *value = value, return 1
 * else       *value unchanged, return 0
 * */
int map_find(uint32_t key, uint32_t *value, map_t *map) {
    int index = _map_find(key, map);
    if(index != UINT32_MAX) {
        *value = map->list[index].value;
        return 1;
    }
    else return 0;
}

void map_insert(uint32_t key, uint32_t value, map_t *map) {

    if((map->capacity_level < 31) && (map->capacity - map->size) * LOAD_FACTOR_DIVIDEND_MAX <= \
       (LOAD_FACTOR_DIVISOR_MAX - LOAD_FACTOR_DIVIDEND_MAX) * map->size)
        map_rehash(map, map->capacity_level + 1);
    uint32_t probe_level = 0, h = hash(key, map);
    uint32_t index = h + probe(probe_level++);
    while(map->list[index%map->capacity].status == MAP_OCCUPIED) {
        if(map->list[index%map->capacity].key == key)
        // if this key exists
            break;
        index = h + probe(probe_level++);
    }
    map->list[index%map->capacity].key = key;
    map->list[index%map->capacity].value = value;
    map->list[index%map->capacity].status = MAP_OCCUPIED;
    map->size++;
}

void map_delete(uint32_t key, map_t *map) {
    if((map->capacity_level > 3) && (map->capacity - map->size) * LOAD_FACTOR_DIVIDEND_MIN >= \
       (LOAD_FACTOR_DIVISOR_MIN - LOAD_FACTOR_DIVIDEND_MIN) * map->size)
            map_rehash(map, map->capacity_level - 1);
    int index = _map_find(key, map);
    if(index != UINT32_MAX){
        map->list[index].status = MAP_DELETED;
        map->size --;
    }
}