#include "../include/Language.h"

int TreeDump(Node* root, const char* current_function)
{
    static int dump_cnt = 0;

    MakeGraphDumpTxt(root, current_function, dump_cnt);

    MakePngFromTxt(dump_cnt);

    dump_cnt++;

    return dump_cnt;
}

int MakeGraphDumpTxt(Node* root, const char* current_function, int dump_cnt)
{
    FILE* dump_fp = nullptr;

    char* dump_filename =  DumpFileName(dump_cnt, "txt");

    dump_fp = fopen(dump_filename, "w");

    if (dump_fp == nullptr)
    {
        printf("In function MakeGraphDumpTxt: can't open file\n");
        exit(-1);
    }

    fprintf(dump_fp, "digraph Dump\n"
                     "{\n");

    fprintf(dump_fp, "\t\"After function: %s\"\n\n", current_function);

    RecursiveTreeDump(root, dump_fp, ROOT, true);

    fprintf(dump_fp, "\n}");

    free(dump_filename);
    fclose(dump_fp);

    return 0;
}

int RecursiveTreeDump(Node* node, FILE* dump_fp, int parents_num, bool left_node)
{
    static int num_of_nodes = -1;
    if (parents_num == ROOT)
        num_of_nodes = -1;
    num_of_nodes++;
    int node_number = num_of_nodes;

    fprintf(dump_fp, "\tNode%d [shape = record, label = ", node_number);

    switch (node->type) {
        case OP:
            fprintf(dump_fp, "\" {<str> str %d |<data> %c | {<left> L| <right> R}} \"];\n",
                    node->str_num ,node->data.ch);
            break;
        case NUM:
            fprintf(dump_fp, "\" {<str> str %d | <data> %0.2f | {<left> L | <right> R}} \"];\n",
                    node->str_num, node->data.num);
            break;
        case VAR:
            fprintf(dump_fp, "\" {<str> str %d | <data> %s | {<left> L | <right> R}} \"];\n",
                    node->str_num, node->data.str);
            break;
        case STD_FUNC: case KEY_WORD:
            fprintf(dump_fp, "\" {<str> str %d | <data> %s | {<left> L | <right> R}} \"];\n",
                    node->str_num, node->data.str);
    }

    if (num_of_nodes != 0){
        if (left_node)
            fprintf(dump_fp, "\tNode%d:<left> -> Node%d\n", parents_num, node_number);
        else
            fprintf(dump_fp, "\tNode%d:<right> -> Node%d\n", parents_num, node_number);
    }

    if (node->left != nullptr && node->right != nullptr) {
        RecursiveTreeDump(node->left, dump_fp, node_number, true);
        RecursiveTreeDump(node->right, dump_fp, node_number, false);
    } else if (node->left != nullptr && node->right == nullptr) {
        RecursiveTreeDump(node->left, dump_fp, node_number, true);
    } else if (node->left == nullptr && node->right != nullptr) {
        RecursiveTreeDump(node->right, dump_fp, node_number, false);
    }

    return 0;
}

int MakePngFromTxt(int dump_cnt)
{
    char* txt_to_png = nullptr;
    txt_to_png = (char*) calloc(200, sizeof(char));

    char* txt_dump_filename =  DumpFileName(dump_cnt, "txt");

    char* png_dump_filename =  DumpFileName(dump_cnt, "png");

    sprintf(txt_to_png, "cd ../TreeDump/ && dot %s -T png -o %s", txt_dump_filename, png_dump_filename);

    system(txt_to_png);

    free(txt_to_png);
    free(txt_dump_filename);
    free(png_dump_filename);

    return 0;
}

char* DumpFileName(int dump_cnt, const char* format)
{
    char* dump_file_name = nullptr;

    dump_file_name = (char*) calloc(50, sizeof(char));

    if(dump_file_name == nullptr)
    {
        printf("In function DumpFileName: dump_file_name is nullptr");
        return nullptr;
    }

    sprintf(dump_file_name, "../TreeDump/TreeDump%d.%s", dump_cnt, format);

    return dump_file_name;
}