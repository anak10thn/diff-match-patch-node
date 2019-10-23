#ifndef DIFF_H
#define DIFF_H

#include <node.h>
#include <node_object_wrap.h>
#include "libdiff.h"

namespace diff
{

using namespace std;

class Diff : public node::ObjectWrap
{
  public:
    static void Init(v8::Local<v8::Object> exports);

  private:
    explicit Diff(string orig, string modif);
    ~Diff();

    static void New(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void ToHTML(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void PatchCreate(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void PatchApply(const v8::FunctionCallbackInfo<v8::Value> &args);
    static v8::Persistent<v8::Function> constructor;
    string _orig;
    string _modif;
};

} // namespace diff

#endif