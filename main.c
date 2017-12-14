#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "collection.h"

typedef struct Person {
    char name[64];
    int age;
} Person;

Person *personCreate(char *name, int age)
{
    Person *person = NULL;
    if (!(person = malloc(sizeof(Person)))) {
        perror("malloc");
        return NULL;
    }
    strncpy(person->name, name, sizeof(person->name));
    person->age = age;

    return person;
}

void personDestroy(void *person)
{
    if (person) {
        free(person);
    }
}

bool printPerson(void *element, void *context)
{
    (void) (context);
    Person *p = element;
    printf("%s, %d\n", p->name, p->age);

    return true;
}

void printPersons(Collection *persons)
{
    collectionWalk(persons, printPerson, NULL);
}

int comparePersonsByAge(const void *a, const void *b)
{
    Person *p1, *p2;
    p1 = *(Person **) a;
    p2 = *(Person **) b;

    return (p1->age - p2->age);
    //return strncmp(p1->name, p2->name, sizeof(p1->name));
}

int comparePersonsByName(const void *a, const void *b)
{
    Person *p1, *p2;
    p1 = *(Person **) a;
    p2 = *(Person **) b;

    return strncmp(p1->name, p2->name, sizeof(p1->name));
}

/**
 * How to define a function to search for a char *name
 */
int findPersonByName(const void *name, const void *person)
{
    int ret = strcmp(name, (*(Person **) person)->name);
    return ret;
}

void collectionTest()
{
    Person p[] = {
            {"Viorel Irimia", 37},
            {"Evelyn Irimia", 6},
            {"Popescu Ion",   33},
            {"John Doe",      31},
            {"Jane Doe",      27},
    };
    int i;
    Person *person;

    printf("Testing collection\n");

    Collection *persons;
    if (!(persons = collectionCreate())) {
        return;
    }
    // test push
    for (i = 0; i < (int) (sizeof(p) / sizeof(*p)); i++) {
        person = personCreate(p[i].name, p[i].age);
        collectionPush(persons, person);
    }
    assert(persons->length == (int) (sizeof(p) / sizeof(*p)));
    assert(persons->capacity > persons->length);
    assert(persons->capacity % COLLECTION_INITIAL_CAPACITY == 0);
    assert(((Person *) persons->data[0])->age == 37);

    // test pop
    int len = persons->length;
    int idx = sizeof(p) / sizeof(*p);
    person = collectionPop(persons);
    assert(persons->length == len - 1);
    assert(person->age == p[idx - 1].age);
    assert(strncmp(person->name, p[idx - 1].name, sizeof(person->name)) == 0);
    collectionPush(persons, person);

    // test unshift
    person = personCreate("First Name", 10);
    collectionUnShift(persons, person);
    assert(((Person *) persons->data[persons->length - 1])->age == p[idx - 1].age);
    assert(strcmp(((Person *) persons->data[persons->length - 1])->name, p[idx - 1].name) == 0);
    assert(persons->length = len + 1);
    assert(strcmp(((Person *) persons->data[0])->name, person->name) == 0);
    assert(((Person *) persons->data[0])->age == person->age);

    // test shift
    person = collectionShift(persons);
    assert(person->age == 10);
    assert(strcmp(person->name, "First Name") == 0);
    assert(persons->length == len);
    personDestroy(person);

    // check unset multi
    collectionUnsetMulti(persons, 1, 3, personDestroy);
    assert(persons->data[1] == NULL);
    assert(persons->data[2] == NULL);
    assert(persons->data[3] == NULL);
    assert(persons->length == len);

    // test compact - data[4] will become data[1]
    person = persons->data[4];
    collectionCompact(persons);
    assert(persons->length == len - 3);
    assert(strcmp(((Person *) persons->data[1])->name, person->name) == 0);
    assert(((Person *) persons->data[1])->age == person->age);

    // reset
    int capacity = persons->capacity;
    collectionReset(persons, personDestroy);
    assert(capacity == persons->capacity);
    assert(persons->length == 0);
    assert(persons->data[0] == 0);

    Person *evePtr = NULL;
    // start again
    for (i = 0; i < idx; i++) {
        person = personCreate(p[i].name, p[i].age);
        collectionPush(persons, person);
        if (strcmp(p[i].name, "Evelyn Irimia") == 0) {
            evePtr = person; // save Eve pointer to use it later in find
        }
    }
    assert(evePtr != NULL);

    // sort persons by age in ascending order //
    collectionSort(persons, comparePersonsByAge);
    int prev = 0;
    for (i = 0; i < persons->length; i++) {
        int age = ((Person *) persons->data[i])->age;
        assert(prev < age);
        prev = age;
    }

    // sort persons by name to prepare for find //
    collectionSort(persons, comparePersonsByName);
    // also tests collectionWalk
    printPersons(persons);

    // test collectionFind
    char *str = "Evelyn Irimia";
    person = collectionFind(persons, str, findPersonByName);
    assert(person == evePtr); // SHOULD BE THE SAME POINTER
    assert(strcmp(person->name, str) == 0);
    assert(person->age == 6);

    collectionDestroy(persons, personDestroy);
}


int main(void)
{
    collectionTest();
}