#include "diff.h"

namespace diff
{

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;

Persistent<Function> Diff::constructor;

Diff::Diff(string orig, string modif) : _orig(orig), _modif(modif)
{
}

Diff::~Diff()
{
}

void Diff::Init(Local<Object> exports)
{
  Isolate *isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "Diff"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "toHTML", ToHTML);
  NODE_SET_PROTOTYPE_METHOD(tpl, "patchCreate", PatchCreate);
  NODE_SET_PROTOTYPE_METHOD(tpl, "patchApply", PatchApply);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "Diff"),
               tpl->GetFunction());
}

void Diff::New(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();

  if (args.IsConstructCall())
  {
    // Invoked as constructor: `new Diff(...)`
    // string origin = args[0]->ToString();
    // string modify = args[1]->ToString();
    String::Utf8Value origin(v8::Isolate::GetCurrent(), args[0]);
    String::Utf8Value modify(v8::Isolate::GetCurrent(), args[1]);
    Diff *obj = new Diff(string(*origin), string(*modify));
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  }
  else
  {
    // Invoked as plain function `Diff(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = {args[0]};
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    Local<Object> result =
        cons->NewInstance(context, argc, argv).ToLocalChecked();
    args.GetReturnValue().Set(result);
  }
}

void Diff::ToHTML(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();

  Diff *obj = ObjectWrap::Unwrap<Diff>(args.Holder());
  diff_match_patch<string> dmp;
  dmp.Diff_Timeout = 0.0;

  string strDiff = dmp.diff_prettyHtml(dmp.diff_main(obj->_orig, obj->_modif, false));

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, strDiff.c_str()));
}

void Diff::PatchCreate(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();

  Diff *obj = ObjectWrap::Unwrap<Diff>(args.Holder());
  diff_match_patch<string> dmp;
  string strPatch = dmp.patch_toText(dmp.patch_make(obj->_orig, obj->_modif));

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, strPatch.c_str()));
}

void Diff::PatchApply(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();

  Diff *obj = ObjectWrap::Unwrap<Diff>(args.Holder());
  diff_match_patch<string> dmp;
  string modif = obj->_modif;
  string orig = obj->_orig;
  pair<string, vector<bool> > out
       = dmp.patch_apply(dmp.patch_fromText(modif), orig);
  string strResult = out.first;

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, strResult.c_str()));
}

} // namespace diff