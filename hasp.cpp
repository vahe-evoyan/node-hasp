#include <v8.h>
#include <node.h>

using namespace v8;

void login(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  args.GetReturnValue().Set(True(isolate));
}

void register_module(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "login", login);
}

NODE_MODULE(hasp, register_module)

