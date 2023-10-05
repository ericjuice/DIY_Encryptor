#include "../include/crypto.h"
#include "../include/help.h"
#include <cstring>
#include <fstream>
#include <iostream>

// constructor
crypto::crypto() {
    std::memset(this->temp_keys, 0, sizeof(this->temp_keys));
    std::memset(this->keys, 0, sizeof(this->keys));
    std::memset((void *)&(this->header), 0, sizeof(this->header));
}

// destructor
crypto::~crypto() {
    this->inFile.close();
    this->outFile.close();
    this->myhelper.print_log("Exited! Hoping next meeting", NORMAL);
}

void crypto::runEMode() {
    this->writeHeader();
    this->encryptFile();
    return;
}

void crypto::runDMode() {
    // read header
    this->inFile.read(reinterpret_cast<char *>(&this->header), sizeof(this->header));

    // check if the file is right version
    if (this->header.version1 != zjx_version1) // Minor versions within the same major version are compatible with each other
    {
        std::string msg = "Wrong version!";
        msg += " The file version: " + std::to_string(this->header.version1) + "." + std::to_string(this->header.version2);
        msg += " Current executable version: " + std::to_string(zjx_version1) + "." + std::to_string(zjx_version2);
        this->myhelper.print_log(msg, ERROR);
        return;
    }
    if (this->header.version2 != zjx_version2) // warning for diff minor version
    {
        std::string msg = "Different minor version!";
        msg += " The file version: " + std::to_string(this->header.version1) + "." + std::to_string(this->header.version2);
        msg += " Current executable version: " + std::to_string(zjx_version1) + "." + std::to_string(zjx_version2);
        this->myhelper.print_log(msg, WARNING);
    }

    // check key if it's right: this.keys == keygen(superkey) xor header.key
    this->keyGen(this->superkey);
    for (int i = 0; i < KEY_LEN_BYTES / 4; i++) {
        if ((this->header.key[i] ^ this->temp_keys[i]) != this->keys[i]) {
            this->myhelper.print_log("Wrong key!", ERROR);
            return;
        }
    }

    this->decryptFile();
    return;
}

bool crypto::keyGen(std::string keyword) {
    std::memset(this->temp_keys, 0, sizeof(this->temp_keys));
    for (char c : keyword) {
        for (int i = 0; i < KEY_LEN_BYTES / 4; i++) {
            if (i % 2 == 0)
                this->temp_keys[i] ^= c + (this->temp_keys[i] >> 3) + (this->temp_keys[i] << 7) + (this->temp_keys[i] << 2);
            else
                this->temp_keys[i] ^= c + (this->temp_keys[i] >> 2) + (this->temp_keys[i] << 5) + (this->temp_keys[i] << 3);
        }
    }
    return true;
}

bool crypto::printTips(std::string inputFile) {
    // open input file
    try {
        this->inFile.open(inputFile, std::ios::in | std::ios::binary);
    } catch (const std::exception &e) {
        this->myhelper.print_log(e.what(), ERROR);
        return false;
    }
    if (!inFile) {
        std::string msg = "File " + inputFile + " not found!";
        this->myhelper.print_log(msg, ERROR);
        return false;
    }

    this->inFile.read(reinterpret_cast<char *>(&this->header), sizeof(this->header));
    if (this->header.version1 != zjx_version1) {
        std::string msg = "Wrong version!";
        msg += " The file version: " + std::to_string(this->header.version1) + "." + std::to_string(this->header.version2);
        msg += " Current executable version: " + std::to_string(zjx_version1) + "." + std::to_string(zjx_version2);
        this->myhelper.print_log(msg, ERROR);
        return false;
    }

    std::cout << "Your tips: " << this->header.tips << std::endl;
    printf("The file is encrypted by version %d.%d encryptor\n", this->header.version1, this->header.version2);

    this->inFile.seekg(0);

    return true;
}

bool crypto::superuser() {
    // check if the superuser
    printf("Please input your superuser password:\n");
    std::string password;
    std::cin >> password;
    if (password != this->superkey) {
        this->myhelper.print_log("Wrong password!", ERROR);
        return false;
    }

    // read header
    this->inFile.read(reinterpret_cast<char *>(&this->header), sizeof(this->header));

    // check if the file is right version
    if (this->header.version1 != zjx_version1) {
        std::string msg = "Wrong version!";
        msg += " The file version: " + std::to_string(this->header.version1) + "." + std::to_string(this->header.version2);
        msg += " Current executable version: " + std::to_string(zjx_version1) + "." + std::to_string(zjx_version2);
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

bool crypto::writeHeader() {
    // todo
    memcpy(this->header.author, "zjx66666", 8);
    this->header.version1 = zjx_version1;
    this->header.version2 = zjx_version2;

    // store the current key to header
    this->keyGen(this->superkey); // header.key == keygen(superkey) xor filekey
    for (int i = 0; i < KEY_LEN_BYTES / 4; i++) {
        this->header.key[i] = this->temp_keys[i] ^ this->keys[i];
    }

    // check if the user wanna set some tips
    this->myhelper.print_log("Do you want to set some tips for your password?\n Please input 'yes' or 'no': ", NORMAL);
    std::string choice;
    std::cin >> choice;
    if (choice == "yes" || choice == "Yes" || choice == "YES" || choice == "y" || choice == "Y") {
        std::string tips;
        this->myhelper.print_log("Please input a reminder sentence for your password: ", NORMAL);
        std::cin.ignore();
        std::getline(std::cin, tips);
        while (tips.size() == 0 || tips.size() >= sizeof(this->header.tips)) {
            std::string msg = "Please input correct tips , max length: " + std::to_string(sizeof(this->header.tips)) + "!";
            this->myhelper.print_log(msg, WARNING);
            std::cin.ignore();
            std::getline(std::cin, tips);
        }
        memcpy(this->header.tips, tips.c_str(), sizeof(this->header.tips));
    } else {
        std::string msg = "No tips!";
        memcpy(this->header.tips, msg.c_str(), sizeof(this->header.tips));
        this->myhelper.print_log(msg, WARNING);
    }

    this->outFile.write(reinterpret_cast<char *>(&this->header), sizeof(this->header));

    return true;
}

bool crypto::init(const std::string &inputFile, const std::string &outputFile, std::string keyword) {
    // open files
    try {
        this->inFile.open(inputFile, std::ios::in | std::ios::binary);
    } catch (const std::exception &e) {
        this->myhelper.print_log(e.what(), ERROR);
        return false;
    }
    if (!inFile) {
        std::string msg = "File " + inputFile + " not found!";
        this->myhelper.print_log(msg, ERROR);
        return false;
    }

    try {
        this->outFile.open(outputFile, std::ios::out | std::ios::binary);
    } catch (const std::exception &e) {
        this->myhelper.print_log(e.what(), ERROR);
        return false;
    }
    if (!this->outFile) {
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
bool crypto::encryptFile() {
    if (this->keyXorFile()) {
        this->myhelper.print_log("File encrypted successfully!", NORMAL);
    } else {
        this->myhelper.print_log("File encryption failed!", ERROR);
        return false;
    }
    return true;
}

bool crypto::keyXorFile() {
    try {

        std::streampos originalPos_loadline = inFile.tellg(); // vars for loadline work
        inFile.seekg(0, std::ios::end);
        std::streampos endPos_loadline = inFile.tellg();
        inFile.seekg(originalPos_loadline, std::ios::beg); // move back to the start
        std::streamsize len_loadline = endPos_loadline - originalPos_loadline;
        std::streamsize tempBytes_loadline = 0; // used to count bytes in a single percent
        const std::streamsize onePercentSize_loadline = len_loadline / 100 == 0 ? 1 : len_loadline / 100;
        int percentCount_loadline = 0;

        // 102400: After calculating, 100MB file should be encrypted in 1s.
        // If file is less than 100KB, set block size to 1
        int temp_block_size = len_loadline / 102400 == 0 ? 1 : len_loadline / 102400;
        temp_block_size = temp_block_size > 10240 ? 10240 : temp_block_size; // max block size is 10240 (40KB)
        const int block_size = temp_block_size;

        uint temp_data[block_size] = {0};
        int index = 0;           // key index
        clock_t start = clock(); // timer

        while (this->inFile.read(reinterpret_cast<char *>(&temp_data), sizeof(temp_data))) {

            if (tempBytes_loadline < sizeof(temp_data)) { // updates the loadline each percent
                if (percentCount_loadline < 100) { // in some tiny files, this maybe greater than 100
                    percentCount_loadline++;
                }
                printf(" Working...[");
                printf("\033[1;34m");
                for (int j = 0; j < 50; j++) {
                    if (j < percentCount_loadline / 2)
                        printf("=");
                    else if (j == percentCount_loadline / 2)
                        printf(">");
                    else
                        printf(" ");
                }
                printf("\033[0m]\033[1;32m %d %% \033[0m\r", percentCount_loadline);
                std::fflush(stdout);
            }

            for (int i = 0; i < block_size; i++) {
                temp_data[i] = temp_data[i] ^ this->keys[index]; // Perform XOR encryption with key
            }

            this->outFile.write(reinterpret_cast<char *>(&temp_data), sizeof(temp_data));

            index++;
            index %= KEY_LEN_BYTES / 4;
            tempBytes_loadline += sizeof(temp_data);
            tempBytes_loadline %= onePercentSize_loadline;
        }

        // Handle remaining bytes
        std::streamsize remainingBytes = this->inFile.gcount();
        if (remainingBytes > 0 && remainingBytes < sizeof(temp_data)) {
            char *remainingData = reinterpret_cast<char *>(&temp_data);
            this->inFile.clear();                               // Clear any error flags
            this->inFile.seekg(-remainingBytes, std::ios::cur); // Move the file pointer back
            this->inFile.read(remainingData, remainingBytes);

            // Save the remaining plaintext directly
            this->outFile.write(remainingData, remainingBytes);
        }

        clock_t end = clock(); // timer
        float seconds = (end - start) / 1000.0;
        printf("\n"); // return a new line for the loadline
        std::string msg = "File size: " + std::to_string(len_loadline / 1024.0) + "KB. Time used: " + std::to_string(seconds) + "s";
        this->myhelper.print_log(msg, NORMAL);

    } catch (std::exception &e) {
        this->myhelper.print_log(e.what(), ERROR);
        return false;
    }
    return true;
}

bool crypto::decryptFile() {
    if (this->keyXorFile()) {
        this->myhelper.print_log("File decrypted successfully!", NORMAL);
        return true;
    } else {
        this->myhelper.print_log("File decryption failed!", ERROR);
        return false;
    }
}