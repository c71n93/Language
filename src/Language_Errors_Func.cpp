#include "../include/Language.h"

int Require(TokensArray* tokens_array, const char sign)
{
    if ((*tokens_array->ptr)->data.ch == sign) {
        TreeDtor(*tokens_array->ptr);
        tokens_array->ptr++;
    }
    else {
        char* message = (char*) calloc(ERROR_CODE_LEN, sizeof(char));
        sprintf(message, "Required sign %c\n", sign);
        Error(__FUNCTION__, message);
        free(message);
        return SYNTAX_ERROR;
    }

    return 0;
}

int Error(const char* function, const char* message)
{
    printf("Error in function %s: %s\n", function, message);
    return SYNTAX_ERROR;
}

