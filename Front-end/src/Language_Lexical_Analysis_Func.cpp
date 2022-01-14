#include "../include/Language.h"

int LexicalAnalysis(String* string, TokensArray* tokens_array)
{
    tokens_array->ptr = (Node**) calloc(MAX_PROGRAM_LEN, sizeof(Node*));
    Node** old_tokens_array_ptr = tokens_array->ptr;

    while(true) {
        if (isdigit(*string->ptr)) {
            *tokens_array->ptr = GetNumber(string);
        } else if (isalpha(*string->ptr)) {
            *tokens_array->ptr = GetWord(string);
        } else if (*string->ptr == '+' || *string->ptr == '-' ||
                   *string->ptr == '*' || *string->ptr == '/' ||
                   *string->ptr == '(' || *string->ptr == ')'){
            *tokens_array->ptr = GetCharacter(string);
        } else if (*string->ptr == '\0') {
            *tokens_array->ptr = GetCharacter(string);
            break;
        } else {
            SyntaxError(__FUNCTION__, "Wrong character");
            break;
        }

        tokens_array->ptr++;
    }

    tokens_array->ptr = old_tokens_array_ptr;

    return 0;
}

Node* GetNumber(String* string)
{
    data_t data;
    double power;

    for (data.num = 0.0; isdigit(*string->ptr); string->ptr++)
        data.num = 10.0 * data.num + *string->ptr - '0';
    if (*string->ptr == '.')
        string->ptr++;
    for (power = 1.0; isdigit(*string->ptr); string->ptr++) {
        data.num = 10.0 * data.num + *string->ptr - '0';
        power *= 10.0;
    }

    data.num = data.num / power;

    return CreateNode(NUM, data, nullptr, nullptr);
}

Node* GetWord(String* string)
{
    data_t data;
    int len = 0;

    data.str = (char*) calloc(MAX_WORD_LEN, sizeof(char));

    for (; isalpha(*string->ptr); len++, string->ptr++)
        data.str[len] = *string->ptr;

    data.str[len] = '\0';

    return CreateNode(FUNC, data, nullptr, nullptr);
}

Node* GetCharacter(String* string)
{
    data_t data;

    data.ch = *string->ptr;
    string->ptr++;

    return CreateNode(OP, data, nullptr, nullptr);
}

Node* CreateNode(int type, data_t data, Node* left, Node* right)
{
    Node* new_node = (Node*)calloc(1, sizeof(Node));
    new_node->type = type;

    if (type == FUNC) {
        new_node->data.str = (char*) calloc(MAX_FUNC_LEN, sizeof(char));
        strcpy(new_node->data.str, data.str);
    } else {
        new_node->data = data;
    }

    new_node->left = left;
    new_node->right = right;

    return new_node;
}