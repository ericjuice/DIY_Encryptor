#include <iostream>
#include <fstream>
#include "../include/crypto.h"
#include "../include/help.h"


// constructor
crypto::crypto()
{
    ;
}

// destructor
crypto::~crypto()
{
    this->inFile.close();
    this->outFile.close();
    this->myhelper.print_log("Exited! Hoping next meeting", NORMAL);
}

void crypto::runEMode(){
    // todo
    return ;
}

void crypto::runDMode(){
    // todo
    return ;
}

bool crypto::keyGen(std::string keyword){
    // todo
    this->myhelper.print_log("Key generated", NORMAL);
    return true;
}

bool crypto::printTips(){
    // todo
    return true;
}

bool crypto::superuser(){
    // todo
    return true;
}

bool crypto::writeHeader(){
    // todo
    return true;
}


bool crypto::init(const std::string &inputFile, const std::string &outputFile, std::string keyword)
{
    // open files
    try
    {
        this->inFile.open(inputFile, std::ios::in | std::ios::binary);
    }
    catch (const std::exception &e)
    {
        this->myhelper.print_log(e.what(), ERROR);
        return false;
    }
    if (!inFile)
    {
        std::string msg = "File " + inputFile + " not found!";
        this->myhelper.print_log(msg, ERROR);
        return false;
    }

    try
    {
        this->outFile.open(outputFile, std::ios::out | std::ios::binary);
    }
    catch (const std::exception &e)
    {
        this->myhelper.print_log(e.what(), ERROR);
        return false;
    }
    if (!this->outFile)
    {
        std::string msg = "File " + outputFile + " not found!";
        this->myhelper.print_log(msg, ERROR);
        return false;
    }

    if(!this->keyGen(keyword)) // transform the key to KEY_LEN_BYTES's char array
    {
        std::string msg = "Key generation failed!";
        this->myhelper.print_log(msg, ERROR);
        return false;
    }
        
    return true;
}


// encrypt the file and write header
bool crypto::encryptFile() 
{
    this->keys[0] = 0xFF;

    char byte ;
    while (this->inFile.get(byte))
    {
        byte = byte ^ this->keys[0]; // Perform XOR encryption with key
        this->outFile.put(byte);
    }

    // this->inFile.close();
    // this->outFile.close();

    this->myhelper.print_log("File encrypted successfully!", NORMAL);
    return true;
}

bool crypto::decryptFile(){
    // todo
    return true;
}