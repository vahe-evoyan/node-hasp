#ifndef HASP_H
#define HASP_H

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>
#include "hasp_api.h"
#include "errors.h"

#define MEMBUFFER_SIZE 128

class Hasp : public node::ObjectWrap {
  public:
    static void Init(v8::Handle<v8::Object> exports);

    hasp_size_t get_size(v8::Isolate*);
    char* encrypt(v8::Isolate*, char*, size_t);
    char* encrypt_wrap(v8::Isolate*, char*, size_t);
    char* decrypt(v8::Isolate*, char*, size_t);
    char* unwrap_decrypt(v8::Isolate*, char*);

  private:
    explicit Hasp();
    ~Hasp();

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> constructor;

    static void login(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void logout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void get_size(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void write(const v8::FunctionCallbackInfo<v8::Value>& args);

    hasp_handle_t handle;
};

#endif // HASP_H
