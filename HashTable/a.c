#include "a.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

enum { BUCKET_COUNT = 1024};

struct Binding
{
    const char* key;
    int value;
    struct Binding* next;
};

struct HashTable
{
    Binding* buckets[BUCKET_COUNT];
};


HashTable* create() {

    HashTable* tableMemory = malloc(sizeof(HashTable));
    for (int i = 0; i < BUCKET_COUNT; i++)
    {
        tableMemory->buckets[i] = NULL;
    }
    
    return tableMemory;
}

unsigned int hash(const char* key) {

    int len = strlen(key);
    unsigned int asciiSum = 0;

    for (int i = 0; i < len; i++)
    {
        asciiSum += key[i];
        asciiSum *= key[i];
        asciiSum = asciiSum % BUCKET_COUNT;
    }
    return asciiSum;
}


bool add(struct HashTable* table, const char* key, int value) {

    int index = hash(key);
	Binding *binding = table->buckets[index];
	while (binding != NULL)
	{
		if (strcmp(key, binding->key) == 0)
		{ // key is found
			binding->value = value;
			return false;
		}
		binding = binding->next;
	}
	// key is not found
	Binding *new_binding = malloc(sizeof(binding));
	char *ownedKey = (char *)malloc(strlen(key) + 1);
	strcpy(ownedKey, key);

	new_binding->key = ownedKey;
	new_binding->value = value;
	new_binding->next = table->buckets[index];
	table->buckets[index] = new_binding;
	return true;
}

Binding* find(HashTable* table, const char* key) {

    unsigned int hashSlot = hash(key);

    // try to find a valid slot
    Binding* entrySlot = table->buckets[hashSlot];

    // the specifies hash table slot is empty, hence our value can't be there
    if (entrySlot == NULL) {
        return NULL;
    }

    // our hash table slot has a value inside of it
    else if (entrySlot != NULL) {
        // return value inside of slot, if it matches key 
        if (strcmp(entrySlot->key, key) == 0) {
            return entrySlot;
        }

        // proceed down the chained bindings looking for value
        entrySlot = entrySlot->next;
    }

    // At this point, we can see that the slot was filled but no matching entries 
    // but no matching entries were found
    return NULL;
}

void freeDynamicMemory(Binding *binding)
{
	free((char *) binding->key);
	free(binding);
}


bool remove(HashTable* table, const char* key) {
    unsigned int hashSlot = hash(key);
    Binding* newEntrySlot;
    newEntrySlot = table->buckets[hashSlot];

    // key is not on the first binding, so walk through each binding in that slot until 
    // either the end is reached or a matching key is found
    if (newEntrySlot != NULL)
    {
        Binding* tempStorage = newEntrySlot;
        if (strcmp(newEntrySlot->key, key) == 0)
        {
            tempStorage->next = newEntrySlot->next;

            // free the deleted entry
            freeDynamicMemory(newEntrySlot);
            return true;
        }
    }

    // key is on the first binding of that hash slot.
    else if (strcmp(newEntrySlot->key, key) == 0)
	{
        table->buckets[hashSlot] = newEntrySlot->next;
        // free the deleted entry
		freeDynamicMemory(newEntrySlot);
		return true;
	}
    
    return false;
}


void delete_table(HashTable* table) {

    for (int i = 0; i < BUCKET_COUNT; i++)
    {
        Binding* hashSlot = table->buckets[i];
        if (hashSlot == NULL) { continue; }
        table->buckets[i] = hashSlot->next;
        
        freeDynamicMemory(hashSlot);

        do
        {
            if (hashSlot->next == NULL)
            {
                break;
            }
            hashSlot = hashSlot->next;

        } while (true);
    }
    free(table);
    
}
