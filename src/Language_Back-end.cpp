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

    FuncArray func_table;
    func_table.call_func_arr = (Func*)calloc(MAX_ASM_FUNCTIONS, sizeof(Func));
    Func* old_call_func_table_ptr = func_table.call_func_arr;
    func_table.def_func_arr = (Func*)calloc(MAX_ASM_FUNCTIONS, sizeof(Func));
    Func* old_def_func_table_ptr = func_table.def_func_arr;

    PrintBeginning(&asm_code);
    if (CodeGeneration(&asm_code, &var_table, &func_table, root) == CODE_GENERATION_ERROR)
        return CODE_GENERATION_ERROR;
    asm_code.str_arr = old_asm_code_ptr;
    var_table.var_arr = old_var_table_ptr;
    func_table.call_func_arr = old_call_func_table_ptr;
    func_table.def_func_arr = old_def_func_table_ptr;

    if (!IsFunctionCallsOK(&func_table)) {
        Error(__FUNCTION__, "Сall to an undefined function.");
        return CODE_GENERATION_ERROR;
    }

    if (asm_code.str_num > MAX_ASM_STRINGS) {
        Error(__FUNCTION__, "Too big code");
        return CODE_GENERATION_ERROR;
    }

    MakeAsmFile(asm_file, &asm_code);

    FreeCodeStrings(&asm_code);
    free(var_table.var_arr);
    free(func_table.call_func_arr);
    free(func_table.def_func_arr);
    fclose(asm_file);

    return 0;
}

int CodeGeneration(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node)
{
    static int cond_counter = 0;

    switch (node->type) {
        case NUM:
            PrintNumber(asm_code, node);
            break;
        case VAR:
            PrintVariable(asm_code, var_table, func_table, node);
            break;
        case STD_FUNC:
            PrintStdFunction(asm_code, var_table, func_table, node);
            break;
        case FUNC_DEF:
            if(strcmp(node->data.str, "main") == 0)
                PrintMainDefinition(asm_code, var_table, func_table, node);
            else
                PrintFunctionDefinition(asm_code, var_table, func_table, node);
            break;
        case FUNC:
            PrintFunction(asm_code, var_table, func_table, node);
            break;
        case OP:
            if (node->data.ch == '=')
                PrintAssignment(asm_code, var_table, func_table, node);
            else if (node->data.ch == ';')
                PrintSemicolon(asm_code, var_table, func_table, node);
            else
                PrintOperator(asm_code, var_table, func_table, node);
            break;
        case KEY_WORD:
            if (node->type == KEY_WORD && strcmp("if-else", node->data.str) == 0) {
                cond_counter++;
                PrintCondition(asm_code, var_table, func_table, node, cond_counter);
            }
            break;
//        default:
//            Неизвестное выражение
    }

    return 0;
}

int PrintBeginning(StringArray* asm_code)
{
    PrintString(asm_code, "\n;Beginning\n");
    PrintString_PushValue(asm_code, 0);
    PrintString(asm_code, "pop bx");
    PrintString(asm_code, "call main");
    PrintString(asm_code, "hlt");

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

int PrintVariable(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node)
{
    PrintString_PushVariable(asm_code, var_table, node->data.str);

    return 0;
}

int PrintStdFunction(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node)
{
    if (strcmp(node->data.str, "print") == 0) {
        CodeGeneration(asm_code, var_table, func_table, node->left);
        PrintString(asm_code, "out");
    } else if (strcmp(node->data.str, "sqrt") == 0) {
        CodeGeneration(asm_code, var_table, func_table, node->left);
        PrintString(asm_code, "sqrt");
    } else if (strcmp(node->data.str, "scan") == 0) {
        if (node->left->type != VAR)
            Error(__FUNCTION__, "Only variable can be argument of scan", node->str_num);

        PrintString(asm_code, "in");
        PrintString_PopInVariable(asm_code, var_table, node->left->data.str);
    }

    return 0;
}

int PrintFunctionDefinition(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node)
{
    func_table->def_func_arr[func_table->def_func_num].name = node->data.str;
    func_table->def_func_num++;

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, ":%s", node->data.str);
    asm_code->str_arr++;
    asm_code->str_num++;

    VarArray local_var_table;
    local_var_table.var_arr = (Var*)calloc(MAX_ASM_VARIABLES, sizeof(Var));
    Var* old_local_var_table_ptr = local_var_table.var_arr;

    CodeGeneration(asm_code, &local_var_table, func_table, node->left);

    local_var_table.var_arr = old_local_var_table_ptr;
    free(old_local_var_table_ptr);

    PrintString(asm_code, "\n;RET\n");
    PrintString(asm_code, "ret");

    return 0;
}

int PrintMainDefinition(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node)
{
  asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
  sprintf(asm_code->str_arr->ptr, ":%s", node->data.str);
  asm_code->str_arr++;
  asm_code->str_num++;

  CodeGeneration(asm_code, var_table, func_table, node->left);

  PrintString(asm_code, "\n;RET\n");
  PrintString(asm_code, "ret");

  return 0;
}

int PrintFunction(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node)
{
    func_table->call_func_arr[func_table->call_func_num].name = node->data.str;
    func_table->call_func_num++;

    PrintString_PushValue(asm_code, var_table->var_num);
    PrintString(asm_code, "pop bx");

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "call %s", node->data.str);
    asm_code->str_arr++;
    asm_code->str_num++;

    PrintString(asm_code, "push bx");
    PrintString_PushValue(asm_code, var_table->var_num);
    PrintString(asm_code, "sub");
    PrintString(asm_code, "pop bx");

    return 0;
}

int PrintAssignment(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node)
{
    CodeGeneration(asm_code, var_table, func_table, node->left);

    int var_adr = FindAddress(var_table, node->right->data.str);

    if (var_adr == NEW_VARIABLE) {
        var_table->var_arr[var_table->var_num].name = node->right->data.str;
        var_adr = var_table->var_num;
        var_table->var_num++;
    }

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "pop [%d + bx] ;%s", var_adr, var_table->var_arr[var_adr].name);
    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int PrintSemicolon(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node)
{
    asm_code->str_arr->ptr = (char*) calloc(MAX_COMMENT_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "\n; statement #%d\n", node->str_num);
    asm_code->str_arr++;
    asm_code->str_num++;

    if (node->left != nullptr && node->right != nullptr) {
        CodeGeneration(asm_code, var_table, func_table, node->left);
        CodeGeneration(asm_code, var_table, func_table, node->right);
    } else if (node->left != nullptr && node->right == nullptr) {
        CodeGeneration(asm_code, var_table, func_table, node->left);
    } else if (node->left == nullptr && node->right != nullptr) {
        CodeGeneration(asm_code, var_table, func_table, node->right);
    }

    return 0;
}

int PrintOperator(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node)
{
    CodeGeneration(asm_code, var_table, func_table, node->left);
    CodeGeneration(asm_code, var_table, func_table, node->right);

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

int PrintCondition(StringArray* asm_code, VarArray* var_table, FuncArray* func_table, Node* node, int cond_counter)
{
    Node* if_node = node->left;
    Node* else_node = node->right;

    //---------if(cond)----------------

    CodeGeneration(asm_code, var_table, func_table, if_node->left);

    PrintString_PushValue(asm_code, 0);

    PrintString_JmpAdr(asm_code, "je skip_if_", cond_counter);

    //---------act1----------------

    CodeGeneration(asm_code, var_table, func_table, if_node->right);

    if (else_node != nullptr)
        PrintString_JmpAdr(asm_code, "jmp skip_else_", cond_counter);

    PrintString_JmpAdr(asm_code, ":skip_if_", cond_counter);

    //---------act2----------------

    if (else_node != nullptr) {
        CodeGeneration(asm_code, var_table, func_table, else_node->right);

        PrintString_JmpAdr(asm_code, ":skip_else_", cond_counter);
    }

    return 0;
}

int FindAddress(VarArray* var_table, const char* var_name)
{
    if (var_table->var_num == 0)
        return NEW_VARIABLE;

    for(int i = 0; i < var_table->var_num; i++)
        if (strcmp(var_name, var_table->var_arr[i].name) == 0)
            return i;

    return NEW_VARIABLE;
}

bool IsFunctionCallsOK(FuncArray* func_table)
{
    int i;
    int j;

    for (i = 0; i < func_table->call_func_num; i++) {
        for (j = 0; j < func_table->def_func_num; j++)
            if (strcmp(func_table->call_func_arr[i].name,
                       func_table->def_func_arr[j].name) == 0)
                break;
        if (j == func_table->def_func_num)
            return false;
    }

    return true;
}

int FreeCodeStrings(StringArray* asm_code)
{
    for(int i = 0; i < asm_code->str_num; i++) {
        free(asm_code->str_arr[i].ptr);
    }

    free(asm_code->str_arr);

    return 0;
}

int PrintString_JmpAdr(StringArray* asm_code, const char* string, int counter)
{
    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "%s%d", string, counter);
    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int PrintString_PushValue(StringArray* asm_code, int value)
{
    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "push %d", value);
    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int PrintString_PushVariable(StringArray* asm_code, VarArray* var_table, const char* var_name)
{
    int var_adr = FindAddress(var_table, var_name);

    if (var_adr == NEW_VARIABLE) {
        char* message = (char*) calloc(ERROR_CODE_LEN, sizeof(char));
        sprintf(message, "\'%s\' was not declared", var_name);
        Error(__FUNCTION__, message);
        free(message);
        return CODE_GENERATION_ERROR;
    }

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "push [%d + bx] ;%s", var_adr, var_table->var_arr[var_adr].name);
    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int PrintString_PopInVariable(StringArray* asm_code, VarArray* var_table, const char* var_name)
{
    int var_adr = FindAddress(var_table, var_name);

    if (var_adr == NEW_VARIABLE) {
        char* message = (char*) calloc(ERROR_CODE_LEN, sizeof(char));
        sprintf(message, "\'%s\' was not declared", var_name);
        Error(__FUNCTION__, message);
        free(message);
        return CODE_GENERATION_ERROR;
    }

    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "pop [%d] ;%s", var_adr, var_table->var_arr[var_adr].name);
    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int PrintString(StringArray* asm_code, const char* string)
{
    asm_code->str_arr->ptr = (char*) calloc(MAX_CMD_LEN, sizeof(char));
    sprintf(asm_code->str_arr->ptr, "%s", string);
    asm_code->str_arr++;
    asm_code->str_num++;

    return 0;
}

int MakeAsmFile(FILE* asm_file, StringArray* asm_code)
{
    for(int i = 0; i < asm_code->str_num; i++)
        fprintf(asm_file, "%s\n", asm_code->str_arr[i].ptr);

    return 0;
}