#ifndef zjx_help_def
#define zjx_help_def

#include <stdio.h>
#include <string>

#define NORMAL 0
#define WARNING 1
#define ERROR 2

const int KEY_LEN_BYTES = 16;

#pragma pack(1)
struct file_header{ // 512 bytes
    char author[8]; // max 8 bytes , note: the last char must be '\0'
    int version1; // 4 bytes, to check if the file is right version
    int version2; // 4 bytes
    char key[KEY_LEN_BYTES]; // current it is 16 bytes, this key is XORed with super key
    char tips[512 - 8 - 8 - KEY_LEN_BYTES]; // tips for recover key
};
#pragma pack()

class helper{
    int version1 = 1;
    int version2 = 0;

    public:
    helper(){
        // print_log("helper hello", NORMAL);
    }
    void print_version(){
        printf("\033[1;32m");
        printf("version: %d.%d\n", version1, version2);
        printf("\033[0;32m");
        printf("@copyright: ericjuice  Github: https://github.com/ericjuice \n");
        printf("\033[0m");
    }
    void print_help(){
        printf("\033[33m");
        printf("Usage: <executable> [args]\n\n");

        printf("    args:\n");
        printf("        -e: encrypt mode\n");
        printf("        -d: decrypt mode\n");
        printf("        -f: input file name\n");
        printf("        -k: keyword\n");
        printf("        -o (optional): output file name\n");
        printf("        -r (optional): recover your key through tips\n");
        printf("        -s (optional): super key:\n");
        printf("        -h (optional): for help info\n");
        printf("        -v (optional): for version info\n");

        printf("\n");

        printf("Example: ./<executable> -e -f \"input.txt\" -k \"keyword\" -o \"output.txt\"\n");
        printf("\033[0m");

    }
    void print_log(std::string str, int type){
        switch(type){
            case NORMAL:
                printf("\033[32m LOG_INFO: ");
                printf("%s\n", str.c_str());
                printf("\033[0m");
                break;
            case WARNING:
                printf("\033[33m LOG_WARNING: ");
                printf("%s\n", str.c_str());
                printf("\033[0m");
                break;
            case ERROR:
                printf("\033[31m LOG_ERROR: ");
                printf("%s\n", str.c_str());
                printf("\033[0m");
                break;
            default:
                printf("\033[31m LOG_ERROR: ");
                printf("unknown info type\n");
                printf("\033[0m");
                break;
        }
    }
};

#endif