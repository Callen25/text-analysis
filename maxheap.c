#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "maxheap.h"

int get_leaves(int size)
{
    int levels = 0;
    int tmpSize = size;
    while(tmpSize > 0)
    {
        tmpSize = tmpSize >> 1;
        levels++;
    }
    int total = 2 << (levels - 1);
    int lastRow = (total / 2) - (total - size - 1);
    int secondLastRow = total / 4;
    return lastRow + ( (2 * secondLastRow) - lastRow)/2;
}

int getMinItem(int* ocurs, int size, int leaves)
{
    int minIdx = ocurs[size - 1];
    for(int i = size - leaves - 2; i < size - 1; i++)
    {
        if(ocurs[i] < ocurs[minIdx]) minIdx = i;
    }
    return minIdx;
}

void add_heap(int ocur, char* word, int* ocurs, char** words, int size, int leaves)
{
    int index = getMinItem(ocurs, size, leaves);
    if(ocurs[index] > ocur || (ocurs[index] == ocur && 
    strcmp(word, words[index]) < 0)) return;

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
        if(words[index] == NULL) return true;
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
