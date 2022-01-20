#include "../include/Language.h"
#include "../include/Language_DSL.h"

Node* GetG(TokensArray* tokens_array)
{
    Node* root = nullptr;

    if ((*(tokens_array->ptr + 1))->data.ch == '=')
        root = GetAs(tokens_array);
    else
        root = GetE(tokens_array);

    CHECK_NODE(root);
    Require(tokens_array, '\0');

    return root;
}

Node* GetE(TokensArray* tokens_array)
{
    Node* t_node = GetT(tokens_array);
    CHECK_NODE(t_node);

    while (IS_CHAR_TOKEN('+') || IS_CHAR_TOKEN('-')) {
        Node* op_node = *tokens_array->ptr;
        tokens_array->ptr++;

        Node* t_node2 = GetT(tokens_array);
        CHECK_NODE(t_node2);

        op_node->left = t_node;
        op_node->right = t_node2;
        t_node = op_node;
    }

    return t_node;
}

Node* GetT(TokensArray* tokens_array)
{
    Node* p_node = GetP(tokens_array);
    CHECK_NODE(p_node);

    while (IS_CHAR_TOKEN('*') || IS_CHAR_TOKEN('/')) {
        Node* op_node = *tokens_array->ptr;
        tokens_array->ptr++;

        Node* p_node2 = GetP(tokens_array);
        CHECK_NODE(p_node2);

        op_node->left = p_node;
        op_node->right = p_node2;
        p_node = op_node;
    }

    return p_node;
}

Node* GetP(TokensArray* tokens_array)
{
    if (IS_CHAR_TOKEN('(')) {
        Require(tokens_array, '(');
        Node* e_node = GetE(tokens_array);
        CHECK_NODE(e_node);
        Require(tokens_array, ')');
        return e_node;
    } else if ((*tokens_array->ptr)->type == VAR) {
        Node *var_node = GetVar(tokens_array);
        CHECK_NODE(var_node);
        return var_node;
    }
    else {
        Node* n_node = GetN(tokens_array);
        CHECK_NODE(n_node);
        return n_node;
    }
}

Node* GetAs(TokensArray* tokens_array)
{
    Node* var_node = GetVar(tokens_array);
    CHECK_NODE(var_node);

    if (IS_CHAR_TOKEN('=')) {
        Node* op_node = *tokens_array->ptr;
        tokens_array->ptr++;

        Node* e_node = GetE(tokens_array);
        CHECK_NODE(e_node);

        op_node->left = e_node;
        op_node->right = var_node;
        var_node = op_node;
    } else {
        SyntaxError(__FUNCTION__, "You forhot \"=\" in assignment");
    }

    return var_node;
}

//Node* GetFunc(TokensArray* tokens_array)
//{
//    char func_name[MAX_FUNC_LEN] = "";
//    int len = 0;
//
//    while (*s != '(') {
//        func_name[len] = *s;
//        s++;
//        len++;
//
//        if(*s == '\0'){
//            SyntaxError(__FUNCTION__, "That's not a function");
//            return nullptr;
//        }
//    }
//
//    if(len >= MAX_FUNC_LEN){
//        SyntaxError(__FUNCTION__, "Too long function name");
//        return nullptr;
//    }
//
//    func_name[len + 1] = '\0';
//
//    s++;
//    Node* argument = GetE();
//    CHECK_NODE(argument);
//    Require(')');
//
//    return CreateNode(FUNC, func_name, argument, nullptr);
//}

Node* GetN(TokensArray* tokens_array)
{
    if((*tokens_array->ptr)->type != NUM){
        SyntaxError(__FUNCTION__, "You forgot the number somewhere");
        return nullptr;
    }

    Node* n_node = *tokens_array->ptr;
    tokens_array->ptr++;

    return n_node;
}

Node* GetVar(TokensArray* tokens_array)
{
    if((*tokens_array->ptr)->type != VAR){
        SyntaxError(__FUNCTION__, "You forgot the variable somewhere");
        return nullptr;
    }

    Node* n_node = *tokens_array->ptr;
    tokens_array->ptr++;

    return n_node;
}

Node* CopyNode(Node* node)
{
    Node* new_node = (Node*)calloc(1, sizeof(Node));
    memcpy(new_node, node, sizeof(Node));

    if (node->type == NUM || node->type == OP || node->type == VAR) {
        new_node->data = node->data;
    } else if (node->type == VAR) {
        new_node->data.str = (char*) calloc(MAX_FUNC_LEN, sizeof(char));
        strcpy(new_node->data.str, node->data.str);
    }

    if (node->left != nullptr && node->right != nullptr) {
        new_node->left = CopyNode(node->left);
        new_node->right = CopyNode(node->right);
    } else if (node->left != nullptr && node->right == nullptr) {
        new_node->left = CopyNode(node->left);
        new_node->right = nullptr;
    }

    return new_node;
}

int FreeChildNodes(Node* node)
{
    TreeDtor(node->left);
    TreeDtor(node->right);
    node->left  = nullptr;
    node->right = nullptr;

    return 0;
}

int TreeDtor(Node* node)
{
    if (node->type == VAR)
        free(node->data.str);

    if (node->left != nullptr && node->right != nullptr) {
        TreeDtor(node->left);
        TreeDtor(node->right);
    }
    else if (node->left != nullptr && node->right == nullptr) {
        TreeDtor(node->left);
    }

    free(node);

    return 0;
}