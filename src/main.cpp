#include <node.h>
#include "hasp.h"

using namespace v8;

void init_classes(Handle<Object> exports) {
    Hasp::Init(exports);
}

NODE_MODULE(hasp, init_classes)
