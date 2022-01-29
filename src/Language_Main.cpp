#include "../include/Language.h"

int main(int argc, char* argv[])
{
    FileName filename;
    if (GetFileNames(&filename, argc, argv) == 1)
        return 1;

    String string;
    ScanProgram(filename.input, &string);
    char* old_string_ptr = string.ptr;
    printf("string: %s\n\n", string.ptr);

    TokensArray tokens_array;
    if (LexicalAnalysis(&string, &tokens_array) == SYNTAX_ERROR)
        return SYNTAX_ERROR;
    Node** old_tokens_array_ptr = tokens_array.ptr;

    free(old_string_ptr);

    PrintNodes(&tokens_array);

    Node* root = GetG(&tokens_array);
    if((root) == nullptr) {
        free(old_tokens_array_ptr);
        return -1;
    }
    TreeDump(root, "GetG");

    if (BackEnd(filename, root) == CODE_GENERATION_ERROR)
        return CODE_GENERATION_ERROR;

    TreeDtor(root); //TODO: x=0;x=0; <-Leaks, x=0;x=(0); <- No Leaks, x=(0);x=0; <- No Leaks (пока забил)

    free(old_tokens_array_ptr);

    return 0;
}