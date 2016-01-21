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
  h->hasp.login(*vender_code);
  if (h->hasp.is_error()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, h->hasp.get_message())
    ));
  }
  args.GetReturnValue().Set(Boolean::New(isolate, !status));
}

void NodeHasp::logout(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  h->hasp.logout();
  if (h->hasp.is_error()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, hasp.get_message())
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

hasp_size_t HaspNode::get_size(Isolate* isolate) {
  hasp_size_t fsize;
  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  fsize = h->hasp.get_size();
  if (h->hasp.is_error()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, h->hasp.get_message())
    ));
  }
  return fsize;
}

void NodeHasp::read(const FunctionCallbackInfo<Value>& args) {
  NodeHasp* h = ObjectWrap::Unwrap<NodeHasp>(args.Holder());
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  hasp_status_t status;
  hasp_size_t fsize = h->get_size(isolate);
  char* content = h->hasp.read();
  if (h->hasp.is_error()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, h->hasp.get_message())
    ));
  }
  size_t length;
  args.GetReturnValue().Set(String::NewFromUtf8(
    isolate, content, String::kNormalString, length
  ));
  delete content;
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
  h->hasp.write(*input);


  if (h->hasp.is_error) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, h->hasp.get_message())
    ));
  }
  delete data;
}
