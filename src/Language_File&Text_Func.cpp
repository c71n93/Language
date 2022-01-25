#include "../include/Language.h"

int GetFileNames (FileName *filename, int argc, char* argv[])
{
    if (argc == 3) {
        filename->input = argv[1];
        filename->output = argv[2];

        return 0;
    } else {
        printf("In function GetFileName: wrong arguments\n");
        return 1;
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

    if (program == nullptr) {
        printf("Wrong input file");
        return WRONG_INPUT_FILE;
    }

    size_t size_of_file = GetSizeOfFile(program);
    string->ptr = (char*) calloc(size_of_file + 1, sizeof(char));
    size_t len = fread(string->ptr, sizeof(char), size_of_file, program);
    string->ptr[len] = '\0';

    DeleteSpaces(string->ptr);

    fclose(program);

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