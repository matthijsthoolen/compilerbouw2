#ifndef _HASH_LIST_H
#define _HASH_LIST_H

#define ADD_HASH(h, k, v, n) h->key = k; h->value = v; h->next = n;

typedef struct hashmap hashmap;

struct hashmap {
    void *key;
    void *value;
    void *next;
};

hashmap*    new_map();
hashmap*    map_pop(hashmap *map);
hashmap*    map_pop_reverse(hashmap *map);
bool        map_push(hashmap *map, void *key, void *value);
bool        map_push_reverse(hashmap *map, void *key, void *value);
bool        map_is_empty(hashmap *map);
bool        map_has(hashmap *map, void *key);
void*       map_get(hashmap *map, void *key);
bool        map_remove(hashmap *map, void *key);
void        map_free(hashmap *map);
void        map_print(hashmap *map);

#endif
