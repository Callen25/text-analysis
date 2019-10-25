#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "maxheap.h"

void add_heap(int ocur, char* word, int* ocurs, char** words, int size)
{
    int index = size - 1;
    if(ocur < ocurs[index]){
        for(int i = size / 2; i <= index; i++){
            if(ocurs[i] < ocur || 
            (ocurs[i] == ocur && strcmp(word, words[i]) < 0)) {
                index = i;
            }
        }
        if(index == size - 1) return; 
    }
    int parentIndex = (index - 1) / 2;
    ocurs[index] = ocur;
    words[index] = malloc(strlen(word));
    strcpy(words[index], word);
    while(index > 0 && !in_order(ocurs, words, index, parentIndex))
    {
        swap_heap(ocurs, words, index, parentIndex);
        index = parentIndex;
        parentIndex = (parentIndex - 1) / 2;
    }
}

bool in_order(int* ocurs, char** words, int index, int parentIndex)
{
    if(ocurs[parentIndex] > ocurs[index]) return true;
    if(ocurs[parentIndex] == ocurs[index]){
        if(words[parentIndex] == NULL && words[index] == NULL) return true;
        if(words[parentIndex] == NULL) return false;
        if(strcmp(words[parentIndex], words[index]) < 0) return true;
    }
    return false;
}

void swap_heap(int* ocurs, char** words, int index, int parentIndex)
{
    int tempNum = ocurs[index];
    char* tempWord = words[index];
    ocurs[index] = ocurs[parentIndex];
    words[index] = words[parentIndex];
    ocurs[parentIndex] = tempNum;
    words[parentIndex] = tempWord;
}

void heap_up(int* ocurs, char** words, int size)
{
    int index = 0;
    int left = 1;
    while(left < size)
    {
        int nextIdx;
        if(left + 1 < size)
            nextIdx = in_order(ocurs, words, left, left + 1) ? left + 1: left;
        else
            nextIdx = left;
        if(in_order(ocurs, words, nextIdx, index)) break;
        swap_heap(ocurs, words, nextIdx, index);
        index = nextIdx;
        left = (index * 2) + 1;
    }
}

void print_max_heap(int* ocurs, char** words, int size)
{
    while(ocurs[0] != 0)
    {
        printf("%d %s\n", ocurs[0], words[0]);
        free(words[0]);
        ocurs[0] = 0;
        heap_up(ocurs, words, size);
    }
}
