#include <v8.h>
#include <node.h>

#include "hasp_api.h"
#include "hasp.h"
#include "errors.h"

using namespace v8;

void login(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  hasp_status_t status;
  hasp_handle_t handle;
  status = hasp_login(HASP_DEFAULT_FID,
                  (hasp_vendor_code_t *)vendor_code,
                  &handle);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  args.GetReturnValue().Set(Integer::New(isolate,
                            static_cast<int32_t>(status)));
}

void register_module(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "login", login);
}

NODE_MODULE(hasp, register_module)

