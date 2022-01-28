#include "../include/Language.h"

int main(int argc, char* argv[])
{
    FileName filename;
    if (GetFileNames(&filename, argc, argv) == 1)
        return 1;

    String string;
    ScanProgram(filename.input, &string);
    char* old_string_ptr = string.ptr;
    printf("string: %s\n\n", string.ptr); //TODO: убрать все распечатки

    TokensArray tokens_array;
    if (LexicalAnalysis(&string, &tokens_array) == SYNTAX_ERROR)
        return SYNTAX_ERROR;
    Node** old_tokens_array_ptr = tokens_array.ptr;

    free(old_string_ptr);

    PrintNodes(&tokens_array); //TODO: убрать все распечатки

    Node* root = GetG(&tokens_array);
    if((root) == nullptr) {
        free(old_tokens_array_ptr);
        return -1;
    }
    TreeDump(root, "GetG");

    BackEnd(filename, root);

    TreeDtor(root);

    free(old_tokens_array_ptr);

    return 0;
}