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
    char keys[KEY_LEN_BYTES];
    std::string superkey = "ericjuice";

public:
    helper myhelper;

    crypto(); // constructor
    ~crypto(); // destructor
    
    // try to init the crypto's file path and keyGen
    bool init(const std::string &inputFile, const std::string &outputFile, std::string keyword);
    bool writeHeader();               //  write 512 bits/ 64 bytes header to the encrypted file
    bool encryptFile();               // encrypt the file while writing header
    bool decryptFile();               // decrypt the file while checking header
    bool keyGen(std::string keyword); // transform the key to KEY_LEN_BYTES's char array
    bool printTips();                 // print tips for recovering key
    bool superuser();                 // check if the superuser
    void runEMode();                  // run encrypt mode
    void runDMode();                  // run decrypt mode
};  

#endif