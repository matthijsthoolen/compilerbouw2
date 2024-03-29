#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "str.h"
#include "dbug.h"
#include "hash_list.h"

/**
 * Creates and returns a new hashmap
 *
 * @returns {hashmap}
 */
hashmap* new_map()
{
    hashmap *map;
    map = (hashmap *) MEMmalloc(sizeof(hashmap));
    
    ADD_HASH(map, NULL, NULL, NULL);
    return map;
}

/**
 * Returns the last added key/value pair
 *
 * @params  {hashmap}   map
 * @returns {hashmap}
 */
hashmap* map_pop(hashmap *map)
{
    DBUG_ENTER("map_pop");

    if (!map) {
        DBUG_RETURN(map);
    }

    hashmap *pop;
    pop = map->next;
    map->next = pop->next;
    pop->next = NULL;
    
    DBUG_RETURN(pop);
}

/**
 * Returns the first added key/value pair
 *
 * @params  {hashmap} map
 * @returns {hashmap} map
 */
hashmap* map_pop_reverse(hashmap *map)
{
    DBUG_ENTER("map_pop_reverse");

    if (!map) {
        DBUG_RETURN(map);
    }    

    hashmap *pop;
    pop = map->next;
    map->next = pop->next;
    pop->next = NULL; 

    DBUG_RETURN(pop);
}

/**
 * Add another key/value pair to the hashmap
 *
 * @param   {hashmap}   map
 * @param   {void}      key
 * @param   {void}      value
 */
bool map_push(hashmap *map, void *key, void *value)
{
    DBUG_ENTER("map_push"); 

    if (!map) {
        DBUG_RETURN(FALSE);
    }
 
    hashmap *new;
    new = new_map();

    ADD_HASH(new, key, value, map->next);

    map->next = new;

    DBUG_RETURN(TRUE);
}

/**
 * Check if the hashmap is currently empty
 *
 * @param   {hashmap}   map
 * @returns {bool}
 */
bool map_is_empty(hashmap *map) {
    DBUG_ENTER("map_is_empty");

    if (!map) {
        DBUG_RETURN(TRUE);
    }
    
    if (map->next == NULL) {
        DBUG_RETURN(TRUE);
    }
    
    DBUG_RETURN(FALSE);
}

/**
 * Check if the hashmap contains a key/value pair with the given key
 *
 * @param   {hashmap}   map
 * @param   {void}      key
 * @returns {bool}
 */
bool map_has(hashmap *map, void *key) {
    DBUG_ENTER("map_has");

    if (!map) {
        DBUG_RETURN(FALSE);
    }

    while((map = map->next)) {
        if(STReq(map->key, key)) {
            DBUG_RETURN(TRUE);
        }
    }

    DBUG_RETURN(FALSE);
}

/**
 * Get a key/value pair if the hashmap contains a key/value pair with the given key
 *
 * @param   {hashmap}   map
 * @param   {void}      key
 * @returns {hashmap}
 */
void* map_get(hashmap *map, void *key) {
    DBUG_ENTER("map_get");

    if (!map) {
        DBUG_RETURN(NULL);
    }

    DBUG_PRINT("HELP", ("Return map value with key %s if exists", key));

    while((map = map->next)) {
        
        if(STReq(map->key, key)) {
            DBUG_RETURN(map->value);
        }
    }

    DBUG_RETURN(NULL);
}

/**
 * Remove the key.value pair with the given key
 *
 * @param   {hashmap}   map
 * @param   {void}      key
 * @returns {bool}
 */
bool map_remove(hashmap *map, void *key)
{
    DBUG_ENTER("map_remove");

    if (!map) {
        DBUG_RETURN(FALSE);
    }

    hashmap *tmp;

    while((map->next)) {
        tmp = map->next;

        if(STReq(tmp->key, key)) {
            map->next = tmp->next;
            MEMfree(tmp);
            DBUG_RETURN(TRUE);
        }
        map = map->next;
    }

    DBUG_RETURN(FALSE);
}

/**
 * Free the hashmap
 *
 * @param   {hashmap}   map
 */
void map_free(hashmap *map)
{
    if (!map) {
        return;
    }

    hashmap *tmp;

    while(map) {
        tmp = map;
        map = map->next;
        tmp->next = NULL;
        MEMfree(tmp);
    }
}
