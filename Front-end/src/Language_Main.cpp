#include "../include/Language.h"

int main()
{
    String string;
    ScanProgram(&string);
    char* old_string_ptr = string.ptr;
    printf("string: %s\n", string.ptr);

    TokensArray tokens_array;
    LexicalAnalysis(&string, &tokens_array);
    Node** old_tokens_array_ptr = tokens_array.ptr;

    for (int i = 0; tokens_array.ptr[i] != nullptr; i++) {
        switch ((*tokens_array.ptr[i]).type) {
            case NUM:
                printf("tokens: data: %f | type: %d\n",
                       (*tokens_array.ptr[i]).data.num, (*tokens_array.ptr[i]).type);
                break;
            case FUNC:
                printf("tokens: data: %s | type: %d\n",
                       (*tokens_array.ptr[i]).data.str, (*tokens_array.ptr[i]).type);
                break;
            case OP:
                printf("tokens: data: %c | type: %d\n",
                       (*tokens_array.ptr[i]).data.ch, (*tokens_array.ptr[i]).type);
                break;
        }
    }

    free(old_string_ptr);

    Node* root = GetG(&tokens_array);
    CHECK_ROOT(root, old_tokens_array_ptr);
    TreeDump(root, "GetG");

    TreeDtor(root);

    free(old_tokens_array_ptr);

    return 0;
}