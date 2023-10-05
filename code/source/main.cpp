#include "../include/crypto.h"

const int ENCRYPT_MODE = 0; // modes
const int DECRYPT_MODE = 1;
const int RECOVER_MODE = 2;
const int SUPER_MODE = 3;

int main(int argc, char *argv[]) {
    crypto c;
    int mode = 0;
    std::string infilename;
    std::string outfilename;
    std::string keyword;

    // parse command line
    if (argc < 2) {
        c.myhelper.print_log("Not enough arguments , use -h for help", ERROR);
        return -1;
    }

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-f" && i + 1 < argc) // check if the arg is "-f"
        {
            infilename = argv[i + 1]; // get the arg as filename
            i++;                      // jump over the next argument
        } else if (arg == "-k" && i + 1 < argc) {
            keyword = argv[i + 1];
            i++;
        } else if (arg == "-o" && i + 1 < argc) {
            outfilename = argv[i + 1];
            i++;
        } else if (arg == "-e") {
            mode = ENCRYPT_MODE;
        } else if (arg == "-d") {
            mode = DECRYPT_MODE;
        } else if (arg == "-r") {
            mode = RECOVER_MODE;
        } else if (arg == "-h") {
            c.myhelper.print_help();
            return 0;
        } else if (arg == "-v") {
            c.myhelper.print_version();
            return 0;
        } else if (arg == "-s") {
            mode = SUPER_MODE;
            keyword = "6666";
        } else {
            c.myhelper.print_log("Wrong arg: " + arg + " use -h for help", ERROR);
            return -1;
        }
    }

    if (infilename.empty()) { // input file is neccessary
        c.myhelper.print_log("No input file, use -h for help", ERROR);
        return -1;
    }

    if (mode != RECOVER_MODE) { // recover mode doesn't need output file or key
        if (outfilename.empty()) {
            std::string msg = "No output file, use default output file: " + infilename + ".zjx";
            outfilename = infilename + ".zjx";
            c.myhelper.print_log(msg, WARNING);
        }
        if (keyword.empty()) {
            c.myhelper.print_log("No keyword, use -h for help", ERROR);
            return -1;
        }
        if (!c.init(infilename, outfilename, keyword)) // gen keys and init files
        {
            c.myhelper.print_log("Init failed", ERROR);
            return -1;
        }
    }

    switch (mode) {
    case ENCRYPT_MODE:
        c.runEMode();
        break;
    case DECRYPT_MODE:
        c.runDMode();
        break;
    case RECOVER_MODE:
        if (infilename.empty()) {
            c.myhelper.print_log("No input file, use -h for help", ERROR);
            return -1;
        }
        c.printTips(infilename);
        break;
    case SUPER_MODE:
        c.myhelper.print_log("Superuser mode", WARNING);
        c.superuser();
        break;
    default:
        break;
    }

    return 0;
}