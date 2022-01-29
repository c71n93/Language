#include "../include/Language.h"
#include "../include/Language_DSL.h"

Node* GetG(TokensArray* tokens_array)
{
    static int num_of_str = 0;
    num_of_str++;
    int str_num = num_of_str;

    Node* st_node = GetSt(tokens_array, str_num);   // Statement

    if (!(IS_CHAR_TOKEN(';'))) {                    // ;
        Error(__FUNCTION__, "You forgot \';\'", str_num);
        return nullptr;
    }
    Node* root = *tokens_array->ptr;
    tokens_array->ptr++;

    Node* g_node = nullptr;                         // General
    if (!(IS_CHAR_TOKEN('\0'))) {
        g_node = GetG(tokens_array);
        if(g_node == nullptr)
            return nullptr;
    } else {
        Require(tokens_array, '\0', str_num);
    }

    root->left = st_node;
    root->right = g_node;

    root->str_num = str_num;

    TreeDump(root, "Into GetG");

    return root;
}

Node* GetSt(TokensArray* tokens_array, int str_num)
{
    Node* node = nullptr;

    if (IS_CHAR_NEXT_TOKEN('='))
        node = GetAs(tokens_array, str_num);
    else
        node = GetE(tokens_array, str_num);

    if (node == nullptr)
        return nullptr;

    return node;
}

Node* GetAs(TokensArray* tokens_array, int str_num)
{
    Node* var_node = GetVar(tokens_array, str_num);
    if(var_node == nullptr)
        return nullptr;

    if (IS_CHAR_TOKEN('=')) {
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

    while (IS_CHAR_TOKEN('+') || IS_CHAR_TOKEN('-')) {
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

    while (IS_CHAR_TOKEN('*') || IS_CHAR_TOKEN('/')) {
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
    if (IS_CHAR_TOKEN('(')) {
        Require(tokens_array, '(', str_num);
        Node* e_node = GetE(tokens_array, str_num);
        Require(tokens_array, ')', str_num);
        return e_node;
    } else if ((*tokens_array->ptr)->type == VAR) {
        Node* node = nullptr;
        if (IsFunction((*tokens_array->ptr)->data.str))
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
    tokens_array->ptr++;
    func_node->type = FUNC;

    Require(tokens_array, '(', str_num);

    Node* arg_node = nullptr;
    if ((*tokens_array->ptr)->type == NUM) {
        arg_node = GetN(tokens_array, str_num);
    } else if ((*tokens_array->ptr)->type == VAR) {
        arg_node = GetE(tokens_array, str_num);
    } else {
        Error(__FUNCTION__, "Wrong argument in function", str_num);
        return nullptr;
    }

    Require(tokens_array, ')', str_num);

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
    if((*tokens_array->ptr)->type != VAR){
        Error(__FUNCTION__, "You forgot the variable", str_num);
        return nullptr;
    }

    Node* var_node = *tokens_array->ptr;
    var_node->str_num = str_num;
    tokens_array->ptr++;

    return var_node;
}

bool IsFunction(char* name)
{
    const int FUNC_NUM = 1;
    const char* FUNC_NAMES[FUNC_NUM] = {"print"};

    for (int i = 0; i < FUNC_NUM; i++)
        if (strcmp(name, FUNC_NAMES[i]) == 0)
            return true;

    return false;
}

int TreeDtor(Node* node)
{
    if (node->type == VAR || node->type == FUNC)
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