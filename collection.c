#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "collection.h"

Collection *collectionCreate()
{
    Collection *col = NULL;

    if (!(col = malloc(sizeof(Collection)))) {
        perror("malloc");
        return NULL;
    }
    col->capacity = 0;
    col->length = 0;
    col->data = NULL;

    return col;
}

void collectionDestroy(Collection *collection, freeFunc func)
{
    int i;
    if (collection->data) {
        if (func) {
            for (i = 0; i < collection->length; i++) {
                func(collection->data[i]);
            }
        }
        free(collection->data);
    }
    free(collection);
}

static bool grow(Collection *collection)
{
    int newCapacity = collection->capacity == 0
                      ? COLLECTION_INITIAL_CAPACITY
                      : collection->capacity * 2;

    collection->data = realloc(collection->data, newCapacity * sizeof(*collection->data));
    if (!collection->data) {
        perror("realloc");
        return false;
    }
    memset(
            collection->data + collection->length, 0,
            (newCapacity - collection->length) * sizeof(*collection->data));

    collection->capacity = newCapacity;

    return true;
}

bool collectionPush(Collection *collection, void *element)
{
    if (collection->length == collection->capacity) {
        if (!grow(collection)) {
            return false;
        }
    }
    collection->data[collection->length++] = element;

    return true;
}

void *collectionPop(Collection *collection)
{
    void *element;
    if (collection->length == 0) {
        return NULL;
    }
    collection->length--;
    element = collection->data[collection->length];
    collection->data[collection->length] = NULL;

    return element;
}

bool collectionUnShift(Collection *collection, void *element)
{
    if (collection->length == collection->capacity) {
        if (!grow(collection)) {
            return false;
        }
    }
    memmove(collection->data + 1, collection->data, (collection->length) * sizeof(*collection->data));
    collection->data[0] = element;
    collection->length++;

    return true;
}

void *collectionShift(Collection *collection)
{
    void *element = NULL;
    if (collection->length == 0) {
        return NULL;
    }
    collection->length--;
    element = collection->data[0];
    memmove(collection->data, collection->data + 1, (collection->length) * sizeof(*collection->data));
    collection->data[collection->length] = NULL;

    return element;
}

void collectionReset(Collection *collection, freeFunc func)
{
    int i;
    if (func) {
        for (i = 0; i < collection->length; i++) {
            func(collection->data[i]);
        }
    }
    memset(collection->data, 0, collection->capacity * sizeof(*collection->data));
    collection->length = 0;
}

bool collectionUnset(Collection *collection, int index, freeFunc func)
{
    if (index >= collection->length) {
        return false;
    }
    if (index < 0) {
        return false;
    }
    if (!collection->data[index]) {
        return false;
    }
    if (func) {
        func(collection->data[index]);
    }
    collection->data[index] = NULL;

    return true;
}

bool collectionUnsetMulti(Collection *collection, int index, int length, freeFunc func)
{
    int i;
    bool ret = true;
    if(index + length > collection->length) {
        return false;
    }
    for(i = index; i < index+length;i++){
        ret &= collectionUnset(collection, i, func);
    }

    return ret;
}

int collectionCompact(Collection *collection)
{
    int i, j;
    for (i = 0, j = 0; i < collection->length; i++, j++) {
        if (!collection->data[i])
            while (j < collection->length && collection->data[j] == NULL)
                j++;
        if (i < j)
            collection->data[i] = j < collection->length ? collection->data[j] : 0;
    }
    int shifted = j - i;
    collection->length -= shifted;

    return shifted;
}

int collectionWalk(Collection *collection, walkFunc func, void *context)
{
    int i;
    for(i = 0; i < collection->length; i++) {
        if(! func(collection->data[i], context)) {
            break;
        }
    }
    return i;
}

void collectionSort(Collection *collection, cmpFunc func)
{
    qsort(collection->data, (size_t)collection->length, sizeof(*collection->data), func);
}

void *collectionFind(Collection *collection, void *element, cmpFunc func)
{
    void *ret = bsearch(element, collection->data, (size_t) collection->length, sizeof(*collection->data), func);
    return *(void **)ret;
}