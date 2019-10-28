#ifndef MAXHEAP_H_
#define MAXHEAP_H_

int get_leaves(int size);
void add_heap(int ocur, char* word, int* ocurs, char** words, int size, int leaves);
bool in_order(int* ocurs, char** words, int index, int parentIndex);
void swap_heap(int* ocurs, char** words, int index, int parentIndex);
void heap_up(int* ocurs, char** words, int size);
void print_max_heap(int* ocurs, char** words, int size);

#endif