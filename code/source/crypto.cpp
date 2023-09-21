#include <iostream>
#include <fstream>
#include <cstring>
#include "../include/crypto.h"
#include "../include/help.h"

// constructor
crypto::crypto()
{
    std::memset(this->temp_keys, 0, sizeof(this->temp_keys));
    std::memset(this->keys, 0, sizeof(this->keys));
    std::memset((void *)&(this->header), 0, sizeof(this->header));
}

// destructor
crypto::~crypto()
{
    this->inFile.close();
    this->outFile.close();
    this->myhelper.print_log("Exited! Hoping next meeting", NORMAL);
}

void crypto::runEMode()
{
    this->writeHeader();
    this->encryptFile();
    return;
}

void crypto::runDMode()
{
    // read header
    this->inFile.read(reinterpret_cast<char *>(&this->header), sizeof(this->header));

    // check if the file is right version
    if (this->header.version1 != zjx_version1 || this->header.version2 != zjx_version2)
    {
        std::string msg = "Wrong version!";
        msg += "The file version: " + std::to_string(this->header.version1) + "." + std::to_string(this->header.version2);
        msg += "Current executable version: " + std::to_string(zjx_version1) + "." + std::to_string(zjx_version2);
        this->myhelper.print_log(msg, ERROR);
        return;
    }

    // check key if it's right: this.keys == keygen(superkey) xor header.key
    this->keyGen(this->superkey);
    for (int i = 0; i < KEY_LEN_BYTES / 4; i++)
    {
        if ((this->header.key[i] ^ this->temp_keys[i]) != this->keys[i])
        {
            this->myhelper.print_log("Wrong key!", ERROR);
            return;
        }
    }

    this->decryptFile();
    return;
}

bool crypto::keyGen(std::string keyword)
{
    std::memset(this->temp_keys, 0, sizeof(this->temp_keys));
    for (char c : keyword)
    {
        for (int i = 0; i < KEY_LEN_BYTES / 4; i++)
        {
            if (i % 2 == 0)
                this->temp_keys[i] ^= c + (this->temp_keys[i] >> 3) + (this->temp_keys[i] << 7) + (this->temp_keys[i] << 2);
            else
                this->temp_keys[i] ^= c + (this->temp_keys[i] >> 2) + (this->temp_keys[i] << 5) + (this->temp_keys[i] << 3);
        }
    }
    return true;
}

bool crypto::printTips(std::string inputFile)
{
    // open input file
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

    this->inFile.read(reinterpret_cast<char *>(&this->header), sizeof(this->header));
    if (this->header.version1 != zjx_version1 || this->header.version2 != zjx_version2)
    {
        std::string msg = "Wrong version!";
        msg += "The file version: " + std::to_string(this->header.version1) + "." + std::to_string(this->header.version2);
        msg += "Current executable version: " + std::to_string(zjx_version1) + "." + std::to_string(zjx_version2);
        this->myhelper.print_log(msg, ERROR);
        return false;
    }

    std::cout << "Your tips: " << this->header.tips << std::endl;

    this->inFile.seekg(0);

    return true;
}

bool crypto::superuser()
{
    // check if the superuser
    printf("Please input your superuser password:\n");
    std::string password;
    std::cin >> password;
    if (password != this->superkey)
    {
        this->myhelper.print_log("Wrong password!", ERROR);
        return false;
    }

    // read header
    this->inFile.read(reinterpret_cast<char *>(&this->header), sizeof(this->header));

    // check if the file is right version
    if (this->header.version1 != zjx_version1 || this->header.version2 != zjx_version2)
    {
        std::string msg = "Wrong version!";
        msg += "The file version: " + std::to_string(this->header.version1) + "." + std::to_string(this->header.version2);
        msg += "Current executable version: " + std::to_string(zjx_version1) + "." + std::to_string(zjx_version2);
        this->myhelper.print_log(msg, ERROR);
        return false;
    }

    // get the key by superuser key
    this->keyGen(this->superkey);
    for (int i = 0; i < KEY_LEN_BYTES / 4; i++) // filekey = keygen(superkey) xor header.key
    {
        this->keys[i] = this->temp_keys[i] ^ this->header.key[i];
    }

    this->decryptFile();

    return true;
}

bool crypto::writeHeader()
{
    // todo
    memcpy(this->header.author, "zjx66666", 8);
    this->header.version1 = zjx_version1;
    this->header.version2 = zjx_version2;

    // store the current key to header
    this->keyGen(this->superkey); // header.key == keygen(superkey) xor filekey
    for (int i = 0; i < KEY_LEN_BYTES / 4; i++)
    {
        this->header.key[i] = this->temp_keys[i] ^ this->keys[i];
    }

    // check if the user wanna set some tips
    this->myhelper.print_log("Do you want to set some tips for your password?\nPlease input 'yes' or 'no': ", NORMAL);
    std::string choice;
    std::cin >> choice;
    if (choice == "yes" || choice == "Yes" || choice == "YES" || choice == "y" || choice == "Y")
    {
        std::string tips;
        std::cin >> tips;
        while(tips.size() == 0 || tips.size() >= sizeof(this->header.tips)){
            std::string msg = "Please input correct tips , max length: " + std::to_string(sizeof(this->header.tips)) + "!";
            this->myhelper.print_log(msg, WARNING);
            std::cin >> tips;
        }
        memcpy(this->header.tips, tips.c_str(), sizeof(this->header.tips));
    }
    else
    {
        std::string msg = "No tips!";
        memcpy(this->header.tips, msg.c_str(), sizeof(this->header.tips));
        printf("Jumped\n");
    }

    this->outFile.write(reinterpret_cast<char *>(&this->header), sizeof(this->header));

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

    if (!this->keyGen(keyword)) // transform the key to KEY_LEN_BYTES's array
    {
        std::string msg = "Key generation failed!";
        this->myhelper.print_log(msg, ERROR);
        return false;
    }
    memcpy(this->keys, this->temp_keys, KEY_LEN_BYTES);

    return true;
}

// encrypt the file and write header
bool crypto::encryptFile()
{
    if (this->keyXorFile())
    {
        this->myhelper.print_log("File encrypted successfully!", NORMAL);
    }
    else
    {
        this->myhelper.print_log("File encryption failed!", ERROR);
        return false;
    }
    return true;
}

bool crypto::keyXorFile()
{
    try
    {
        uint temp_data;
        int index = 0;
        while (this->inFile.read(reinterpret_cast<char *>(&temp_data), sizeof(temp_data)))
        {
            temp_data = temp_data ^ this->keys[index]; // Perform XOR encryption with key
            this->outFile.write(reinterpret_cast<char *>(&temp_data), sizeof(temp_data));
            index++;
            index %= KEY_LEN_BYTES / 4;
        }

        // Handle remaining bytes
        std::streamsize remainingBytes = this->inFile.gcount();
        if (remainingBytes > 0 && remainingBytes < sizeof(temp_data))
        {
            char *remainingData = reinterpret_cast<char *>(&temp_data);
            this->inFile.clear();                               // Clear any error flags
            this->inFile.seekg(-remainingBytes, std::ios::cur); // Move the file pointer back
            this->inFile.read(remainingData, remainingBytes);

            // Save the remaining plaintext directly
            this->outFile.write(remainingData, remainingBytes);
        }
    }
    catch (std::exception &e)
    {
        this->myhelper.print_log(e.what(), ERROR);
        return false;
    }
    return true;
}

bool crypto::decryptFile()
{
    if (this->keyXorFile())
    {
        this->myhelper.print_log("File decrypted successfully!", NORMAL);
        return true;
    }
    else
    {
        this->myhelper.print_log("File decryption failed!", ERROR);
        return false;
    }
}