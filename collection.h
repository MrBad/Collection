#ifndef COLLECTION_H
#define COLLECTION_H

#include <stdbool.h>

#define COLLECTION_INITIAL_CAPACITY 4

typedef struct Collection Collection;

struct Collection {
    void **data;    // pointer to the array of data
    int capacity;   // maximum number of elements this collection can store
    int length;     // number of elements stored right now
};

typedef void (*freeFunc)(void *element);
typedef int (*cmpFunc)(const void *a, const void *b);
typedef bool (*walkFunc)(void *element, void *context);

Collection *collectionCreate();
void collectionDestroy(Collection *collection, freeFunc func);
bool collectionPush(Collection *collection, void *element);
void *collectionPop(Collection *collection);
bool collectionUnShift(Collection *collection, void *element);
void *collectionShift(Collection *collection);
void collectionReset(Collection *collection, freeFunc func);
bool collectionUnset(Collection *collection, int index, freeFunc func);
bool collectionUnsetMulti(Collection *collection, int index, int length, freeFunc func);
int collectionCompact(Collection *collection);
int collectionWalk(Collection *collection, walkFunc func, void *context);
void collectionSort(Collection *collection, cmpFunc func);
void *collectionFind(Collection *collection, void *element, cmpFunc func);

#endif // COLLECTION_H
