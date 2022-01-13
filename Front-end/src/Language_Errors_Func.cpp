#include "../include/Language.h"

//int Require(const char sign)
//{
//    if (*s == sign) {
//        s++;
//    }
//    else {
//        SyntaxError(__FUNCTION__, "");
//        printf("Required sign %c\n", sign);
//        return SYNTAX_ERROR;
//    }
//
//    return 0;
//}

int SyntaxError(const char* function, const char* message)
{
    printf("SyntaxError in function %s: %s\n", function, message);
    return SYNTAX_ERROR;
}