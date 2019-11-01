/**
 * This program reads files line by line and determines if a given
 * regex pattern matches the line, and prints out all lines that match.
 * 
 * Extended for HW2 to support groups in regex []
 * 
 * See HW1 and HW2 specs
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#define MAX_LENGTH 257

/* Enum of possible meta chars */
enum meta_char
{
    DOT,
    PLUS,
    STAR,
    QMARK,
    DIGIT,
    NON_DIGIT,
    LETTER,
    NON_LETTER,
    SPACE,
    GROUP,
    COMP_GROUP,
    END
};
/**
 * This is the compiled version of a regex character.
 */ 
typedef struct reg_char
{
    bool is_meta; // Flag that denotes if a char is a meta char
    char match_char; // The character to match with if not a meta char
    enum meta_char meta; // The enum of a specific meta char
    struct reg_char *group; // List of reg_chars, used if meta char is group
} reg_char;

/**
 * This function attempts to match an individual character.
 * 
 * @param reg_char: Compiled regex character
 * @param c: Current char in line 
 */ 
bool match_reg(const struct reg_char reg, const char c)
{
    if (reg.is_meta) // If meta char, match based on rules of that char
    {
        switch (reg.meta)
        {
        case DOT:
            return true;
        case DIGIT:
            return isdigit(c) != 0;
        case NON_DIGIT:
            return isdigit(c) == 0;
        case LETTER:
            return isalpha(c) != 0;
        case NON_LETTER:
            return isalpha(c) == 0;
        case SPACE:
            return c == ' ' || c == '\t';
        case GROUP: // If any reg_char in a group matches, there is a match
        {
            int i = 0;
            while (reg.group[i].meta != END)
            {
                if (match_reg(reg.group[i], c))
                    return true;
                i++;
            }
            return false;
        }
        case COMP_GROUP: // If any char in a comp group matches, no match
        {
            int i = 0;
            while (reg.group[i].meta != END)
            {
                if (match_reg(reg.group[i], c))
                    return false;
                i++;
            }
            return true;
        }
        default: // Invalid characters dont match
            return false;
        }
    }
    else // this is a standard character match, if equal there is a match
    {
        return c == reg.match_char;
    }
}
/**
 * This function compiles the regex into an array of easy to use structs.
 * 
 * @param reg_pattern: String representation of regex
 * @param pattern: Compiled representation of regex
 */ 
void compile_reg(const char* reg_pattern, reg_char* pattern)
{
    int i = 0, j = 0;
    // For each char in regex, translate it to its compiled version
    while (reg_pattern[i] != '\n' && reg_pattern[i] != '\0')
    {
        switch (reg_pattern[i])
        {
        case '.':
        {
            pattern[j].is_meta = true;
            pattern[j].meta = DOT;
        }
        break;
        case '+':
        {
            pattern[j].is_meta = true;
            pattern[j].meta = PLUS;
        }
        break;
        case '*':
        {
            pattern[j].is_meta = true;
            pattern[j].meta = STAR;
        }
        break;
        case '?':
        {
            pattern[j].is_meta = true;
            pattern[j].meta = QMARK;
        }
        break;
        case '\\':
        {
            i += 1;
            switch (reg_pattern[i])
            {
            case 'd':
            {
                pattern[j].is_meta = true;
                pattern[j].meta = DIGIT;
            }
            break;
            case 'D':
            {
                pattern[j].is_meta = true;
                pattern[j].meta = NON_DIGIT;
            }
            break;
            case 'w':
            {
                pattern[j].is_meta = true;
                pattern[j].meta = LETTER;
            }
            break;
            case 'W':
            {
                pattern[j].is_meta = true;
                pattern[j].meta = NON_LETTER;
            }
            break;
            case 's':
            {
                pattern[j].is_meta = true;
                pattern[j].meta = SPACE;
            }
            break;
            default:
            {
                pattern[j].is_meta = false;
                pattern[j].match_char = '\\';
            }
            break;
            }
        }
        break;
        case '[':
        {
            pattern[j].is_meta = true;
            if (reg_pattern[i + 1] == '^')
            {
                pattern[j].meta = COMP_GROUP;
                i++;
            }
            else
            {
                pattern[j].meta = GROUP;
            }
            char reg_group[MAX_LENGTH];
            int k = 0;
            i++;
            // Characters within group get compiled into their own array
            while (reg_pattern[i] != ']')
            {
                reg_group[k] = reg_pattern[i];
                i++;
                k++;
            }
            compile_reg(reg_group, pattern[j].group);
        }
        break;
        default:
        {
            pattern[j].is_meta = false;
            pattern[j].match_char = reg_pattern[i];
        }
        break;
        }
        i++;
        j++;
    }
    pattern[j].is_meta = true;
    pattern[j].meta = END;
}
/**
 * This is a helper function used to determine if the regex after greedy operators, 
 * + and * is a match. If so those we might need to stop matching.
 * 
 * @param cur: Current character in line
 * @param pattern: Compiled regex pattern
 * @param pattern_idx: Index in compiled regex
 */ 
bool next_match(const char cur, const reg_char *pattern, int pattern_idx)
{
    reg_char cur_reg = pattern[pattern_idx];
    if (cur_reg.meta == END) // The end of regex is next, so stop matching
        return true;
    else if (match_reg(cur_reg, cur)) //The next reg_char is a match
        return true;
    // These are optional, check if we match with the next reg_char
    else if (pattern[pattern_idx + 1].meta == STAR ||
             pattern[pattern_idx + 1].meta == QMARK)
        return next_match(cur, pattern, pattern_idx + 2);
    else // No match
        return false;
}

/**
 * Main function for recursively matching regex.
 * 
 * @param line: Current line being read
 * @param pattern: Compiled regex pattern
 * @param line_idx: Index of line being looked at
 * @param pattern_idx: Index of regex being looked at
 * @param starting_pos: Position in line that we are matching from
 */ 
int match_p(const char *line, const reg_char *pattern, int line_idx,
            int pattern_idx, int starting_pos)
{
    // If end of regex was reached, we found a match
    if (pattern[pattern_idx].meta == END) 
        return starting_pos;
    // If end of line was reached without a match, no match
    if (line[line_idx] == '\0' || line[line_idx] == '\n')
        return -1;
    else // Attempt to match current regex character with current character
    {
        reg_char cur = pattern[pattern_idx];
        reg_char next = pattern[pattern_idx + 1];
        // For * or +, make as many matches as possible
        if (next.meta == STAR || next.meta == PLUS)
        {
            // If first char does not match for +, unseccessful match
            if (next.meta == PLUS)
            {
                if (!match_reg(cur, line[line_idx]))
                    return match_p(line, pattern, starting_pos + 1, 0,
                                   starting_pos + 1);
            }
            // Keep matching while we still can
            while (match_reg(pattern[pattern_idx], line[line_idx]) &&
                   line[line_idx] != '\0' && line[line_idx] != '\n')
            {
                // If there is a potential match with the next reg_char,
                // see if that can match the line recursively
                if (next_match(line[line_idx], pattern, pattern_idx + 2))
                {
                    if (match_p(line, pattern, line_idx, pattern_idx + 2, starting_pos) != -1)
                        return starting_pos;
                }
                line_idx += 1;
            }
            // After maximum matches are found, try to match next char
            return match_p(line, pattern, line_idx, pattern_idx + 2,
                           starting_pos);
        }
        else if (next.meta == QMARK)
        {
            // If this matches, determine if we should match or not
            if (match_reg(cur, line[line_idx]))
            {
                // See if this line will match if we chose to match with ?
                if (match_p(line, pattern, line_idx + 1, pattern_idx + 2,
                            starting_pos) != -1)
                    return starting_pos;
            }
            // We are not matching with ?, move onto next regex char
            return match_p(line, pattern, line_idx, pattern_idx + 2,
                           starting_pos);
        }
        else
        {
            // Standard match, if it does match look for next match
            if (match_reg(cur, line[line_idx])){
                return match_p(line, pattern, line_idx + 1, pattern_idx + 1,
                               starting_pos);
            }
            // It did not match, start over from starting_pos + 1
            return match_p(line, pattern, starting_pos + 1, 0, starting_pos + 1);
        }
    }
}

/**
 * Reads regex from file into string.
 * 
 * @param re_file: location of file
 * @param pattern: compiled regex pattern
 */ 
void read_regex(const char* re_file, reg_char* pattern)
{
    FILE *regex_file = fopen(re_file, "r");
    if (regex_file == NULL)
    {
        fprintf(stderr, "ERROR: regex file <%s> does not exist\n", re_file);
        exit(1);
    }
    char reg_pattern[MAX_LENGTH];
    fgets(reg_pattern, MAX_LENGTH, regex_file);
    fclose(regex_file);
    compile_reg(reg_pattern, pattern);
}

/* Read the file while matching each line to regex pattern */
void parse_file(const char *in_file, const char *re_file)
{
    reg_char pattern[MAX_LENGTH];
    read_regex(re_file, pattern);

    FILE *input_file = fopen(in_file, "r");
    if (input_file == NULL)
    {
        fprintf(stderr, "ERROR: input file <%s> does not exist\n", in_file);
        exit(1);
    }
    char line[MAX_LENGTH];
    while (fgets(line, MAX_LENGTH, input_file))
    {
        if (match_p(line, pattern, 0, 0, 0) != -1)
        {
            // Append newline char if the line does not have one
            if (line[strlen(line) - 1] == '\n')
            {
                printf("%s", line);
            }
            else
            {
                printf("%s\n", line);
            }
        }
    }
    fclose(input_file);
}

// int main(int argc, char **argv)
// {
//     setvbuf(stdout, NULL, _IONBF, 0);
//     if (argc != 3)
//     {
//         fprintf(stderr, "ERROR: Invalid arguments\n"
//                         "USAGE: a.out <regex-file> <input-file>\n");
//         return 1;
//     }
//     parse_file(argv[2], argv[1]);
// }
