#ifndef BATON_H
#define BATON_H

#include <v8.h>
#include <uv.h>
#include <node.h>
#include "hasp_api.h"

struct Baton {
  uv_work_t request;
  v8::Persistent<v8::Function> callback;

  hasp_status_t status;
  char* vendor_code;
  v8::Persistent<v8::Object> holder;
  Hasp* hasp;
};

#endif // BATON_H
