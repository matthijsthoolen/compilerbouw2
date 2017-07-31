#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "str.h"
#include "hash_list.h"

typedef struct hashmap hashmap;
#define ADD_HASH(h, key, val, nxt) h->key = key; h->val = val; h->nxt = nxt;

/**
 * Creates and returns a new hashmap
 *
 * @returns {hashmap}
 */
hashmap* new_map()
{
    hashmap *map;
    map = (hashmap *) MEMalloc(sizeof(map));
    
    ADD_HASH(map, NULL, NULL, NULL);
    return map;
}

/**
 * Returns the last added key/value pair
 *
 * @params  {hashmap}   map
 * @returns {hashmap}
 */
hashmap* pop(hashmap *map)
{
}

/**
 * Returns the first added key/value pair
 *
 * @params  {hashmap} map
 * @returns {hashmap} map
 */
hashmap* pop_reverse(hashmap *map)
{
}

/**
 * Add another key/value pair to the hashmap
 *
 * @param   {hashmap}   map
 * @param   {void}      key
 * @param   {void}      value
 */
hashmap* push(hashmap *map, void *key, void *value)
{
}

/**
 * Check if the hashmap is currently empty
 *
 * @param   {hashmap}   map
 * @returns {bool}
 */
bool is_empty(hasmap *map) {
}

/**
 * Check if the hashmap contains a key/value pair with the given key
 *
 * @param   {hashmap}   map
 * @param   {void}      key
 * @returns {bool}
 */
bool has(hashmap *map, void *key) {
}

/**
 * Remove the key.value pair with the given key
 *
 * @param   {hashmap}   map
 * @param   {void}      key
 */
hashmap* remove(hashmap *map, void *key) {
}

/**
 * Free the hashmap
 *
 * @param   {hashmap}   map
 */
void free(hashmap *map)
{
}

/**
 * Print the hashmap to the console
 *
 * @param   {hashmap}   map
 */
void print(hashmap *map)
{
    printf("Hashmap = {");

    if (map != null) {
        while (map->next) {
            printf("%s: ", (char*)map->key);
            printf("<%p>, ", map->value);            
        }
    }

    printf("}");
}
