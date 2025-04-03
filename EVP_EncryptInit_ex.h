//
// Created by Furkan Aşkın on 3.04.2025.
//

#ifndef STKCS_LAST_EVP_ENCRYPTINIT_EX_H
#define STKCS_LAST_EVP_ENCRYPTINIT_EX_H


#include "EVP_CIPHER_CTX.h"

class EVP_EncryptInit_ex {

public:
    EVP_EncryptInit_ex(EVP_CIPHER_CTX ctx, void, long i, unsigned char string[32], unsigned char string1[16]);
};


#endif //STKCS_LAST_EVP_ENCRYPTINIT_EX_H
