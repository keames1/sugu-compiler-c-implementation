#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifndef lexical_analysis

typedef enum // Token type signifier constants.
{
    tt_integer,
    tt_float,
    tt_string,
    tt_char,
    tt_kwd,

    EXPR_TKN_RANGE_START,
    tt_lparen,
    tt_rparen,
    tt_plus,
    tt_minus,
    tt_asterisk,
    tt_dbl_asterisk,
    tt_fwd_slash,
    tt_double_equ,
    tt_percent,
    tt_andpersand,
    tt_pipe_sym,
    tt_backslash,
    tt_tilde,
    tt_carrot,
    tt_not_equ,
    tt_less_than,
    tt_greater_than,
    tt_less_than_equ,
    tt_greater_than_equ,
    tt_left_shift,
    tt_right_shift,
    EXPR_TKN_RANGE_END,

    tt_dbl_fwd_slash,
    tt_equ,
    tt_illegal_token,
    tt_end_of_file,
    tt_identifier,
    tt_member_field_ref,
    tt_semicolon,
    tt_colon,
    tt_octothorpe,
    tt_newline,
    tt_dollar_sign,
    tt_comma,

    // Token type any signifies a compare mode which ignores the tokens type in a comparison.
    TT_ANY,
    // Token type none signifies a token whos comparison with another token always signifies false.
    TT_NONE,

} tokT;

// Keyword operators
char KWD_AND[]                 =              "and";
char KWD_OR[]                  =              "or";
char KWD_NOT[]                 =              "not";
char KWD_IF[]                  =              "if";
char KWD_ELIF[]                =              "elif";
char KWD_ELSE[]                =              "else";
     
// Data type keywords
char KWD_I8[]                  =              "i8";
char KWD_I16[]                 =              "i16";
char KWD_I32[]                 =              "i32";
char KWD_I64[]                 =              "i64";
char KWD_IEXP[]                =              "Iexp";
char KWD_U8[]                  =              "u8";
char KWD_U16[]                 =              "u16";
char KWD_U32[]                 =              "u32";
char KWD_U64[]                 =              "u64";
char KWD_F32[]                 =              "f32";
char KWD_F64[]                 =              "f64";
char KWD_FEXP[]                =              "Fexp";
char KWD_CHAR[]                =              "char";
char KWD_BOOL[]                =              "bool";
char KWD_REF[]                 =              "ref";
char KWD_STR[]                 =              "str";
char KWD_STRING[]              =              "String";
     
// nonruntime related keywords
char KWD_NONRUNTIME[]          =              ".nonruntime";
char KWD_END[]                 =              ".end";
char KWD_AVAILABLE[]           =              ".available";
char KWD_UNAVAILABLE[]         =              ".unavailable";
     
// Flow control and block related keywords
char KWD_WHILE[]               =              "while";
char KWD_FOR[]                 =              "for";
char KWD_FUN[]                 =              "fun";
char KWD_STRUCT[]              =              "struct";
char KWD_ENUM[]                =              "enum";

// Arrays of string pointers used to iteratively search for a string among all the above keywords.
char* const KS_ALL_KWDS[] = {

  KWD_AND, KWD_OR, KWD_NOT, KWD_IF, KWD_ELIF, KWD_ELSE,

  KWD_I8, KWD_I16, KWD_I32, KWD_I64, KWD_IEXP, KWD_F32, KWD_F64, KWD_FEXP, KWD_CHAR, KWD_BOOL, KWD_REF,
  KWD_U8, KWD_U16, KWD_U32, KWD_U64, KWD_STR, KWD_STRING,

  KWD_NONRUNTIME, KWD_END, KWD_AVAILABLE, KWD_UNAVAILABLE,

  KWD_WHILE, KWD_FOR, KWD_FUN, KWD_STRUCT, KWD_ENUM,
  
  NULL // The null pointer marks the end of the array. This allows the array size to be unknown
};

typedef union // A union that stores the value of a token.
{
    int i;
    double f;
    char* s;
    char c;

} valUnion;

// Data type signifier constants for the tokVal struct representing the data type of the union member of the
// struct.
typedef enum
{
    DT_INT = (int)TT_NONE + 1,
    DT_FLOAT,
    DT_STRING,
    DT_CHAR,

} valType;

typedef struct // The value field of the Token struct is represented with this guy.
{
    valType type;
    valUnion* value;

} TokVal;

// A token contains the basic identifying data used by the parser and some metadata used for tracing syntax
// and lexical errors.
typedef struct
{
    tokT tokType;
    TokVal* tokVal;
    int startIdx;
    int endIdx;
    char* fileName;

} Token;
/*
  Constructors for the Token struct. It populates a Token instance at a pointer so the caller has control
  over how and where the actual struct is allocated in memory. I made it return an integer so an error code
  could be returned if invalid parameters are passed to the constructor.
*/
int Token_new (
                 Token* self,
                 tokT tokType,
                 TokVal* tokVal,
                 int startIdx,
                 int endIdx,
                 char* fileName
              )
{
    self->tokType = tokType;
    self->tokVal = tokVal;
    self->startIdx = startIdx;
    self->endIdx = endIdx;
    self->fileName = fileName;
    
    return 0;

}


int Token_newConcise (Token* self, tokT tokType, TokVal* tokVal)
{
    return Token_new (
                       self,
                       tokType,
                       tokVal,
                       -1,
                       -1,
                       NULL
                     );

}

bool isKwd(char aWord[])
{
    for (int i = 0; KS_ALL_KWDS[i] != NULL; i++)
    {
        if (strcmp(KS_ALL_KWDS[i], aWord) == 0) return true;
    }
    return false;
}

typedef struct
{
    char* srcCode;
    int idx;
    char cuChar;
    char* fileName;

} Lexer;

int const LEXER_IDX_SENTINAL_VALUE = -2048;

void Lexer_toNext (Lexer* self)
{
    self->idx++;
    if (self->cuChar != '\0')
    {
        self->cuChar = self->srcCode[self->idx];
    } else
    {
        self->idx = LEXER_IDX_SENTINAL_VALUE;
        self->cuChar = '\0';
    }

}

int Lexer_new (Lexer* self, char* srcCode, char* fileName)
{
    self->srcCode = srcCode;
    self->idx = -1;
    self->cuChar = '\xff';
    self->fileName = fileName;
    
    Lexer_toNext(self);
    return 0;

}

#define lexical_analysis
#endif
