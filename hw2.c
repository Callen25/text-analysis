/**
 * This program extends the regex from HW1 and adds text-statistics 
 * functionality, as specified in HW2 specs.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "hw1.h"
#include "hashtable.h"
#include "maxheap.h"

#define SINGLE_SIZE 100000
#define SINGLE_TOP 50
#define BIGRAM_SIZE 50000
#define BIGRAM_TOP 20
#define TRIGRAM_SIZE 30000
#define TRIGRAM_TOP 12
#define STOP_SIZE 75
#define HTML_CHARS 5

#define MATCHES_SIZE 50000

const char* const htmlChars[] = {"nbsp", "quot", "amp", "lt", "gt"};

/**
 * Struct for counting word/bigram/trigram ocurrences.
 * A hashtable for calculating unique words, and a heap for 
 * calculating top N words/
 */
typedef struct word_count
{
    // Save words/ocurrences in hash table
    int size;
    char **dictionary;
    int *ocurs;
    // Count of total and unique ocurrences
    int total;
    int unique;
    // Save top N words in a heap of size N
    int heap_size;
    int leaves;
    char **words;
    int *top_ocurs;
} word_count;

/**
 * Create a word count struct for single words, bigrams, or trigrams.
 * 
 * @param tableSize: size of hash table (max number of words that can be held)
 * @param heapSize: size of heap (i.e. the number of words to calc top N of)
 */ 
word_count new_word_count(int tableSize, int heapSize)
{
    word_count new = {tableSize, calloc(tableSize, sizeof(char *)), 
    calloc(tableSize, sizeof(int)), 0, 0, heapSize, get_leaves(heapSize), 
    calloc(heapSize, sizeof(char *)), calloc(heapSize, sizeof(int))};
    return new;
}

/**
 * After counts are calculated, calculate the top N words/bigrams/trigrams.
 * Using a fixed size maxheap.
 * 
 * @param topW: a word_count struct for either single words, bigrams, or trigrams
 */ 
void calc_top_N(word_count topW)
{
    for (int i = 0; i < topW.size; i++)
    {
        if (topW.ocurs[i] > 0)
        {
            add_heap(topW.ocurs[i], topW.dictionary[i], topW.top_ocurs,
                     topW.words, topW.heap_size, topW.leaves);
            free(topW.dictionary[i]);
        }
    }
}

/**
 * Adds the top 50 words as specified to the HashSet of stop words.
 * 
 * @param stop_words: HashSet of stop_words
 * @param size: size of HashSet
 */ 
void fill_stop_words(char ***stop_words, int size)
{
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

/**
 * Determines if word with &; sentinels about to be added should be ignored 
 * since it is an html char.
 * 
 * @param word: word about to be counted
 */ 
bool is_htmlChar(char* word)
{
    for(int i = 0; i < HTML_CHARS; i++)
        if(strcmp(word, htmlChars[i]) == 0) return true;
    return false;
}

/**
 * Takes two words and combines them into a bigram.
 * i.e. "word1" "word2" -> "word1 word2"
 * 
 * @param bigram: two sequential words
 * @param firstWord: first word in sequence
 * @param secondWord: second word in sequence
 */ 
void make_bigram(char **bigram, char *firstWord, char *secondWord)
{
    strcpy(*bigram, firstWord);
    strcat(*bigram, " ");
    strcat(*bigram, secondWord);
}

/**
 * Go through file and find all lines that match regex pattern.
 * 
 * @param filename: path to file
 * @param regex: regex pattern to match
 * @param matches: Array of lines in file that match regex pattern
 */ 
int regex_match(const char* filename, const char* regex, char*** matches)
{
    *matches = malloc(500 * sizeof(char*));

    int m_index = 0;
    reg_char pattern[MAX_LENGTH];
    compile_reg(regex, pattern);

    FILE *read = fopen(filename, "r");

    if (read == NULL)
    {
        fprintf(stderr, "ERROR: input file <%s> does not exist\n", filename);
        exit(1);
    }

    // Read all lines of file into next index
    (*matches)[m_index] = malloc(1025 * sizeof(char));
    while (fgets((*matches)[m_index], 1025, read))
    {
        // If line matches, start adding to next index
        if (match_p((*matches)[m_index], pattern, 0, 0, 0) != -1)
        {
            (*matches)[m_index+1] = malloc(1025 * sizeof(char));
            (*matches)[m_index][strlen((*matches)[m_index]) - 1] = '\0';
            m_index++;
        }
    }
    free((*matches)[m_index]); // Last index is not used
    (*matches)[m_index] = NULL;

    return m_index;
}

/**
 * Goes here if in_script. Look for closing script tag, and leave if found. If
 * one was not found for the whole line, leave flag on and look for closing tag
 * in next line.
 * 
 * @param line: current line being read
 * @param i: current pos in line
 * @param in_script: flag turned on if inside script
 */ 
void ignore_script(char *line, int *i, bool *in_script)
{
    int start = *i;
    while (line[*i] != '\0' && *in_script)
    {
        if (line[*i] == '<')
            start = *i;
        else if (*i - start == 7)
        {
            // Check to see if this tag is a closing script tag
            char tag[*i - start + 1];
            memcpy(tag, &line[start + 1], *i - start);
            tag[*i - start] = '\0';
            if (strcmp(tag, "/script") == 0)
                *in_script = false;
        }
        *i += 1;
    }
}

/**
 * Take a valid word and add it to single word count. If bigram/trigram is valid,
 * do the same for them.
 * 
 * @param line: current line being read
 * @param single: count of individual words
 * @param bigrams: count of bigrams
 * @param biword: last word
 * @param trigrams: count of trigrams
 * @param triword: second to last word
 * @param stop_words: words to ignore for bigrams and trigrams
 * @param i: current position in line
 * @param start: start of current word
 */ 
void add_count(char* line, word_count *single, word_count *bigrams, char **biword, 
word_count *trigrams, char **triword, char **stop_words, int i, int start)
{
    char sub[i - start + 1];
    memcpy(sub, &line[start], i - start);
    sub[i - start] = '\0';
    // Ensure this is not a special html character
    if(start > 0 && line[start - 1] == '&' && line[i] == ';' && is_htmlChar(sub))
    {
        start = i;
        return;
    }

    single->unique += add(sub, single->dictionary, single->ocurs,
                          single->size);
    single->total += 1;
    // If this is a valid bigram, add it to bigram count
    if (*biword != NULL && !contains(sub, stop_words, STOP_SIZE))
    {
        char *bigram = calloc(strlen(sub) + strlen(*biword) + 2, sizeof(char));
        make_bigram(&bigram, *biword, sub);
        bigrams->unique += add(bigram, bigrams->dictionary,
                               bigrams->ocurs, bigrams->size);
        bigrams->total += 1;
        // If this is a valid trigram, add it to trigram count
        if (*triword != NULL)
        {
            char *trigram = calloc(strlen(bigram) + strlen(*triword) + 2, 
            sizeof(char));
            make_bigram(&trigram, *triword, bigram);
            trigrams->unique += add(trigram, trigrams->dictionary,
                                    trigrams->ocurs, trigrams->size);
            trigrams->total += 1;
            free(trigram);
        }
        free(bigram);
    }
    // Assign triword to old biword if valid
    free(*triword);
    if(*biword != NULL) {
        *triword = malloc((strlen(*biword) + 1) * sizeof(char));
        strcpy(*triword, *biword);
    }
    else *triword = NULL;
    // Assign biword to current word if valid
    free(*biword);
    if (!contains(sub, stop_words, STOP_SIZE))
    {
        *biword = malloc((strlen(sub) + 1) * sizeof(char));
        strcpy(*biword, sub);
    }
    else *biword = NULL;
}
/**
 * parse_words adds valid words, bigrams, trigrams to their respective word
 * counters. It ignores certain HTML characters, anything inside HTML tags
 * and anything inside of a script.
 * 
 * @param line: current line in file
 * @param single: count of single words/unique single words
 * @param bigrams: count of bigrams/unique bigrams
 * @param biword: last word, used for adding bigram
 * @param trigrams: count of trigrams/unique trigrams
 * @param triword: second last word, used for adding trigram
 * @param stop_words: words to skip for calculating bigrams and trigrams
 * @param in_tag: set to true when inside a tag
 * @param in_script: set to true when inside a script
 * @param check_pos: checkpoint position for backtracking
 * @param check_line: checkpoint line to go to for backtracking
 * @param cur_line: current line, set check_line to this to save checkpoint
 * @param checkpoint: flag that is turned on if we are returning
 */ 
void parse_words(char *line, word_count *single, word_count *bigrams, char **biword, 
word_count *trigrams, char **triword, char **stop_words, bool *in_tag, bool *in_script,
int* check_pos, int* check_line, int cur_line, bool checkpoint)
{
    int i = 0;
    if(checkpoint) i = *check_pos + 1;
    int start = i;
    bool apos = false; // Turned on if an apostraphe is found in current word
    while (line[i] != '\0')
    {
        if (isalpha(line[i]))
            line[i] = tolower(line[i]);
        if (*in_script)
        {
            ignore_script(line, &i, in_script);
            start = i;
        }
        else if (*in_tag && line[i] == '>')
        {
            if (i - start >= 6)
            {
                // Determine what type of tag we found. If it is a script
                // tag, ignore everything inside of it, else we have exited a tag
                char tag[7];
                memcpy(tag, &line[start], 7);
                tag[6] = '\0';
                if (strcmp(tag, "script") == 0)
                {
                    *in_script = true;
                    *check_pos = i;
                    *check_line = cur_line;
                    ignore_script(line, &i, in_script);
                }
            }
            *in_tag = false;
            start = i;
        }
        else if (!isalpha(line[i]) && !*in_tag) // End of word, add if possible
        {
            if (line[i] == '<')
            {
                *in_tag = true;
                *check_pos = i;
                *check_line = cur_line;
            }
            if (i - start >= 2 && (line[i] != '\'' || apos))
            {
                // This is only used when a word ends with ''
                if (!isalpha(line[i - 1]))
                {
                    // The word is delimited and ignored if too small
                    i--;
                    if (i - start < 2)
                    {
                        start = i;
                        apos = false;
                        continue;
                    }
                }
                // Word is valid, so add
                add_count(line, single, bigrams, biword, trigrams, triword, 
                stop_words, i, start);
                start = i;
                apos = false;
            }
            // If an apos is found, turn flag on to ensure only one per word
            else if (line[i] == '\'')
                apos = true; 
            else // We reached a terminating character, so start over
            {
                start = i;
                apos = false;
            }
        }
        // Move beginning character up, so that word is onlu valid chars
        else if (!isalpha(line[start]))
        {
            while (start < i)
                if (line[start++] == '\'') apos = false;
        }
        i++;
    }
}

/**
 * Prints the output of the program. i.e. Total words, top N words etc.
 * 
 * @param count: number of files
 * @param single: count of single words
 * @param bigrams: count of bigrams
 * @param trigrams: count of trigrams
 */ 
void print_statistics(int count, word_count *single, word_count *bigrams,
                      word_count *trigrams)
{
    printf("Total number of documents: %d\n", count);
    printf("Total number of words: %d\n", single->total);
    printf("Total number of unique words: %d\n", single->unique);

    printf("Total number of interesting bigrams: %d\n", bigrams->total);
    printf("Total number of unique interesting bigrams: %d\n",
           bigrams->unique);

    printf("Total number of interesting trigrams: %d\n", trigrams->total);
    printf("Total number of unique interesting trigrams: %d\n",
           trigrams->unique);

    printf("\nTop 50 words:\n");
    print_max_heap(single->top_ocurs, single->words, single->heap_size);

    printf("\nTop 20 interesting bigrams:\n");
    print_max_heap(bigrams->top_ocurs, bigrams->words, bigrams->heap_size);

    printf("\nTop 12 interesting trigrams:\n");
    print_max_heap(trigrams->top_ocurs, trigrams->words, trigrams->heap_size);
}

/**
 * For a given file, calculate text statistics and add them to single, bigrams,
 * and trigrams counts.
 * 
 * @param filename: location of file 
 * @param single: count of words
 * @param bigrams: count of bigrams
 * @param trigrams: count of trigrams
 * @param stop_words: words to ignore for bigrams and trigrams
 */ 
void count_words(char *filename, word_count *single, word_count *bigrams,
                 word_count *trigrams, char **stop_words, bool checkpoint, 
                 int check_line, int check_pos)
{
    FILE* read = fopen(filename, "r");
    char line[1025];
    int cur_line = 0;
    bool in_tag = false;
    bool in_script = false;
    char *biword = NULL;
    char *triword = NULL;

    while (fgets(line, 1025, read))
    {
        if(cur_line >= check_line)
        {
            parse_words(line, single, bigrams, &biword, trigrams, &triword, 
            stop_words, &in_tag, &in_script, &check_pos, &check_line, cur_line, 
            checkpoint);
            if(checkpoint) checkpoint = false;
        }
        cur_line++;
    }

    free(biword);
    free(triword);
    fclose(read);

    // If still in a tag at end of file, return to checkpoint 
    if(in_tag || in_script) 
    {
        count_words(filename, single, bigrams, trigrams, stop_words, true, 
        check_line, check_pos);
    }
}
/**
 * Calculate text statistics of array of files as specified in specs.
 * 
 * @param count: number of files
 * @param files: array of files (command line arguments)
 */ 
void text_statistics(int count, char **files)
{
    word_count single = new_word_count(SINGLE_SIZE, SINGLE_TOP);
    word_count bigrams = new_word_count(BIGRAM_SIZE, BIGRAM_TOP);
    word_count trigrams = new_word_count(TRIGRAM_SIZE, TRIGRAM_TOP);

    char **stop_words = calloc(STOP_SIZE, sizeof(char *));
    fill_stop_words(&stop_words, STOP_SIZE);

    for (int i = 1; i < count; i++)
    {
        count_words(files[i], &single, &bigrams, &trigrams, stop_words, false, 0, 0);
    }

    for (int i = 0; i < STOP_SIZE; i++)
    {
        free(stop_words[i]);
    }
    free(stop_words);

    calc_top_N(single);
    calc_top_N(bigrams);
    calc_top_N(trigrams);

    print_statistics(count - 1, &single, &bigrams, &trigrams);

    free(single.dictionary);
    free(single.ocurs);
    free(single.top_ocurs);
    free(single.words);

    free(bigrams.dictionary);
    free(bigrams.ocurs);
    free(bigrams.top_ocurs);
    free(bigrams.words);

    free(trigrams.dictionary);
    free(trigrams.ocurs);
    free(trigrams.top_ocurs);
    free(trigrams.words);
}

#ifdef USE_SUBMITTY_MAIN

int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);
    if ( argc != 3 )
    {
        fprintf( stderr, "ERROR: Invalid arguments\n" );
        fprintf( stderr, "USAGE: %s <regex-file> <input-file>\n", argv[0] );
        return EXIT_FAILURE;
    }

    FILE *regex_file = fopen(argv[1], "r");
    if (regex_file == NULL)
    {
        fprintf(stderr, "ERROR: regex file <%s> does not exist\n", argv[1]);
        exit(1);
    }

    char reg_pattern[MAX_LENGTH];
    fgets(reg_pattern, MAX_LENGTH, regex_file);
    fclose(regex_file);

    char** matches = NULL;

    int l = regex_match(argv[2], reg_pattern, &matches);

    int i;
    for(i = 0; i < l; i++)
    {
        printf("%s\n", matches[i]);
    }
    for(i = 0; i < l; i++)
    {
        free(matches[i]);
    }
    free(matches);

    return EXIT_SUCCESS;
}

#else

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Invalid arguments\n"
                        "USAGE: a.out <input-file1> [ <input-file2> ... ]\n");
        return 1;
    }
    text_statistics(argc, argv);
}

#endif
