#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

//---------Language_Structs---------

typedef union data_t {
    double num;
    char ch;
    char* str;
}data_t;

typedef struct Node {
    data_t data;
    int type = -1;
    int str_num = 0;

    Node* left = nullptr;
    Node* right = nullptr;
}Node;

typedef struct String {
    char* ptr = nullptr;
}String;

typedef struct TokensArray {
    Node** ptr = nullptr;
}TokensArray;

typedef struct FileName {
    char* input = nullptr;
    char* output = nullptr;
}FileName;

typedef struct StringArray {
    String* str_arr = nullptr;
    int str_num = 0;
}StringArray;

typedef struct Var {
    char* name = nullptr;
}Var;

typedef struct VarTable {
    Var* var_arr = nullptr;
    int var_num = 0;
}VarTable;

//---------Language_Enums---------

enum Constants {
    ROOT         = -1,
    NEW_VARIABLE = -1,

    MAX_WORD_LEN = 128,
    MAX_FUNC_LEN = 4,

    MAX_ASM_STRINGS   = 1024,
    MAX_ASM_VARIABLES = 1024,

    OP_CMD_LEN      = 3 + 1,
    MAX_CMD_LEN     = 5 + 33 + 1, //"push " + double[24 + 1 + 8] + \n TODO: посчитать как-то менее кринжово
    MAX_COMMENT_LEN = 4 + 12,

    ERROR_CODE_LEN  = 20 + MAX_WORD_LEN,
};

enum NodeTypes {
    NUM  = 1,
    VAR  = 2,
    OP   = 3,
    SEMICOLON = 4,
    FUNC = 5,
};

enum Errors {
    WRONG_INPUT_FILE      = 1,
    SYNTAX_ERROR          = 2,
    CODE_GENERATION_ERROR = 3,
};

//---------Language_Tree_Func---------

Node* GetG(TokensArray* tokens_array);

Node* GetE(TokensArray* tokens_array, int str_num);

Node* GetT(TokensArray* tokens_array, int str_num);

Node* GetP(TokensArray* tokens_array, int str_num);

Node* GetAs(TokensArray* tokens_array, int str_num);

Node* GetFunc(TokensArray* tokens_array);

Node* GetN(TokensArray* tokens_array, int str_num);

Node* GetVar(TokensArray* tokens_array, int str_num);

Node* CopyNode(Node* node);

int FreeChildNodes(Node* node);

int TreeDtor(Node* node);

//---------Language_Lexical_Analysis_Func---------

int LexicalAnalysis(String* string, TokensArray* tokens_array);

Node* GetNumber(String* string);

Node* GetWord(String* string);

Node* GetCharacter(String* string);

Node* CreateNode(int type, data_t data, Node* left, Node* right);

bool IsCharacterFromArray(char ch, const char* array);

int PrintNodes(TokensArray* tokens_array);

//---------Language_Errors_Func--------------

int Require(TokensArray* tokens_array, const char sign);

int Error(const char* function, const char* message);

//---------Language_File_&Text_Func--------------

int GetFileNames (FileName *filename, int argc, char* argv[]);

size_t GetSizeOfFile (FILE* fp);

int ScanProgram(char* filename, String* string);

int DeleteSpaces(char* str);

//---------Language_Tree_Dump--------------

int TreeDump(Node* root, const char* current_function);

int MakeGraphDumpTxt(Node* root, const char* current_function, int dump_cnt);

int MakePngFromTxt(int dump_cnt);

int RecursiveTreeDump(Node* node, FILE* dump_fp, int parents_num, bool left_node);

char* DumpFileName(int dump_cnt, const char* format);

//---------Language_Back-end--------------

int BackEnd(FileName filename, Node* root);

int CodeGeneration(StringArray* asm_code, VarTable* var_table, Node* node);

int PrintNumber(StringArray* asm_code, Node* node);

int PrintSemicolon(StringArray* asm_code, VarTable* var_table, Node* node);

int PrintOperator(StringArray* asm_code, VarTable* var_table, Node* node);

int PrintAssignment(StringArray* asm_code, VarTable* var_table, Node* node);

int PrintVariable(StringArray* asm_code, VarTable* var_table, Node* node);

int FreeCodeStrings(StringArray* asm_code);

int MakeAsmFile(FILE* asm_file, StringArray* asm_code);

int FindAddress(VarTable* var_table, char* var_name);