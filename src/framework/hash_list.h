#ifndef _HASH_LIST_H
#define _HASH_LIST_H

typedef struct hashmap hashmap;

struct hashmap {
    void *key;
    void *value;
    void *next;
}

hashmap*    new_map();
hashmap*    pop(hashmap *map);
hashmap*    pop_reverse(hashmap *map);
hashmap*    push(hashmap *map, void *key, void *value);
hashmap*    push_reverse(hashmap *map, void *key, void *value);
bool        is_empty(hasmap *map);
bool        has(hashmap *map, void *key);
void        clear(hashmap *map);
void        print(hashmap *map);


#endif
