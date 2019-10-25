#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


int hashCode(char* key, int size)
{
    int val = 1;
    int i = 0;
    while(key[i] != '\0')
    {
        val = (val * key[i]) % size;
        if(val == 0) val++;
        i++;
    }
    val = val % size;
    return val;
}

int get(char* key, char** keys, int* values, int size)
{
    int val = hashCode(key, size);
    while(keys[val] != NULL)
    {
        if(strcmp(keys[val], key) == 0) return values[val];
        val++;
        if(val == size) val = 0;
    }
    return -1;
}

int add(char* key, char** keys, int* values, int size)
{
    int val = hashCode(key, size);
    while(keys[val] != NULL)
    {
        if(strcmp(keys[val], key) == 0)
        {
            values[val]++;
            return 0;
        }
        val = (val + 1) % size;
    }
    keys[val] = calloc(strlen(key), sizeof(char*));
    strcpy(keys[val], key);
    values[val]++;
    return 1;
}

bool contains(char* key, char** keys, int size)
{
    int val = hashCode(key, size);
    while(keys[val] != NULL)
    {
        if(strcmp(keys[val], key) == 0) return true;
        val++;
        if(val == size) val = 0;
    }
    return false;
}

int set_add(char* key, char** keys, int size)
{
    int val = hashCode(key, size);
    while(keys[val] != NULL)
    {
        if(strcmp(keys[val], key) == 0) return 0;
        val++;
        if(val == size) val = 0;
    }
    keys[val] = malloc(strlen(key));
    strcpy(keys[val], key);
    return 1;
}
