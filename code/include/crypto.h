#ifndef zjx_crypto_def
#define zjx_crypto_def

#include <fstream>
#include <string>
#include "help.h"

class crypto
{
private:
    std::ifstream inFile;
    std::ofstream outFile;
    uint keys[KEY_LEN_BYTES / 4];
    uint temp_keys[KEY_LEN_BYTES / 4]; // to support keyGen and superkey
    std::string superkey = "ericjuice"; // super key
    file_header header; // file header

public:
    helper myhelper;

    crypto();  // constructor
    ~crypto(); // destructor

    // try to init the crypto's file path and keyGen
    bool init(const std::string &inputFile, const std::string &outputFile, std::string keyword);
    bool writeHeader();               //  write 512 bits/ 64 bytes header to the encrypted file
    bool encryptFile();               // encrypt the file while writing header
    bool decryptFile();               // decrypt the file while checking header
    bool keyXorFile();                // the subfunc of en/decrypt
    bool keyGen(std::string keyword); // transform the key to KEY_LEN_BYTES's char array
    bool printTips(std::string infilename);                 // print tips for recovering key
    bool superuser();                 // check if the superuser
    void runEMode();                  // run encrypt mode
    void runDMode();                  // run decrypt mode
};

#endif