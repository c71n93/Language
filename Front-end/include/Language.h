#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

typedef union data_t {
    double num;
    char ch;
    char* str;
}data_t;

typedef struct Node {
    data_t data;
    int type = -1;

    Node* left = nullptr;
    Node* right = nullptr;

}Node;

typedef struct String {
    char* ptr = nullptr;
}String;

typedef struct TokensArray {
    Node** ptr = nullptr;
}TokensArray;

enum Constants {
    ROOT            = -1,
    MAX_PROGRAM_LEN = 1024,
    MAX_WORD_LEN    = 128,
    UTF_LETTER_SIZE = 2,
    MAX_FUNC_LEN    = 4,
};

enum NodeTypes {
    BRACKET = -1,
    NUM  = 1,
    VAR  = 2,
    OP   = 3,
    FUNC = 4,
};

enum Errors {
    SYNTAX_ERROR = 1,
};

#define CHECK_ROOT(root, string)   \
do{                                \
    if((root) == nullptr) {        \
        free(string);              \
        return -1;                 \
    }                              \
} while(0)

#define CHECK_NODE(node)    \
do{                         \
    if((node) == nullptr)   \
        return nullptr;     \
} while(0)

//---------Language_Tree_Func---------

Node* GetG(const char* string);

Node* GetE();

Node* GetT();

Node* GetP();

Node* GetPow();

Node* GetFunc();

Node* GetN();

Node* GetId();

Node* CreateNode(int type, data_t data, Node* left, Node* right);

Node* CopyNode(Node* node);

int FreeChildNodes(Node* node);

int TreeDtor(Node* node);

//---------Language_Lexical_Analysis_Func---------

int LexicalAnalysis(String* string, TokensArray* tokens_array);

Node* GetNumber(String* string);

Node* GetWord(String* string);

Node* GetOperator(String* string);

Node* GetBracket(String* string);

//---------Language_Errors_Func--------------

int Require(const char sign);

int SyntaxError(const char* function, const char* message);

//---------Language_File_&Text_Func--------------

int ScanProgram(String* string);

int ScanString(char* array);

int DeleteSpaces(char* str);

//---------Language_Tree_Dump--------------

int TreeDump(Node* root, const char* current_function);

int MakeGraphDumpTxt(Node* root, const char* current_function, int dump_cnt);

int MakePngFromTxt(int dump_cnt);

int RecursiveTreeDump(Node* node, FILE* dump_fp, int parents_num, bool left_node);

char* DumpFileName(int dump_cnt, const char* format);