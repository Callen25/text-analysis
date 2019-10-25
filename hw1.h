#ifndef HW1_H_
#define HW1_H_

#define MAX_LENGTH 257

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

typedef struct reg_char
{
    bool is_meta;
    char match_char;
    enum meta_char meta;
    struct reg_char *group;
} reg_char;

bool match_reg(const struct reg_char reg, const char c);
void compile_reg(const char* reg_pattern, reg_char* pattern);
bool next_match(const char cur, const reg_char *pattern, int pattern_idx);
int match_p(const char *line, const reg_char *pattern, int line_idx,
            int pattern_idx, int starting_pos);
void read_regex(const char* re_file, reg_char* pattern);
void parse_file(const char *in_file, const char *re_file);

#endif // HW1_H_