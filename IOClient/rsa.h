#pragma once

#include <openssl/rsa.h>


#define rsa_padding RSA_NO_PADDING // TODO: which padding ?


#ifdef __cplusplus
extern "C"
{
#endif

RSA* rsa_public_init(char* key);
RSA* rsa_private_init(char* key);

#ifdef __cplusplus
}
#endif
