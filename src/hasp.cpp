#include "hasp.h"
#include <cstring>

using namespace v8;

static unsigned char vendor_code[] =
  "AzIceaqfA1hX5wS+M8cGnYh5ceevUnOZIzJBbXFD6dgf3tBkb9cvUF/Tkd/iKu2fsg9wAysYKw7"
  "RMAsVvIp4KcXle/v1RaXrLVnNBJ2H2DmrbUMOZbQUFXe698qmJsqNpLXRA367xpZ54i8kC5DTXw"
  "DhfxWTOZrBrh5sRKHcoVLumztIQjgWh37AzmSd1bLOfUGI0xjAL9zJWO3fRaeB0NS2KlmoKaVT5"
  "Y04zZEc06waU2r6AU2Dc4uipJqJmObqKM+tfNKAS0rZr5IudRiC7pUwnmtaHRe5fgSI8M7yvypv"
  "m+13Wm4Gwd4VnYiZvSxf8ImN3ZOG9wEzfyMIlH2+rKPUVHI+igsqla0Wd9m7ZUR9vFotj1uYV0O"
  "zG7hX0+huN2E/IdgLDjbiapj1e2fKHrMmGFaIvI6xzzJIQJF9GiRZ7+0jNFLKSyzX/K3JAyFrIP"
  "ObfwM+y+zAgE1sWcZ1YnuBhICyRHBhaJDKIZL8MywrEfB2yF+R3k9wFG1oN48gSLyfrfEKuB/qg"
  "Np+BeTruWUk0AwRE9XVMUuRbjpxa4YA67SKunFEgFGgUfHBeHJTivvUl0u4Dki1UKAT973P+nXy"
  "2O0u239If/kRpNUVhMg8kpk7s8i6Arp7l/705/bLCx4kN5hHHSXIqkiG9tHdeNV8VYo5+72hgaC"
  "x3/uVoVLmtvxbOIvo120uTJbuLVTvT8KtsOlb3DxwUrwLzaEMoAQAFk6Q9bNipHxfkRQER4kR7I"
  "YTMzSoW5mxh3H9O8Ge5BqVeYMEW36q9wnOYfxOLNw6yQMf8f9sJN4KhZty02xm707S7VEfJJ1KN"
  "q7b5pP/3RjE0IKtB2gE6vAPRvRLzEohu0m7q1aUp8wAvSiqjZy7FLaTtLEApXYvLvz6PEJdj4Te"
  "gCZugj7c8bIOEqLXmloZ6EgVnjQ7/ttys7VFITB3mazzFiyQuKf4J6+b/a/Y";

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
  args.GetReturnValue().Set(Boolean::New(isolate, !status));
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

char* Hasp::unwrap_decrypt(Isolate* isolate, char* data) {
  size_t length;
  memcpy(&length, data, __SIZEOF_SIZE_T__);
  char* content = new char[length];
  memcpy(content, data + __SIZEOF_SIZE_T__, length);
  decrypt(isolate, content, length);
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

  char* content = h->unwrap_decrypt(isolate, data);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, content));
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
  encrypt(isolate, content, length);
  char* wrapped = new char[length + __SIZEOF_SIZE_T__];
  memcpy(wrapped, &length, __SIZEOF_SIZE_T__);
  memcpy(wrapped + __SIZEOF_SIZE_T__, content, length);
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

  size_t length = input.length() > 16 ? input.length() : 16;
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
