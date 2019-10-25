#ifndef HASHTABLE_H_
#define HASHTABLE_H_

int hashCode(char* key, int size);
int get(char* key, char** keys, int* values, int size);
int add(char* key, char** keys, int* values, int size);
bool contains(char* key, char** keys, int size);
int set_add(char* key, char** keys, int size);

#endif
