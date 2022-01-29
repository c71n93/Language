#include "../include/Language.h"

int LexicalAnalysis(String* string, TokensArray* tokens_array)
{
    tokens_array->ptr = (Node**) calloc(strlen(string->ptr) + 1, sizeof(Node*));
    Node** old_tokens_array_ptr = tokens_array->ptr;

    while(true) {
        if (isdigit(*string->ptr)) {
            *tokens_array->ptr = GetNumber(string);
            if (*tokens_array->ptr == nullptr)
                return SYNTAX_ERROR;
        } else if (isalpha(*string->ptr)) {
            *tokens_array->ptr = GetWord(string);
        } else if (IsCharacterFromArray(*string->ptr, "+-*/()=;")) {
            *tokens_array->ptr = GetCharacter(string);
        } else if (*string->ptr == '\0') {
            *tokens_array->ptr = GetCharacter(string);
            break;
        } else {
            return Error(__FUNCTION__, "Wrong character");
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
    int len = 0;

    for (data.num = 0.0; isdigit(*string->ptr); string->ptr++, len++)
        data.num = 10.0 * data.num + *string->ptr - '0';
    if (*string->ptr == '.')
        string->ptr++, len++;
    for (power = 1.0; isdigit(*string->ptr); string->ptr++, len++) {
        data.num = 10.0 * data.num + *string->ptr - '0';
        power *= 10.0;
    }

    if (len > MAX_NUM_LEN) {
        Error(__FUNCTION__, "Too long number");
        return nullptr;
    }

    data.num = data.num / power;

    return CreateNode(NUM, data, nullptr, nullptr);
}

Node* GetWord(String* string)
{
    data_t data;
    int len = 0;

    data.str = (char*) calloc(MAX_WORD_LEN, sizeof(char));

    for (; isalpha(*string->ptr) || isdigit(*string->ptr) || *string->ptr == '_';
        len++, string->ptr++) {
        data.str[len] = *string->ptr;
        if (len >= 128)
            Error(__FUNCTION__, "Too long word");
    }

    data.str[len] = '\0';

    return CreateNode(VAR, data, nullptr, nullptr);
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

    new_node->data = data;

    new_node->left = left;
    new_node->right = right;

    return new_node;
}

bool IsCharacterFromArray(char ch, const char* array)
{
    for(int i = 0; i < strlen(array); i++)
        if (array[i] == ch)
            return true;

    return false;
}

int PrintNodes(TokensArray* tokens_array)
{
    for (int i = 0; (*tokens_array->ptr[i]).data.ch != '\0' || (*tokens_array->ptr[i]).type != OP; i++) {
        switch ((*tokens_array->ptr[i]).type) {
            case NUM:
                printf("tokens: data: %f | type: %d\n",
                       (*tokens_array->ptr[i]).data.num, (*tokens_array->ptr[i]).type);
                break;
            case VAR:
                printf("tokens: data: %s | type: %d\n",
                       (*tokens_array->ptr[i]).data.str, (*tokens_array->ptr[i]).type);
                break;
            case OP:
                printf("tokens: data: %c | type: %d\n",
                       (*tokens_array->ptr[i]).data.ch, (*tokens_array->ptr[i]).type);
                break;
        }
    }

    printf("\n");

    return 0;
}