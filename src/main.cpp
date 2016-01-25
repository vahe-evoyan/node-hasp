#include <node.h>
#include "node_hasp.h"

using namespace v8;

void init_classes(Handle<Object> exports) {
    NodeHasp::Init(exports);
}

NODE_MODULE(hasp, init_classes)
