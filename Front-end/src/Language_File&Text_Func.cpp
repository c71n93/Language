#include "../include/Language.h"

int ScanProgram(String* string)
{
    string->ptr = (char*) calloc(MAX_PROGRAM_LEN, sizeof(char));

    printf("Enter the text of your program:\n");
    ScanString(string->ptr);
    DeleteSpaces(string->ptr);

    return 0;
}

int ScanString(char* array)
{
    int i = 0;

    while (true) {
        int c = getchar();

        if (i > MAX_PROGRAM_LEN) {
            printf("In function ScanString: too much letters\n");
            break;
        }

        if (c == '\n') {
            array[i] = '\0';
            break;
        } else {
            array[i] = (char) c;
            i++;
        }
    }

    return 0;
}

int DeleteSpaces(char* str)
{
    size_t len = strlen(str);
    size_t i = 0;

    for (size_t j = 0; j < len; j++) {
        if (!isspace(str[j])) {
            str[i] = str[j];
            i++;
        }
    }

    str[i] = '\0';

    return 0;
}