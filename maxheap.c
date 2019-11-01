/**
 * Functions needed to keep array representation of maxheap for storing words
 * and word ocurrences. These are used when calculating the top ocurrences
 * of words.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "maxheap.h"

/**
 * Calculate the number of leaves for a heap of a given size. Do this by 
 * calculating the elements that would fit if the last level was full and 
 * then figure out the number of parent elements they would have. Add this 
 * number to the amount of elements in the last level.
 * 
 * @param size: size of heap
 */ 
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

/**
 * Find the minimum element in the heap, by getting the smallest out of
 * all the leaves.
 * 
 * @param ocurs: Array of ocurs in heap
 * @param words: Array of words in heap
 * @param size: Number of elements that fit in heap
 * @param leaves: Number of elements without children in heap
 */ 
int getMinItem(int* ocurs, char** words, int size, int leaves)
{
    int minIdx = size - 1;
    for(int i = size - leaves - 2; i < size - 1; i++)
    {
        if(ocurs[i] < ocurs[minIdx] || (ocurs[i] == ocurs[minIdx] && ocurs[i] != 0 && 
        strcmp(words[i], words[minIdx]) > 0)) minIdx = i;
    }
    return minIdx;
}

/**
 * Attempt to add this element to the heap. Only add if greater than smallest
 * element.
 * 
 * @param ocur: Ocurrences for this element
 * @param word: Word for this element
 * @param ocurs: Array of ocurs in heap
 * @param words: Array of words in heap
 * @param size: Number of elements that can fit in heap
 * @param leaves: Number of elements without children in heap
 */ 
void add_heap(int ocur, char* word, int* ocurs, char** words, int size, int leaves)
{
    // Get the least element of the heap and see if element can be added
    int index = getMinItem(ocurs, words, size, leaves);
    if(ocurs[index] > ocur || (ocurs[index] == ocur && 
    strcmp(word, words[index]) > 0)) return;
    // Add to the least element of the heap
    int parentIndex = (index - 1) / 2;
    ocurs[index] = ocur;
    words[index] = realloc(words[index], (strlen(word) + 1) * sizeof(char));
    strcpy(words[index], word);
    // While this element is not in correct place, move it up the heap
    while(index > 0 && !in_order(ocurs, words, index, parentIndex))
    {
        swap_heap(ocurs, words, index, parentIndex);
        index = parentIndex;
        parentIndex = (parentIndex - 1) / 2;
    }
}

/**
 * See if parent index > child index.
 * 
 * @param ocurs: Array of word ocurrences/ word ocurs in heap
 * @param words: Array of words/ words in heap
 * @param index: Child index -- seeing if element here < parentIndex
 * @param parentIndex: Parent index -- seeing if element here > index
 */ 
bool in_order(int* ocurs, char** words, int index, int parentIndex)
{
    if(ocurs[parentIndex] > ocurs[index]) return true;
    // If they have the same ocurs, compare ASCII value to determine order
    if(ocurs[parentIndex] == ocurs[index]){
        if(words[index] == NULL) return true;
        if(words[parentIndex] == NULL) return false;
        if(strcmp(words[parentIndex], words[index]) < 0) return true;
    }
    return false;
}

/**
 * Swap to element in the heap. The same as swapping elements in an array.
 * 
 * @param ocurs: Ocurrences array, word ocurrences in heap
 * @param words: Words array, words in heap
 * @param index: child index being swapped
 * @param parentIndex: Index of child's parent being swapped
 */ 
void swap_heap(int* ocurs, char** words, int index, int parentIndex)
{
    int tempNum = ocurs[index];
    char* tempWord = words[index];
    ocurs[index] = ocurs[parentIndex];
    words[index] = words[parentIndex];
    ocurs[parentIndex] = tempNum;
    words[parentIndex] = tempWord;
}

/**
 * This function brings the next element to the top of the heap
 * and moves the elements displaced back into place.
 * 
 * @param ocurs: Word occurrences in heap
 * @param words: Words in heap
 * @param size: size of heap
 */ 
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

/**
 * Print out every value saved in max heap. At the same time remove 
 * and free elements after they are printed.
 * 
 * @param ocurs: Word ocurrences in heap
 * @param words: Words in heap
 * @param size: size of heap
 */ 
void print_max_heap(int* ocurs, char** words, int size)
{
    while(ocurs[0] != 0)
    {
        printf("%d %s\n", ocurs[0], words[0]);
        free(words[0]);
        words[0] = NULL;
        ocurs[0] = 0;
        heap_up(ocurs, words, size); // Bring next element to the top of heap
    }
}
