#include "../include/Language.h"

char* GetFileName(int argc, char* argv[])
{
    if (argc == 2) {
        return argv[1];
    } else {
        printf("In function GetSaveFileName: wrong arguments\n");
        exit(-1);
    }
}

size_t GetSizeOfFile(FILE* fp)
{
    fseek(fp, 0, SEEK_END);
    size_t size_of_file = ftell(fp);
    rewind(fp);

    return size_of_file;
}

int ScanProgram(char* filename, String* string)
{
    FILE* program = fopen(filename, "r");

    if (program == nullptr)
    {
        printf("Wrong input file");
        return WRONG_INPUT_FILE;
    }

    size_t size_of_file = GetSizeOfFile(program);
    string->ptr = (char*) calloc(size_of_file, sizeof(char));
    size_t len = fread(string->ptr, sizeof(char), size_of_file, program);
    string->ptr[len] = '\0';

    DeleteSpaces(string->ptr);

    return 0;
}

size_t DeleteSpaces(char *str)
{
    int i, j;
    int is_newline = true;
    int is_space = true;
    int num_of_strings = 0;

    for (i = 0, j = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            if(!is_newline && str[i + 1] != '\n' && str[i + 1] != '\0') {
                str[j++] = str[i];
                is_newline = true;
                num_of_strings++;
            } else
                continue;
        } else if (isspace(str[i])) {
            if (!is_space && !is_newline){
                str[j++] = str[i];
                is_space = true;
            } else
                continue;
        } else {
            str[j++] = str[i];
            is_newline = false;
            is_space = false;
        }
    }

    str[j] = '\n';
    return num_of_strings + 1;
}