#include "../include/Language.h"

int BackEnd(FileName filename, Node* root)
{
    FILE* asm_file = fopen(filename.output, "w");
    if (asm_file == nullptr) {
        printf("Error: Wrong output file");
        return CODE_GENERATION_ERROR;
    }

    StringArray asm_code;
    asm_code.str_arr = (String*) calloc(MAX_ASM_STRINGS, sizeof(String)); //TODO: определять количество строк
    String* old_asm_code_ptr = asm_code.str_arr;

    VarArray var_table;
    var_table.var_arr = (Var*)calloc(MAX_ASM_VARIABLES, sizeof(Var));
    Var* old_var_table_ptr = var_table.var_arr;

    if (CodeGeneration(&asm_code, &var_table, root) == CODE_GENERATION_ERROR)
        return CODE_GENERATION_ERROR;
    PrintHlt(&asm_code);
    asm_code.str_arr = old_asm_code_ptr;
    var_table.var_arr = old_var_table_ptr;

    if (asm_code.str_num > 1024)
        Error(__FUNCTION__, "Too big code");

    MakeAsmFile(asm_file, &asm_code);

    FreeCodeStrings(&asm_code);
    free(var_table.var_arr);
    fclose(asm_file);

    return 0;
}

int CodeGeneration(StringArray* asm_code, VarArray* var_table, Node* node)
{
    static int cond_counter = 0;

    if (node->type == NUM) {
        PrintNumber(asm_code, node);
    } else if (node->type == VAR) {
        if (PrintVariable(asm_code, var_table, node) == CODE_GENERATION_ERROR)
            return CODE_GENERATION_ERROR;
    } else if (node->type == STD_FUNC) {
        PrintFunction(asm_code, var_table, node);
    } else if (node->type == OP && node->data.ch == '=') {
        PrintAssignment(asm_code, var_table, node);
    } else if (node->type == OP && node->data.ch == ';') {
        PrintSemicolon(asm_code, var_table, node);
    } else if (node->type == OP) {
        PrintOperator(asm_code, var_table, node);
    } else if (node->type == KEY_WORD && strcmp("if-else", node->data.str) == 0) {
        cond_counter ++;
        PrintCondition(asm_code, var_table, node, cond_counter);
    }
//    else {
//        Неизвестное выражение
//    }

    return 0;
}

int PrintHlt(StringArray* asm_code)
{
    asm_code->str_arr->ptr = (char*) calloc(MAX_COMMENT_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "\nhlt\n");
    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int PrintNumber(StringArray* asm_code, Node* node)
{
    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "push %.8f", node->data.num);
    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int PrintVariable(StringArray* asm_code, VarArray* var_table, Node* node)
{
    int var_adr = FindAddress(var_table, node->data.str);

    if (var_adr == NEW_VARIABLE) {
        char* message = (char*) calloc(ERROR_CODE_LEN, sizeof(char));
        sprintf(message, "\'%s\' was not declared", node->data.str);
        Error(__FUNCTION__, message, node->str_num);
        free(message);
        return CODE_GENERATION_ERROR;
    }

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "push [%d] ;%s", var_adr, var_table->var_arr[var_adr].name);
    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int PrintFunction(StringArray* asm_code, VarArray* var_table, Node* node)
{
    if (strcmp(node->data.str, "print") == 0) {
        CodeGeneration(asm_code, var_table, node->left);

        asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
        sprintf(asm_code->str_arr->ptr, "out");
        asm_code->str_arr++;
        asm_code->str_num++;
    }

    return 0;
}

int PrintAssignment(StringArray* asm_code, VarArray* var_table, Node* node)
{
    CodeGeneration(asm_code, var_table, node->left);

    int var_adr = FindAddress(var_table, node->right->data.str);

    if (var_adr == NEW_VARIABLE) {
        var_table->var_arr[var_table->var_num].name = node->right->data.str;
        var_adr = var_table->var_num;
        var_table->var_num++;
    }

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "pop [%d] ;%s", var_adr, var_table->var_arr[var_adr].name);
    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int PrintSemicolon(StringArray* asm_code, VarArray* var_table, Node* node)
{
    asm_code->str_arr->ptr = (char*) calloc(MAX_COMMENT_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "\n; string %d\n", node->str_num);
    asm_code->str_arr++;
    asm_code->str_num++;

    if (node->left != nullptr && node->right != nullptr) {
        CodeGeneration(asm_code, var_table, node->left);
        CodeGeneration(asm_code, var_table, node->right);
    } else if (node->left != nullptr && node->right == nullptr) {
        CodeGeneration(asm_code, var_table, node->left);
    } else if (node->left == nullptr && node->right != nullptr) {
        CodeGeneration(asm_code, var_table, node->right);
    }

    return 0;
}

int PrintOperator(StringArray* asm_code, VarArray* var_table, Node* node)
{
    CodeGeneration(asm_code, var_table, node->left);
    CodeGeneration(asm_code, var_table, node->right);

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));

    switch (node->data.ch) {
        case '+':
            sprintf(asm_code->str_arr->ptr, "add");
            break;
        case '-':
            sprintf(asm_code->str_arr->ptr, "sub");
            break;
        case '*':
            sprintf(asm_code->str_arr->ptr, "mul");
            break;
        case '/':
            sprintf(asm_code->str_arr->ptr, "div");
            break;
    }

    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int PrintCondition(StringArray* asm_code, VarArray* var_table, Node* node, int cond_counter)
{
    Node* if_node = node->left;
    Node* else_node = node->right;

    //---------if(cond)----------------

    CodeGeneration(asm_code, var_table, if_node->left);

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "push 0");
    asm_code->str_arr++;
    asm_code->str_num++;

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "je skip_if_%d", cond_counter);
    asm_code->str_arr++;
    asm_code->str_num++;

    //---------act1----------------

    CodeGeneration(asm_code, var_table, if_node->right);

    if (else_node != nullptr) {
        asm_code->str_arr->ptr = (char *) calloc(MAX_CMD_LEN, sizeof(char));
        sprintf(asm_code->str_arr->ptr, "jmp skip_else_%d", cond_counter);
        asm_code->str_arr++;
        asm_code->str_num++;
    }

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, ":skip_if_%d", cond_counter);
    asm_code->str_arr++;
    asm_code->str_num++;

    //---------act2----------------

    if (else_node != nullptr) {
        CodeGeneration(asm_code, var_table, else_node->right);

        asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
        sprintf(asm_code->str_arr->ptr, ":skip_else_%d", cond_counter);
        asm_code->str_arr++;
        asm_code->str_num++;
    }

    return 0;
}

int FindAddress(VarArray* var_table, char* var_name)
{
    for(int i = 0; i < var_table->var_num; i++)
        if (strcmp(var_name, var_table->var_arr[i].name) == 0)
            return i;

    return NEW_VARIABLE;
}

int FreeCodeStrings(StringArray* asm_code)
{
    for(int i = 0; i < asm_code->str_num; i++) {
        free(asm_code->str_arr[i].ptr);
    }

    free(asm_code->str_arr);

    return 0;
}

int MakeAsmFile(FILE* asm_file, StringArray* asm_code)
{
    for(int i = 0; i < asm_code->str_num; i++)
        fprintf(asm_file, "%s\n", asm_code->str_arr[i].ptr);

    return 0;
}