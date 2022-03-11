#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>


//---------Language_Structs---------

typedef struct FileName {
    char* input = nullptr;
    char* output = nullptr;
}FileName;

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

typedef struct TokensArray {
    Node** ptr = nullptr;
}TokensArray;

typedef struct String {
    char* ptr = nullptr;
}String;

typedef struct StringArray {
    String* str_arr = nullptr;
    int str_num = 0;
}StringArray;

typedef struct Var {
    char* name = nullptr;
}Var;

typedef struct VarArray {
    Var* var_arr = nullptr;
    int var_num = 0;
}VarArray;

typedef struct Func {
    char* name = nullptr;
}Func;

typedef struct FuncArray {
    Func* call_func_arr = nullptr;
    int call_func_num = 0;

    Func* def_func_arr = nullptr;
    int def_func_num = 0;
}FuncArray;


//---------Language_Constants---------

enum Constants {
    ROOT         = -1,
    NEW_VARIABLE = -1,

    MAX_WORD_LEN = 128,
    MAX_NUM_LEN  = 128,

    MAX_ASM_STRINGS   = 4096,
    MAX_ASM_VARIABLES = 64,
    MAX_ASM_FUNCTIONS = 512,

    MAX_CMD_LEN     = 10 + MAX_NUM_LEN + MAX_WORD_LEN, //"push [MAX_NUM_LEN] ;MAX_WORD_LEN\n"
    MAX_COMMENT_LEN = 32, //точно хватит

    ERROR_CODE_LEN  = 128 + MAX_WORD_LEN,
};

enum NodeTypes {
    WORD      = 0,
    NUM       = 1,
    VAR       = 2,
    OP        = 3,
    STD_FUNC  = 4,
    FUNC      = 5,
    FUNC_DEF  = 6,
    KEY_WORD  = 7,
};

enum Errors {
    WRONG_INPUT_FILE      = 1,
    SYNTAX_ERROR          = 2,
    CODE_GENERATION_ERROR = 3,
};

//---------Language_Tree_Func---------

Node* GetG(TokensArray* tokens_array);

Node* GetSt(TokensArray* tokens_array, int str_num);

Node* GetBlk(TokensArray* tokens_array, int str_num);

Node* GetCond(TokensArray* tokens_array, int str_num);

Node* GetIf(TokensArray* tokens_array, int str_num);

Node* GetElse(TokensArray* tokens_array, int str_num);

Node* GetFuncDef(TokensArray* tokens_array, int str_num);

Node* GetAs(TokensArray* tokens_array, int str_num);

Node* GetE(TokensArray* tokens_array, int str_num);

Node* GetT(TokensArray* tokens_array, int str_num);

Node* GetP(TokensArray* tokens_array, int str_num);

Node* GetStdFunc(TokensArray* tokens_array, int str_num);

Node* GetFunc(TokensArray* tokens_array, int str_num);

Node* GetN(TokensArray* tokens_array, int str_num);

Node* GetVar(TokensArray* tokens_array, int str_num);

bool IsWordFromArray(char* word, const char* key_words[], int words_num);

int TreeDtor(Node* node);

Node* CopyNode(Node* node);

int FreeChildNodes(Node* node);

//---------Language_Lexical_Analysis_Func---------

int LexicalAnalysis(String* string, TokensArray* tokens_array);

Node* GetNumber(String* string);

Node* GetWord(String* string);

Node* GetCharacter(String* string);

Node* CreateNode(int type, data_t data, Node* left, Node* right);

bool IsCharacterFromArray(char ch, const char* array);

int PrintNodes(TokensArray* tokens_array);

//---------Language_Back-end--------------

int BackEnd(FileName filename, Node* root);

int CodeGeneration(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node);

int PrintBeginning(StringArray* asm_code);

int PrintNumber(StringArray* asm_code, Node* node);

int PrintVariable(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node);

int PrintStdFunction(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node);

int PrintFunctionDefinition(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node);

int PrintMainDefinition(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node);

int PrintFunction(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node);

int PrintAssignment(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node);

int PrintSemicolon(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node);

int PrintOperator(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node);

int PrintCondition(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node, int cond_counter);

int FindAddress(VarArray* var_table, const char* var_name);

bool FunctionWasCalled(FuncArray* func_table, char* func_name);

bool IsFunctionCallsOK(FuncArray* func_table);

int FreeCodeStrings(StringArray* asm_code);

int PrintString_JmpAdr(StringArray* asm_code, const char* string, int counter);

int PrintString(StringArray* asm_code, const char* string);

int PrintString_PushValue(StringArray* asm_code, int value);

int PrintString_PushVariable(StringArray* asm_code, VarArray* var_table, const char* var_name);

int PrintString_PopInVariable(StringArray* asm_code, VarArray* var_table, const char* var_name);

int MakeAsmFile(FILE* asm_file, StringArray* asm_code);

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

//---------Language_Errors_Func--------------

int Require(TokensArray* tokens_array, const char sign, const char* function, int str_num = -1);

int Error(const char* function, const char* message, int str_num = -1);