#include "rsa.h"

#include <openssl/pem.h>
#include <openssl/bio.h>


RSA* rsa_public_init(char* key)
{
  BIO* bio = BIO_new_mem_buf(key, -1 /* null-term */);
  RSA* rsa = 0;

  PEM_read_bio_RSA_PUBKEY(bio, &rsa, 0, 0);

  if (!rsa)
    printf("!rsa pub\n");

  return rsa;
}


RSA* rsa_private_init(char* key)
{
  BIO* bio = BIO_new_mem_buf(key, -1 /* null-term */);
  RSA* rsa = 0;

  PEM_read_bio_RSAPrivateKey(bio, &rsa, 0, 0);

  if (!rsa)
    printf("!rsa prv\n");

  return rsa;
}

