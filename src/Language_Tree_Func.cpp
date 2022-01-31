#include "../include/Language.h"
#include "../include/Language_DSL.h"

Node* GetG(TokensArray* tokens_array)
{
    static int num_of_str = 0;
    num_of_str++;
    int str_num = num_of_str;

    Node* root = GetSt(tokens_array, str_num);   // Statement
    if(root == nullptr)
        return nullptr;

    Node* g_node = nullptr;                      // General
    if (IS_TOKEN_OP('\0')) {
        Require(tokens_array, '\0', __FUNCTION__, str_num);
    } else {
        g_node = GetG(tokens_array);
        if(g_node == nullptr)
            return nullptr;
    }

    root->right = g_node;
    root->str_num = str_num;

    TreeDump(root, "Into GetG");

    return root;
}

Node* GetSt(TokensArray* tokens_array, int str_num)
{

    Node* node = nullptr;

    if (!(IS_TOKEN_OP(';'))) {
        if (IS_NEXT_TOKEN_OP('='))
            node = GetAs(tokens_array, str_num);
        else if (IS_TOKEN_TYPE(WORD) && strcmp("if", (*tokens_array->ptr)->data.str) == 0)
            node = GetCond(tokens_array, str_num);
//        else if (IS_TOKEN_OP('{'))
//            node = GetBlk(tokens_array, str_num);
        else
            node = GetE(tokens_array, str_num);
        if (node == nullptr)
            return nullptr;
    }

    if (!(IS_TOKEN_OP(';'))) {                    // ;
        Error(__FUNCTION__, "You forgot \';\'", str_num);
        return nullptr;
    }
    Node* st_node = *tokens_array->ptr;
    tokens_array->ptr++;

    st_node->left = node;
    st_node->str_num = str_num;

    return st_node;
}

Node* GetBlk(TokensArray* tokens_array, int str_num)
{
    if (IS_TOKEN_OP('{'))
        Require(tokens_array, '{', __FUNCTION__, str_num);

    Node* root_node = GetSt(tokens_array, str_num);   // Statement

    Node* blk_node = nullptr;                         // Blk
    if (IS_TOKEN_OP('}')) {
        Require(tokens_array, '}', __FUNCTION__, str_num);
    } else {
        blk_node = GetBlk(tokens_array, str_num);
        if(blk_node == nullptr)
            return nullptr;
    }

    root_node->right = blk_node;
    root_node->str_num = str_num; //TODO: починить подсчёт строчек

    return root_node;
}

Node* GetCond(TokensArray* tokens_array, int str_num)
{
    Node* if_node = *tokens_array->ptr;
    if_node->type = KEY_WORD;
    tokens_array->ptr++;

    Require(tokens_array, '(', __FUNCTION__, str_num);
    Node* cond_node = GetE(tokens_array, str_num);
    Require(tokens_array, ')', __FUNCTION__, str_num);

    cond_node->left = GetBlk(tokens_array, str_num);

    if_node->left = cond_node;
    if (IS_TOKEN_TYPE(WORD) && strcmp("else", (*tokens_array->ptr)->data.str) == 0) {
        if_node->right = GetElse(tokens_array, str_num);
    } else {
        if_node->right = nullptr;
    }

    return if_node;
}

Node* GetIf(TokensArray* tokens_array, int str_num){
    Node* if_node = *tokens_array->ptr;
    if_node->type = KEY_WORD;
    tokens_array->ptr++;

    Require(tokens_array, '(', __FUNCTION__, str_num);
    Node* cond_node = GetE(tokens_array, str_num);
    Require(tokens_array, ')', __FUNCTION__, str_num);

    cond_node->left = GetBlk(tokens_array, str_num);

    return if_node;
}

Node* GetElse(TokensArray* tokens_array, int str_num)
{
    Node* else_node = *tokens_array->ptr;
    else_node->type = KEY_WORD;
    tokens_array->ptr++;

    Require(tokens_array, '(', __FUNCTION__, str_num);
    Require(tokens_array, ')', __FUNCTION__, str_num);

    else_node->left = GetBlk(tokens_array, str_num);
    else_node->right = nullptr;

    return else_node;
}

Node* GetAs(TokensArray* tokens_array, int str_num)
{
    Node* var_node = GetVar(tokens_array, str_num);
    if(var_node == nullptr)
        return nullptr;

    if (IS_TOKEN_OP('=')) {
        Node* op_node = *tokens_array->ptr;
        tokens_array->ptr++;

        Node* e_node = GetE(tokens_array, str_num);
        if(e_node == nullptr)
            return nullptr;

        op_node->left = e_node;
        op_node->right = var_node;
        var_node = op_node;
        var_node->str_num = str_num;
    }

    return var_node;
}

Node* GetE(TokensArray* tokens_array, int str_num)
{
    Node* t_node = GetT(tokens_array, str_num);
    if(t_node == nullptr)
        return nullptr;

    while (IS_TOKEN_OP('+') || IS_TOKEN_OP('-')) {
        Node* op_node = *tokens_array->ptr;
        tokens_array->ptr++;

        Node* t_node2 = GetT(tokens_array, str_num);
        if(t_node2 == nullptr)
            return nullptr;

        op_node->left = t_node;
        op_node->right = t_node2;
        t_node = op_node;
        t_node->str_num = str_num;
    }

    return t_node;
}

Node* GetT(TokensArray* tokens_array, int str_num)
{
    Node* p_node = GetP(tokens_array, str_num);
    if(p_node == nullptr)
        return nullptr;

    while (IS_TOKEN_OP('*') || IS_TOKEN_OP('/')) {
        Node* op_node = *tokens_array->ptr;
        tokens_array->ptr++;

        Node* p_node2 = GetP(tokens_array, str_num);
        if(p_node2 == nullptr)
            return nullptr;

        op_node->left = p_node;
        op_node->right = p_node2;
        p_node = op_node;
        p_node->str_num = str_num;
    }

    return p_node;
}

Node* GetP(TokensArray* tokens_array, int str_num)
{
    if (IS_TOKEN_OP('(')) {
        Require(tokens_array, '(', __FUNCTION__, str_num);
        Node* e_node = GetE(tokens_array, str_num);
        Require(tokens_array, ')', __FUNCTION__, str_num);
        return e_node;
    } else if (IS_TOKEN_TYPE(WORD)) {
        Node* node = nullptr;
        const int func_num = 1;
        const char* func_array[func_num] = {"print"};
        if (IsWordFromArray((*tokens_array->ptr)->data.str, func_array, func_num))
            node = GetFunc(tokens_array, str_num);
        else
            node = GetVar(tokens_array, str_num);
        return node;
    }
    else {
        Node* n_node = GetN(tokens_array, str_num);
        return n_node;
    }
}

Node* GetFunc(TokensArray* tokens_array, int str_num)
{
    Node* func_node = *tokens_array->ptr;
    func_node->type = FUNC;
    tokens_array->ptr++;


    Require(tokens_array, '(', __FUNCTION__, str_num);
    Node* arg_node = GetE(tokens_array, str_num);
    Require(tokens_array, ')', __FUNCTION__, str_num);

    func_node->left = arg_node;
    func_node->right = nullptr;
    func_node->str_num = str_num;

    return func_node;
}

Node* GetN(TokensArray* tokens_array, int str_num)
{
    if((*tokens_array->ptr)->type != NUM){
        Error(__FUNCTION__, "You forgot the number", str_num);
        return nullptr;
    }

    Node* n_node = *tokens_array->ptr;
    tokens_array->ptr++;
    n_node->str_num = str_num;

    return n_node;
}

Node* GetVar(TokensArray* tokens_array, int str_num)
{
    if((*tokens_array->ptr)->type != WORD){
        Error(__FUNCTION__, "You forgot the variable", str_num);
        return nullptr;
    }

    Node* var_node = *tokens_array->ptr;
    var_node->type = VAR;
    var_node->str_num = str_num;
    tokens_array->ptr++;

    return var_node;
}

bool IsWordFromArray(char* word, const char* key_words[], int words_num)
{
    for (int i = 0; i < words_num; i++)
        if (strcmp(word, key_words[i]) == 0)
            return true;

    return false;
}

int TreeDtor(Node* node)
{
    if (node->type == VAR || node->type == FUNC ||
        node->type == KEY_WORD || node->type == WORD)
        free(node->data.str);

    if (node->left != nullptr && node->right != nullptr) {
        TreeDtor(node->left);
        TreeDtor(node->right);
    } else if (node->left != nullptr && node->right == nullptr) {
        TreeDtor(node->left);
    } else if (node->left == nullptr && node->right != nullptr) {
        TreeDtor(node->right);
    }

    free(node);

    return 0;
}

//Node* CopyNode(Node* node)
//{
//    Node* new_node = (Node*)calloc(1, sizeof(Node));
//    memcpy(new_node, node, sizeof(Node));
//
//    if (node->type == NUM || node->type == OP || node->type == VAR) {
//        new_node->data = node->data;
//    } else if (node->type == VAR) {
//        new_node->data.str = (char*) calloc(MAX_FUNC_LEN, sizeof(char));
//        strcpy(new_node->data.str, node->data.str);
//    }
//
//    if (node->left != nullptr && node->right != nullptr) {
//        new_node->left = CopyNode(node->left);
//        new_node->right = CopyNode(node->right);
//    } else if (node->left != nullptr && node->right == nullptr) {
//        new_node->left = CopyNode(node->left);
//        new_node->right = nullptr;
//    }
//
//    return new_node;
//}
//
//int FreeChildNodes(Node* node)
//{
//    TreeDtor(node->left);
//    TreeDtor(node->right);
//    node->left  = nullptr;
//    node->right = nullptr;
//
//    return 0;
//}