#include "hasp.h"
#include "errors.h"
#include <string.h>
#include <openssl/hmac.h>

#define get_crypto_length(len) len > 16 ? len : 16

const char AUTHKEY[] = "OLgs1Qwj3jzZ4l6sbqjzQVpJo6vnEm7"
                       "rvlw4Bx86MSAVFwVBCG2bcHvueasRVj";

Hasp::Hasp(void) : status(HASP_STATUS_OK) {
}

Hasp::~Hasp(void) {
}

char* Hasp::decrypt(const char* data) {
  size_t length;
  memcpy(&length, data, __SIZEOF_SIZE_T__);
  size_t crypto_length = get_crypto_length(length);
  // hmac
  unsigned char* digest = new unsigned char[32]; // get size dinamically
  memcpy(digest, data + __SIZEOF_SIZE_T__, 32);
  // copy encrypted data
  char* content = new char[crypto_length];
  memcpy(content, data + __SIZEOF_SIZE_T__ + 32, crypto_length); // get size dynamically
  // authenticate
  // not clear why crypto_length is used
  unsigned char* auth = HMAC(EVP_sha256(), AUTHKEY, strlen(AUTHKEY),
                             (unsigned char*)content, crypto_length,
                             NULL, NULL);
  if (0 == memcmp(digest, auth, 32)) {
    // failed authentication
  }
  status = hasp_decrypt(handle, content, crypto_length);
  content[crypto_length] = 0; // FIXME: may fail for large data
  delete [] digest;
  return content;
}

char* Hasp::read() {
  hasp_size_t fsize = get_size();
  char* data = new char[fsize];
  status = hasp_read(handle, HASP_FILEID_RW, 0, fsize, data);
  return decrypt(data);
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
  char* wrapped = new char[crypto_length + 32 + __SIZEOF_SIZE_T__];
  memcpy(wrapped, &length, __SIZEOF_SIZE_T__);
  memcpy(wrapped + __SIZEOF_SIZE_T__, digest, 32);
  memcpy(wrapped + __SIZEOF_SIZE_T__ + 32, content, crypto_length);
  return wrapped;
}

void Hasp::write(char* input) {
  hasp_size_t fsize = get_size();
  size_t length = strlen(input);
  // FIXME: get the size of HMAC dinamically
  if (length + __SIZEOF_SIZE_T__ + 32 > fsize) {
    // space exceeded
    return;
  }
  char* data = encrypt(input, length);
  clean();
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
