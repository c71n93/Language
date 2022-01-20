#include "../include/Language.h"

int main(int argc, char* argv[])
{
    String string;
    ScanProgram(GetFileName(argc, argv), &string);
    char* old_string_ptr = string.ptr;
    printf("string: %s\n\n", string.ptr);

    TokensArray tokens_array;
    if (LexicalAnalysis(&string, &tokens_array) == SYNTAX_ERROR)
        return SYNTAX_ERROR;
    Node** old_tokens_array_ptr = tokens_array.ptr;

    free(old_string_ptr);

    PrintNodes(&tokens_array);

    Node* root = GetG(&tokens_array);
    CHECK_ROOT(root, old_tokens_array_ptr);
    TreeDump(root, "GetG");

    TreeDtor(root);

    free(old_tokens_array_ptr);

    return 0;
}