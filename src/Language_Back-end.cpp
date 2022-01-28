#include "../include/Language.h"

int BackEnd(FileName filename, Node* root)
{
    FILE* asm_file = fopen(filename.output, "w");

    StringArray asm_code;
    asm_code.str = (String*) calloc(MAX_ASM_STRINGS, sizeof(String)); //TODO: определять количество строк
    String* old_asm_code_ptr = asm_code.str;

    VarTable var_table;
    var_table.var = (Var*)calloc(MAX_WORD_LEN, sizeof(Var));
    Var* old_var_table_ptr = var_table.var;


    CodeGeneration(&asm_code, &var_table, root);
    asm_code.str = old_asm_code_ptr;
    var_table.var = old_var_table_ptr;

    MakeAsmFile(asm_file, &asm_code);

    FreeCodeStrings(&asm_code);
    free(var_table.var);
    fclose(asm_file);

    return 0;
}

int CodeGeneration(StringArray* asm_code, VarTable* var_table, Node* node)
{
    if (node->type == NUM) {
        PrintNumber(asm_code, var_table, node);
        asm_code->str++;
        asm_code->str_num++;
    } else if (node->type == VAR) {
        PrintVariable(asm_code, var_table, node);
        asm_code->str++;
        asm_code->str_num++;
    } else if (node->type == OP && node->data.ch == '=') {
        PrintAssignment(asm_code, var_table, node);
        asm_code->str++;
        asm_code->str_num++;
    } else if (node->type == OP) {
        PrintOperator(asm_code, var_table, node);
        asm_code->str++;
        asm_code->str_num++;
    }

    return 0;
}

int PrintNumber(StringArray* asm_code, VarTable* var_table, Node* node)
{
    asm_code->str->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str->ptr, "push %.8f", node->data.num);
}

int PrintOperator(StringArray* asm_code, VarTable* var_table, Node* node)
{
    CodeGeneration(asm_code, var_table, node->left);
    CodeGeneration(asm_code, var_table, node->right);

    asm_code->str->ptr = (char*) calloc(OP_CMD_LEN, sizeof(char));

    switch (node->data.ch) {
        case '+':
            sprintf(asm_code->str->ptr, "add");
            break;
        case '-':
            sprintf(asm_code->str->ptr, "sub");
            break;
        case '*':
            sprintf(asm_code->str->ptr, "mul");
            break;
        case '/':
            sprintf(asm_code->str->ptr, "div");
            break;
    }

    return 0;
}

int PrintAssignment(StringArray* asm_code, VarTable* var_table, Node* node)
{
    int var_adr = FindAddress(var_table, node->right->data.str);

    if (var_adr == NEW_VARIABLE) {
        var_table->var->name = node->right->data.str;
        var_adr = var_table->var_num;
        var_table->var_num++;
    }

    CodeGeneration(asm_code, var_table, node->left);

    asm_code->str->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str->ptr, "pop [%d]", var_adr);

    return 0;
}

int PrintVariable(StringArray* asm_code, VarTable* var_table, Node* node)
{
    int var_adr = FindAddress(var_table, node->data.str);

    if (var_adr == NEW_VARIABLE) {
        char* message = (char*) calloc(ERROR_CODE_LEN, sizeof(char));
        sprintf(message, "\'%s\' was not declared", node->data.str);
        Error(__FUNCTION__, message);
        free(message);
        return CODE_GENERATION_ERROR;
    }

    asm_code->str->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));

    sprintf(asm_code->str->ptr, "push [%d]", var_adr);
}

int FreeCodeStrings(StringArray* asm_code)
{
    for(int i = 0; i < asm_code->str_num; i++) {
        free(asm_code->str[i].ptr);
    }

    free(asm_code->str);

    return 0;
}

int MakeAsmFile(FILE* asm_file, StringArray* asm_code)
{
    for(int i = 0; i < asm_code->str_num; i++)
        fprintf(asm_file, "%s\n", asm_code->str[i].ptr);

    return 0;
}

int FindAddress(VarTable* var_table, char* var_name)
{
    for(int i = 0; i < var_table->var_num; i++)
        if (strcmp(var_name, var_table->var->name) == 0)
            return i;

    return NEW_VARIABLE;
}