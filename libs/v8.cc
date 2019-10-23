#include <node.h>
#include "diff.h"

namespace diff {

using v8::Local;
using v8::Object;

void InitAll(Local<Object> exports) {
  Diff::Init(exports);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, InitAll)

}