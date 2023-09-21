#include "../include/crypto.h"

int main(int argc, char *argv[])
{
    crypto c;
    int mode = 0; // 0 for encrypt, 1 for decrypt
    std::string infilename;
    std::string outfilename;
    std::string keyword;
    bool super_flag = false;

    // parse command line
    if (argc < 2)
    {
        c.myhelper.print_help();
        return -1;
    }

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-f" && i + 1 < argc) // check if the arg is "-f"
        {
            infilename = argv[i + 1]; // get the arg as filename
            i++;                      // jump over the next argument
        }
        else if (arg == "-k" && i + 1 < argc)
        {
            keyword = argv[i + 1];
            i++;
        }
        else if (arg == "-o" && i + 1 < argc)
        {
            outfilename = argv[i + 1];
            i++;
        }
        else if (arg == "-e")
        {
            mode = 0;
        }
        else if (arg == "-d")
        {
            mode = 1;
        }
        else if (arg == "-r")
        {
            if(infilename.empty()){
                c.myhelper.print_log("No input file", ERROR);
                c.myhelper.print_help();
                return -1;
            }
            c.printTips(infilename);
            return 0;
        }
        else if (arg == "-h")
        {
            c.myhelper.print_help();
            return 0;
        }
        else if (arg == "-v")
        {
            c.myhelper.print_version();
            return 0;
        }
        else if (arg == "-s")
        {
            super_flag = true;
            keyword = "6666";
        }
        else
        {
            c.myhelper.print_log("Wrong arg: " + arg, ERROR);
            c.myhelper.print_help();
            return -1;
        }
    }
    if (infilename.empty())
    {
        c.myhelper.print_log("No input file", ERROR);
        c.myhelper.print_help();
        return -1;
    }
    if (outfilename.empty())
    {
        std::string msg = "No output file, use default output file: " + infilename + ".zjx";
        outfilename = infilename + ".zjx";
        c.myhelper.print_log(msg, WARNING);
    }
    if (keyword.empty())
    {
        c.myhelper.print_log("No keyword", ERROR);
        c.myhelper.print_help();
        return -1;
    }

    if (!c.init(infilename, outfilename, keyword)) // gen keys and init files
    {
        c.myhelper.print_log("Init failed", ERROR);
        return -1;
    }

    if (super_flag){
        c.myhelper.print_log("Superuser mode", WARNING);
        c.superuser();
        return 0;
    }

    if (mode == 0)
        c.runEMode();
    else
        c.runDMode();

    return 0;
}