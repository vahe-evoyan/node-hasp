#ifndef NODE_HASP_H
#define NODE_HASP_H

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>
#include "hasp_api.h"
#include "errors.h"
#include "hasp.h"

#define MEMBUFFER_SIZE 128

class NodeHasp : public node::ObjectWrap {
  public:
    static void Init(v8::Handle<v8::Object> exports);
    hasp_size_t get_size(v8::Isolate*);
  private:
    explicit NodeHasp();
    ~NodeHasp();
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> constructor;
    static void login(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void logout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void get_size(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void write(const v8::FunctionCallbackInfo<v8::Value>& args);
    Hasp hasp;
};

#endif // NODE_HASP_H
