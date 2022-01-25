#include "../include/Language.h"

int Require(TokensArray* tokens_array, const char sign)
{
    if ((*tokens_array->ptr)->data.ch == sign) {
        TreeDtor(*tokens_array->ptr);
        tokens_array->ptr++;
    }
    else {
        SyntaxError(__FUNCTION__, "");
        printf("Required sign %c\n", sign);
        return SYNTAX_ERROR;
    }

    return 0;
}

int SyntaxError(const char* function, const char* message)
{
    printf("SyntaxError in function %s: %s\n", function, message);
    return SYNTAX_ERROR;
}