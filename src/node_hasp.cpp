#include "node_hasp.h"
#include <cstring>

using namespace v8;

#define get_crypto_length(len) len > 16 ? len : 16

Persistent<Function> NodeHasp::constructor;

NodeHasp::NodeHasp() {
}

NodeHasp::~NodeHasp() {
}

void NodeHasp::Init(Handle<Object> exports) {
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

void NodeHasp::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    NodeHasp* h = new NodeHasp();
    h->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void NodeHasp::login(const FunctionCallbackInfo<Value>& args) {
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

  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  String::Utf8Value vendor_code(args[0]);
  hasp_status_t status;
  status = hasp_login(HASP_DEFAULT_FID,
                  (hasp_vendor_code_t *) *vendor_code,
                  &h->handle);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  args.GetReturnValue().Set(Boolean::New(isolate, !status));
}

void NodeHasp::logout(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  hasp_status_t status;
  status = hasp_logout(h->handle);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  args.GetReturnValue().Set(Boolean::New(isolate, !status));
} 

void NodeHasp::get_size(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  int32_t fsize = static_cast<int32_t>(h->get_size(isolate));
  args.GetReturnValue().Set(Integer::New(isolate, fsize));
}

hasp_size_t NodeHasp::get_size(Isolate* isolate) {
  hasp_size_t fsize;
  const hasp_status_t status = hasp_get_size(handle, HASP_FILEID_RW, &fsize);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  return fsize;
}

char* NodeHasp::decrypt(Isolate* isolate, char* data, size_t length) {
  const hasp_status_t status = hasp_decrypt(handle, data, length);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  return data;
}

char* NodeHasp::unwrap_decrypt(Isolate* isolate, char* data, size_t &length) {
  memcpy(&length, data, __SIZEOF_SIZE_T__);
  size_t crypto_length = get_crypto_length(length);
  char* content = new char[crypto_length];
  memcpy(content, data + __SIZEOF_SIZE_T__, crypto_length);
  decrypt(isolate, content, crypto_length);
  return content;
}

void NodeHasp::read(const FunctionCallbackInfo<Value>& args) {
  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
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

char* NodeHasp::encrypt(Isolate* isolate, char* data, size_t length) {
  const hasp_status_t status = hasp_encrypt(handle, data, length);
  if (status) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp_statusmap[status])
    ));
  }
  return data;
}

char* NodeHasp::encrypt_wrap(Isolate* isolate, char* content, size_t length) {
  size_t crypto_length = get_crypto_length(length);
  encrypt(isolate, content, crypto_length);
  char* wrapped = new char[crypto_length + __SIZEOF_SIZE_T__];
  memcpy(wrapped, &length, __SIZEOF_SIZE_T__);
  memcpy(wrapped + __SIZEOF_SIZE_T__, content, crypto_length);
  return wrapped;
}

void NodeHasp::write(const FunctionCallbackInfo<Value>& args) {
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

  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
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
