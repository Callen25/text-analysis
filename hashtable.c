/**
 * These are functions that are useful for my array implementation of a
 * hashtable. An array of words and their ocurrences are stored. Indexes
 * in words array map to same index in ocurrences array.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Gets the hash value for a key. A keys hash value is the multiplicitave
 * sums modulo with respect to size. This allows keys to be evenly distributed
 * in the set.
 * 
 * @param key: key being hashed
 * @param size: size of set
 */ 
int hashCode(char* key, int size)
{
    int val = 1;
    int i = 0;
    while(key[i] != '\0')
    {
        val = (val * key[i]) % size; // Mod every iteration to prevent ovrflw
        if(val == 0) val++;
        i++;
    }
    val = val % size;
    return val;
}

/**
 * Get value associated with key.
 * 
 * @param key: key looking for
 * @param keys: set of keys
 * @param values: set of associated values for keys
 * @param size: size of hashtable
 */ 
int get(char* key, char** keys, int* values, int size)
{
    int val = hashCode(key, size);
    // Look for key in block of addresses, if an open address is found, key DNE
    while(keys[val] != NULL)
    {
        if(strcmp(keys[val], key) == 0) return values[val];
        val++;
        if(val == size) val = 0;
    }
    return -1;
}

/**
 * Add a key and increment its value.
 * 
 * @param key: Key being added/modified
 * @param keys: Set of keys 
 * @param values: Set of values, where idx of key -> idx of value
 * @param size: size of hashtable
 */ 
int add(char* key, char** keys, int* values, int size)
{
    int val = hashCode(key, size);
    // Look for address of key or find open one
    while(keys[val] != NULL)
    {
        // If key already exists, increment its value
        if(strcmp(keys[val], key) == 0)
        {
            values[val]++;
            return 0;
        }
        val = (val + 1) % size;
    }
    // Assign key to open address, and increment its value
    keys[val] = malloc((strlen(key) + 1) * sizeof(char*));
    strcpy(keys[val], key);
    values[val]++;
    return 1;
}

/**
 * Check to see if set contains a specified key.
 * 
 * @param key: key looking for
 * @param keys: set of all keys
 * @param size: size of set
 */ 
bool contains(char* key, char** keys, int size)
{
    int val = hashCode(key, size);
    // Check sequential addresses for key, if empty address is reached: key DNE
    while(keys[val] != NULL)
    {
        if(strcmp(keys[val], key) == 0) return true;
        val++;
        if(val == size) val = 0;
    }
    return false;
}

/**
 * For when using hash table as a set. Adds a key to the
 * set of keys.
 * 
 * @param key: key adding to set
 * @param keys: set of keys
 * @param size: size of set
 */ 
int set_add(char* key, char** keys, int size)
{
    int val = hashCode(key, size);
    // Look for open address to add key into
    while(keys[val] != NULL)
    {
        if(strcmp(keys[val], key) == 0) return 0; // Key already exists in set
        val++;
        if(val == size) val = 0;
    }
    keys[val] = malloc((strlen(key) + 1) * sizeof(char));
    strcpy(keys[val], key);
    return 1;
}
