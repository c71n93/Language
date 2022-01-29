#include "../include/Language.h"

int Require(TokensArray* tokens_array, const char sign, int str_num)
{
    if ((*tokens_array->ptr)->data.ch == sign) {
        TreeDtor(*tokens_array->ptr);
        tokens_array->ptr++;
    }
    else {
        char* message = (char*) calloc(ERROR_CODE_LEN, sizeof(char));
        sprintf(message, "Required sign %c", sign);
        Error(__FUNCTION__, message, str_num);
        free(message);
        return SYNTAX_ERROR;
    }

    return 0;
}

int Error(const char* function, const char* message, int str_num) //TODO: сделать определение строки с ошибкой
{
    if (str_num == -1)
        printf("Error in function %s: %s\n", function, message);
    else
        printf("Error at string %d; in function %s: %s\n", str_num, function, message);
    return SYNTAX_ERROR;
}

