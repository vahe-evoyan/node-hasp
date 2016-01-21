#ifndef HASP_H
#define HASP_H

#include <hasp_api.h>
#include <stdlib.h>

class Hasp {
  public:
    Hasp();
    ~Hasp();

    char* read();
    void write(char*);
    void clean();
    void login(const char*);
    void logout();
    hasp_size_t get_size();

    bool is_error();
    const char* get_message();
    int get_errno();

  protected:
    char* decrypt(const char*);
    char* encrypt(char*, size_t);

  private:
    hasp_handle_t handle;
    hasp_status_t status;
};

#endif // HASP_H
