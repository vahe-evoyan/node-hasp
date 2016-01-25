#include "hasp.h"
#include "errors.h"
#include <string.h>
#include <openssl/hmac.h>

#define get_crypto_length(len) len > 16 ? len : 16

const char AUTHKEY[] = "OLgs1Qwj3jzZ4l6sbqjzQVpJo6vnEm7"
                       "rvlw4Bx86MSAVFwVBCG2bcHvueasRVj";
const size_t HMAC_LENGTH = 32; // bytes

Hasp::Hasp(void) : status(HASP_STATUS_OK) {
}

Hasp::~Hasp(void) {
}

char* Hasp::decrypt(const char* data, size_t& length) {
  memcpy(&length, data, __SIZEOF_SIZE_T__);
  size_t crypto_length = get_crypto_length(length);
  // hmac
  unsigned char* digest = new unsigned char[HMAC_LENGTH];
  memcpy(digest, data + __SIZEOF_SIZE_T__, HMAC_LENGTH);
  // copy encrypted data
  char* content = new char[crypto_length];
  memcpy(content, data + __SIZEOF_SIZE_T__ + HMAC_LENGTH, crypto_length);
  // authenticate
  // not clear why crypto_length is used
  unsigned char* auth = HMAC(EVP_sha256(), AUTHKEY, strlen(AUTHKEY),
                             (unsigned char*)content, crypto_length,
                             NULL, NULL);
  if (0 != memcmp(digest, auth, HMAC_LENGTH)) {
    status = HASP_INT_ERR;
    return NULL;
  }
  status = hasp_decrypt(handle, content, crypto_length);
  delete [] digest;
  return content;
}

char* Hasp::read(size_t& length) {
  hasp_size_t fsize = get_size();
  char* data = new char[fsize];
  status = hasp_read(handle, HASP_FILEID_RW, 0, fsize, data);
  return decrypt(data, length);
}

void Hasp::login(const char* vendor_code) {
  status = hasp_login(HASP_DEFAULT_FID,
                      (hasp_vendor_code_t *) vendor_code,
                      &handle);
}

bool Hasp::is_error() {
  return status != HASP_STATUS_OK;
}

const char* Hasp::get_message() {
  return hasp_statusmap[status];
}

int Hasp::get_errno() {
  return (int) status;
}

void Hasp::logout() {
  status = hasp_logout(handle);
}

char* Hasp::encrypt(char* content, size_t length) {
  // encrypt
  size_t crypto_length = get_crypto_length(length);
  status = hasp_encrypt(handle, content, crypto_length);
  // hmac
  unsigned char* digest = HMAC(EVP_sha256(), AUTHKEY, strlen(AUTHKEY),
                               (unsigned char*)content, crypto_length,
                               NULL, NULL);
  // wrap
  char* wrapped = new char[crypto_length + HMAC_LENGTH + __SIZEOF_SIZE_T__];
  memcpy(wrapped, &length, __SIZEOF_SIZE_T__);
  memcpy(wrapped + __SIZEOF_SIZE_T__, digest, HMAC_LENGTH);
  memcpy(wrapped + __SIZEOF_SIZE_T__ + HMAC_LENGTH, content, crypto_length);
  return wrapped;
}

void Hasp::write(char* input, size_t length) {
  hasp_size_t fsize = get_size();
  if (length + __SIZEOF_SIZE_T__ + HMAC_LENGTH > fsize) {
    status = HASP_MEM_RANGE;
    return;
  }
  clean();
  char* data = encrypt(input, length);
  status = hasp_write(handle, HASP_FILEID_RW, 0, fsize, data);
  delete [] data;
}

void Hasp::clean() {
  hasp_size_t fsize = get_size();
  // fsize -= fsize/2;
  char* data = new char[fsize];
  for (hasp_size_t i = 0; i < fsize; ++i) {
    data[i] = '\0';
  }
  status = hasp_write(handle, HASP_FILEID_RW, 0, fsize, data);
  delete data;
}

hasp_size_t Hasp::get_size() {
  hasp_size_t fsize;
  status = hasp_get_size(handle, HASP_FILEID_RW, &fsize);
  return fsize;
}
