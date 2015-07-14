#include "hasp.h"

using namespace v8;

Persistent<Function> Hasp::constructor;

Hasp::Hasp() {
}

Hasp::~Hasp() {
}

void Hasp::Init(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "Hasp"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "login", login);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSize", get_size);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "Hasp"), tpl->GetFunction());
}

void Hasp::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    Hasp* h = new Hasp();
    h->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void Hasp::login(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  Hasp* h = ObjectWrap::Unwrap<Hasp>(args.Holder());
  hasp_status_t status;
  status = hasp_login(HASP_DEFAULT_FID,
                  (hasp_vendor_code_t *)vendor_code,
                  &h->handle);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  args.GetReturnValue().Set(Integer::New(isolate,
                            static_cast<int32_t>(status)));
} 

void Hasp::get_size(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  hasp_status_t status;
  hasp_size_t fsize;
  Hasp* h = ObjectWrap::Unwrap<Hasp>(args.Holder());
  status = hasp_get_size(h->handle, HASP_FILEID_RW, &fsize);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  args.GetReturnValue().Set(Integer::New(isolate,
                            static_cast<uint32_t>(fsize)));
}

void Hasp::read(const FunctionCallbackInfo<Value>& args) {
}

void Hasp::write(const FunctionCallbackInfo<Value>& args) {
}

