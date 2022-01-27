#include "../include/Language.h"

int BackEnd(FileName filename, Node* root)
{
    FILE* asm_file = fopen(filename.output, "w");

    StringArray asm_code;
    asm_code.str = (String*) calloc(MAX_ASM_STRINGS, sizeof(String));  //TODO: определять количество строк
    String* old_asm_code_ptr = asm_code.str;

    VarTable var_table;
    var_table.var = (Var*)calloc(MAX_WORD_LEN, sizeof(Var));
    Var* old_var_table_ptr = var_table.var;

    CodeGeneration(&asm_code, &var_table, root);

    free(old_asm_code_ptr);
    free(old_var_table_ptr);

    return 0;
}

int CodeGeneration(StringArray* asm_code, VarTable* var_table, Node* node)
{
    String* old_asm_code_ptr = asm_code->str;
    Var* old_var_table_ptr = var_table->var;

    if (node->type == OP) {
        CodeGeneration(asm_code, var_table, node->left);
        CodeGeneration(asm_code, var_table, node->right);
        PrintOp(asm_code, node);
    }
    else if (node->type == VAR) {
        var_table->var->name = node->data.str;
        var_table->var->adr = var_table->var_num;
        var_table->var_num++;
    }

    asm_code->str = old_asm_code_ptr;
    var_table->var = old_var_table_ptr;

    return 0;
}
int PrintOp(StringArray* asm_code, Node* node)
{
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

    asm_code->str++; //TODO: проверить

    return 0;
}

int FreeCodeStrings()
{

}
