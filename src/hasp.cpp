#include "hasp.h"
#include "baton.h"
#include <cstring>

using namespace v8;

#include <iostream>
using namespace std;

#define get_crypto_length(len) len > 16 ? len : 16

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
  NODE_SET_PROTOTYPE_METHOD(tpl, "logout", logout);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getSize", get_size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", write);
  NODE_SET_PROTOTYPE_METHOD(tpl, "read", read);

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


bool Hasp::login(Isolate* isolate, hasp_vendor_code_t* vendor_code) {
  hasp_status_t status = hasp_login(HASP_DEFAULT_FID, vendor_code, &handle);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  return !status;
}

hasp_status_t Hasp::login(hasp_vendor_code_t* vendor_code) {
  return hasp_login(HASP_DEFAULT_FID, vendor_code, &handle);
}

void Hasp::async_login(uv_work_t* request) {
  Baton* baton = static_cast<Baton*>(request->data);
  // Isolate* isolate = Isolate::GetCurrent();
  // Local<Object> object = Local<Object>::New(isolate, baton->holder);
  // Hasp* h = ObjectWrap::Unwrap<Hasp>(object);
  baton->status = baton->hasp->login((hasp_vendor_code_t *) baton->vendor_code);
}

void Hasp::on_login(uv_work_t* request, int status) {
  Baton* baton = static_cast<Baton*>(request->data);
  Isolate* isolate = Isolate::GetCurrent();
  const uint8_t argc = 2;
  Handle<Value> argv[argc];
  argv[1] = Boolean::New(isolate, !baton->status);
  if (baton->status) {
    argv[0] = Exception::Error(String::NewFromUtf8(isolate,
                               hasp_statusmap[baton->status]));
  } else {
    argv[0] = Null(isolate);
  }
  Local<Function> callback = Local<Function>::New(isolate, baton->callback);
  Local<Object> object = Local<Object>::New(isolate, baton->holder);
  callback->Call(object, argc, argv);
  baton->callback.Reset();
  delete baton;
}

void Hasp::login(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Please specify vendor code")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Vendor code should be a string")));
    return;
  }

  // Hasp* h = ObjectWrap::Unwrap<Hasp>(args.Holder());
  // String::Utf8Value vendor_code(args[0]);
  // bool status = h->login(isolate, (hasp_vendor_code_t *) *vendor_code);
  // args.GetReturnValue().Set(Boolean::New(isolate, status));

  Baton *baton = new Baton();
  Local<Function> callback = Local<Function>::Cast(args[1]);
  Local<Object> holder = Local<Object>::Cast(args.This());
  baton->request.data = baton;
  String::Utf8Value vendor_code(args[0]);
  baton->vendor_code = new char[vendor_code.length()];
  strncpy(baton->vendor_code, *vendor_code, vendor_code.length());
  baton->callback.Reset(isolate, Persistent<Function>(isolate, callback));
  baton->holder.Reset(isolate, Persistent<Object>(isolate, holder));
  baton->hasp = ObjectWrap::Unwrap<Hasp>(args.Holder());
  uv_queue_work(uv_default_loop(), &(baton->request), async_login, on_login);
}

void Hasp::logout(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  Hasp* h = ObjectWrap::Unwrap<Hasp>(args.Holder());
  hasp_status_t status;
  status = hasp_logout(h->handle);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  args.GetReturnValue().Set(Boolean::New(isolate, !status));
} 

void Hasp::get_size(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  Hasp* h = ObjectWrap::Unwrap<Hasp>(args.Holder());
  int32_t fsize = static_cast<int32_t>(h->get_size(isolate));
  args.GetReturnValue().Set(Integer::New(isolate, fsize));
}

hasp_size_t Hasp::get_size(Isolate* isolate) {
  hasp_size_t fsize;
  const hasp_status_t status = hasp_get_size(handle, HASP_FILEID_RW, &fsize);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  return fsize;
}

char* Hasp::decrypt(Isolate* isolate, char* data, size_t length) {
  const hasp_status_t status = hasp_decrypt(handle, data, length);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  return data;
}

char* Hasp::unwrap_decrypt(Isolate* isolate, char* data, size_t &length) {
  memcpy(&length, data, __SIZEOF_SIZE_T__);
  size_t crypto_length = get_crypto_length(length);
  char* content = new char[crypto_length];
  memcpy(content, data + __SIZEOF_SIZE_T__, crypto_length);
  decrypt(isolate, content, crypto_length);
  return content;
}

void Hasp::read(const FunctionCallbackInfo<Value>& args) {
  Hasp* h = ObjectWrap::Unwrap<Hasp>(args.Holder());
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  hasp_status_t status;
  hasp_size_t fsize = h->get_size(isolate);

  char* data = new char[fsize];
  status = hasp_read(h->handle, HASP_FILEID_RW, 0, fsize, data);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }

  size_t length;
  char* content = h->unwrap_decrypt(isolate, data, length);
  args.GetReturnValue().Set(String::NewFromUtf8(
    isolate, content, String::kNormalString, length
  ));
  delete data;
  delete content;
}

char* Hasp::encrypt(Isolate* isolate, char* data, size_t length) {
  const hasp_status_t status = hasp_encrypt(handle, data, length);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  return data;
}

char* Hasp::encrypt_wrap(Isolate* isolate, char* content, size_t length) {
  size_t crypto_length = get_crypto_length(length);
  encrypt(isolate, content, crypto_length);
  char* wrapped = new char[crypto_length + __SIZEOF_SIZE_T__];
  memcpy(wrapped, &length, __SIZEOF_SIZE_T__);
  memcpy(wrapped + __SIZEOF_SIZE_T__, content, crypto_length);
  return wrapped;
}

void Hasp::write(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "First argument should be a string")));
    return;
  }

  Hasp* h = ObjectWrap::Unwrap<Hasp>(args.Holder());
  String::Utf8Value input(args[0]);
  hasp_status_t status;
  hasp_size_t fsize = h->get_size(isolate);

  size_t length = input.length();
  if (length + __SIZEOF_SIZE_T__ > fsize) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, "Storage max size exceeded")
    ));
  }

  char* data = h->encrypt_wrap(isolate, *input, length);
  status = hasp_write(h->handle, HASP_FILEID_RW, 0, fsize, data);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  delete data;
}
