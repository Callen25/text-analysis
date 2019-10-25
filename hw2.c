#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "hw1.h"
#include "hashtable.h"
#include "maxheap.h"

typedef struct word_count
{
    // Save words/ocurrences in hash table
    int size;
    char** dictionary;
    int* ocurs;
    // Count of total and unique ocurrences
    int total;
    int unique;
    // Save top N words in a heap of size N
    int heap_size;
    char** words;
    int* top_ocurs;
} word_count;

/* Add top 50 most common words to set of stop words */
void fill_stop_words(char*** stop_words, int size)
{
    //Arr is 1.5x size of set to decrease amount of collisions
    set_add("the", *stop_words, size);
    set_add("of", *stop_words, size);
    set_add("to", *stop_words, size);
    set_add("a", *stop_words, size);
    set_add("and", *stop_words, size);
    set_add("in", *stop_words, size);
    set_add("said", *stop_words, size);
    set_add("for", *stop_words, size);
    set_add("that", *stop_words, size);
    set_add("was", *stop_words, size);
    set_add("on", *stop_words, size);
    set_add("he", *stop_words, size);
    set_add("is", *stop_words, size);
    set_add("with", *stop_words, size);
    set_add("at", *stop_words, size);
    set_add("by", *stop_words, size);
    set_add("it", *stop_words, size);
    set_add("from", *stop_words, size);
    set_add("as", *stop_words, size);
    set_add("be", *stop_words, size);
    set_add("were", *stop_words, size);
    set_add("an", *stop_words, size);
    set_add("have", *stop_words, size);
    set_add("his", *stop_words, size);
    set_add("but", *stop_words, size);
    set_add("has", *stop_words, size);
    set_add("are", *stop_words, size);
    set_add("not", *stop_words, size);
    set_add("who", *stop_words, size);
    set_add("they", *stop_words, size);
    set_add("its", *stop_words, size);
    set_add("had", *stop_words, size);
    set_add("will", *stop_words, size);
    set_add("would", *stop_words, size);
    set_add("about", *stop_words, size);
    set_add("i", *stop_words, size);
    set_add("been", *stop_words, size);
    set_add("this", *stop_words, size);
    set_add("their", *stop_words, size);
    set_add("new", *stop_words, size);
    set_add("or", *stop_words, size);
    set_add("which", *stop_words, size);
    set_add("we", *stop_words, size);
    set_add("more", *stop_words, size);
    set_add("after", *stop_words, size);
    set_add("us", *stop_words, size);
    set_add("percent", *stop_words, size);
    set_add("up", *stop_words, size);
    set_add("one", *stop_words, size);
    set_add("people", *stop_words, size);
}

void make_bigram(char** bigram, char* firstWord, char* secondWord)
{
    strcpy(*bigram, firstWord);
    strcat(*bigram, " ");
    strcat(*bigram, secondWord);
}

int regex_match(const char* filename, const char* regex, char*** matches)
{
    int m_index = 0;
    reg_char pattern[MAX_LENGTH];
    compile_reg(regex, pattern);

    FILE* read = fopen(filename, "r");

    if (read == NULL)
    {
        fprintf(stderr, "ERROR: input file <%s> does not exist\n", filename);
        exit(1);
    }

    char line[1025];
    while(fgets(line, 1025, read))
    {
        if (match_p(line, pattern, 0, 0, 0) != -1)
        {
            *matches[m_index] = malloc(1025);
            strcpy(*matches[m_index], line);
            m_index++;
        }
    }

    return m_index;
}

void ignore_script(char* line, int* i, bool* in_script)
{
    int start = *i;
    while(line[*i] !='\0' && *in_script)
    {
        if(line[*i] == '<') start = *i;
        else if(*i - start == 7){
            char tag[*i - start + 1];
            memcpy(tag, &line[start + 1], *i - start);
            tag[*i - start] = '\0';
            if(strcmp(tag, "/script") == 0) *in_script = false;
        }
        *i += 1;
    }
}

void parse_words(char* line, word_count* single, word_count* bigrams,
char** biword, char** stop_words, bool* in_tag, bool* in_script)
{
    int i = 0;
    int start = 0;
    bool apos = false;
    while(line[i] != '\0')
    {
        if(isalpha(line[i])) line[i] = tolower(line[i]);
        if(*in_script) ignore_script(line, &i, in_script);
        else if(*in_tag && line[i] == '>')
        {
            if(i - start >= 6){
                char tag[7];
                memcpy(tag, &line[start], 7);
                tag[6] = '\0';
                if(strcmp(tag, "script") == 0){
                    *in_script = true;
                    ignore_script(line, &i, in_script);
                } 
            }
            *in_tag = false;
            start = i;
        }
        else if(!isalpha(line[i]) && !*in_tag)
        {
            if(line[i] == '<') *in_tag = true;
            if(i - start >= 2 && (line[i] != '\'' || apos)){
                if(!isalpha(line[i-1])){
                    i--;
                    if(i - start < 2){
                        start = i;
                        apos = false;
                        continue;
                    }
                }
                char sub[i - start + 1];
                memcpy(sub, &line[start], i - start);
                sub[i - start] = '\0';
                single->unique += add(sub, single->dictionary, single->ocurs, 
                single->size);
                single->total += 1;
                if(*biword != NULL && !contains(sub, stop_words, 75)){
                    char* bigram = malloc(strlen(sub) + strlen(*biword) + 2);
                    make_bigram(&bigram, *biword, sub);
                    bigrams->unique += add(bigram, bigrams->dictionary, 
                    bigrams->ocurs, bigrams->size);
                    bigrams->total += 1;
                }
                if(!contains(sub, stop_words, 75)) {
                    *biword = malloc(strlen(sub) + 1);
                    strcpy(*biword, sub);
                }
                else *biword = NULL;
                start = i;
                apos = false;
            }
            else if(line[i] == '\'') apos = true;
            else{
                start = i;
                apos = false;
            }
        }
        else if(!isalpha(line[start])){
            while(start < i){
                if(line[start++] == '\'') apos = false;
            }
        }
        i++;
    }
}

void print_statistics(int count, word_count* single, word_count* bigrams)
{
    printf("Total number of documents: %d\n", count);
    printf("Total number of words: %d\n", single->total);
    printf("Total number of unique words: %d\n", single->unique);
    printf("Total number of interesting bigrams: %d\n", bigrams->total);
    printf("Total number of unique interesting bigrams: %d\n", 
    bigrams->unique);
    printf("\nTop 50 words:\n");
    print_max_heap(single->top_ocurs, single->words, single->heap_size);
    printf("\nTop 20 interesting bigrams:\n");
    print_max_heap(bigrams->top_ocurs, bigrams->words, bigrams->heap_size);
}

void count_words(char* filename, word_count* single, word_count* bigrams, 
char** stop_words)
{
    FILE* read = fopen(filename, "r");
    char line[1025];
    bool in_tag = false;
    bool in_script = false;
    char* biword = NULL;

    while(fgets(line, 1025, read))
    {
        parse_words(line, single, bigrams, &biword, stop_words, &in_tag, 
        &in_script);
    }
}

void text_statistics(int count, char** files, char*** stop_words)
{
    word_count single = {100000, calloc(100000, sizeof(char*)), calloc(100000, 
    sizeof(int)), 0, 0, 50, calloc(50, sizeof(char*)), calloc(50, 
    sizeof(int))};

    word_count bigrams = {40000, calloc(40000, sizeof(char*)), calloc(40000,
    sizeof(int)), 0, 0, 20, calloc(20, sizeof(char*)), calloc(20, 
    sizeof(int))};

    for(int i = 1; i < count; i++)
    {
        count_words(files[i], &single, &bigrams, *stop_words);
    }

    for(int i = 0; i < single.size; i++){
        if(single.ocurs[i] > 0){
            add_heap(single.ocurs[i], single.dictionary[i], single.top_ocurs, 
            single.words, single.heap_size);
            free(single.dictionary[i]);
        }
    }

    for(int i = 0; i < bigrams.size; i++){
        if(bigrams.ocurs[i] > 0){
            add_heap(bigrams.ocurs[i], bigrams.dictionary[i], bigrams.top_ocurs,
            bigrams.words, bigrams.heap_size);
            free(bigrams.dictionary[i]);
        }
    }

    print_statistics(count - 1, &single, &bigrams);

    free(single.dictionary);
    free(single.ocurs);
    free(single.top_ocurs);
    free(single.words);
}

int main(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Invalid arguments\n"
                        "USAGE: a.out <inputt-file>\n");
        return 1;
    }
    char** stop_words = calloc(75, sizeof(char*));
    fill_stop_words(&stop_words, 75);
    text_statistics(argc, argv, &stop_words);
}
