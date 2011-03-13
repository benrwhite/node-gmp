
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

#include "node_gmp.h"


using namespace v8;
using namespace node;

#define GETARG(arg,t)                                                            \
  if (arg->IsNumber() || arg->IsString()) {                                      \
    String::Utf8Value val(arg->ToString());                                      \
    char * num = t;                                                              \
    try {                                                                        \
      i = num;                                                                   \
    } catch (std::invalid_argument err) {                                        \
      return ThrowException(Exception::TypeError(String::New("bad argument")));  \
    }                                                                            \
  } else if (!(arg->IsUndefined() || arg->IsNull())) {                           \
    return ThrowException(Exception::TypeError(String::New("bad argument")));    \
  }


Handle<Value>
GInt::New(const Arguments &args) {
  HandleScope scope;

  mpz_class i = 0;

  GETARG(args[0], strtok(*val, "."));

  GInt *g = new GInt(i);
  g->Wrap(args.This());
  return args.This();
}


GInt::GInt(mpz_class val): ObjectWrap() {
  val_ = val;
}

GInt::~GInt(){

}


Handle<Value>
GInt::Add(const Arguments &args) {
  HandleScope scope;

  mpz_class i = 0;

  GETARG(args[0], strtok(*val, "."));

  GInt *self = ObjectWrap::Unwrap<GInt>(args.This());
  self->val_ += i;

  return args.This();
}

Handle<Value>
GInt::Sub(const Arguments &args) {
  HandleScope scope;

  mpz_class i = 0;

  GETARG(args[0], strtok(*val, "."));

  GInt *self = ObjectWrap::Unwrap<GInt>(args.This());
  self->val_ -= i;

  return args.This();
}

Handle<Value>
GInt::Mul(const Arguments &args) {
  HandleScope scope;

  mpz_class i = 0;

  GETARG(args[0], strtok(*val, "."));

  GInt *self = ObjectWrap::Unwrap<GInt>(args.This());
  self->val_ *= i;

  return args.This();
}

Handle<Value>
GInt::Div(const Arguments &args) {
  HandleScope scope;

  mpz_class i = 0;

  GETARG(args[0], strtok(*val, "."));

  GInt *self = ObjectWrap::Unwrap<GInt>(args.This());
  self->val_ /= i;

  return args.This();
}

Handle<Value>
GInt::Pow(const Arguments &args) {
  HandleScope scope;

  if (!args[0]->IsNumber()) {
    return ThrowException(Exception::TypeError(String::New("exponent must be an int")));
  }

  GInt *self = ObjectWrap::Unwrap<GInt>(args.This());

  mpz_t c;
  mpz_init(c);

  mpz_pow_ui(c, self->val_.get_mpz_t(), (long)args[0]->Int32Value());

  self->val_ = mpz_class(c);

  mpz_clear(c);

  return args.This();
}

Handle<Value>
GInt::ToString(const Arguments &args) {
  HandleScope scope;

  GInt *self = ObjectWrap::Unwrap<GInt>(args.This());

  Local<String> str = String::New(self->val_.get_str(10).c_str());
  return scope.Close(str);
}

Handle<Value>
GFloat::New(const Arguments &args) {
  HandleScope scope;

  mpf_class i = 0;

  GETARG(args[0], *val);

  GFloat *g = new GFloat(i);
  g->Wrap(args.This());
  return args.This();
}


GFloat::GFloat(mpf_class val): ObjectWrap() {
  val_ = val;
}

GFloat::~GFloat(){

}


Handle<Value>
GFloat::Add(const Arguments &args) {
  HandleScope scope;

  mpf_class i = 0;

  GETARG(args[0], *val);

  GFloat *self = ObjectWrap::Unwrap<GFloat>(args.This());
  self->val_ += i;

  return args.This();
}

Handle<Value>
GFloat::Sub(const Arguments &args) {
  HandleScope scope;

  mpf_class i = 0;

  GETARG(args[0], *val);

  GFloat *self = ObjectWrap::Unwrap<GFloat>(args.This());
  self->val_ -= i;

  return args.This();
}

Handle<Value>
GFloat::Mul(const Arguments &args) {
  HandleScope scope;

  mpf_class i = 0;

  GETARG(args[0], *val);

  GFloat *self = ObjectWrap::Unwrap<GFloat>(args.This());
  self->val_ *= i;

  return args.This();
}

Handle<Value>
GFloat::Div(const Arguments &args) {
  HandleScope scope;

  mpf_class i = 0;

  GETARG(args[0], *val);

  GFloat *self = ObjectWrap::Unwrap<GFloat>(args.This());
  self->val_ /= i;

  return args.This();
}

Handle<Value>
GFloat::Pow(const Arguments &args) {
  HandleScope scope;

  if (!args[0]->IsNumber()) {
    return ThrowException(Exception::TypeError(String::New("exponent must be an int")));
  }

  GFloat *self = ObjectWrap::Unwrap<GFloat>(args.This());

  mpf_t c;
  mpf_init(c);

  mpf_pow_ui(c, self->val_.get_mpf_t(), (long)args[0]->Int32Value());

  self->val_ = mpf_class(c);

  mpf_clear(c);

  return args.This();
}

Handle<Value>
GFloat::ToNumber(const Arguments &args) {
  HandleScope scope;
  GFloat *self = ObjectWrap::Unwrap<GFloat>(args.This());

  Local<Number> val = Number::New(self->val_.get_d());

  return scope.Close(val);
}

Handle<Value>
GFloat::ToString(const Arguments &args) {
  HandleScope scope;
  mp_exp_t exp;
  long int i;
  GFloat *self = ObjectWrap::Unwrap<GFloat>(args.This());
  const char *unfixed = self->val_.get_str(exp,10).c_str();
  char *buff = (char *)alloca(abs(exp) + 2 + strlen(unfixed));
  char *cp = buff;
  if(unfixed[0] == '-') { *cp++ = '-'; unfixed++; }
  if(exp<=0) {
    *cp++ = '0'; *cp++ = '.';
    i = exp;
    while(i++<0) { *cp++ = '0'; }
  }
  i = 0;
  while(i<exp || *unfixed) {
    if(i++ == exp && exp != 0) { *cp++ = '.'; }
    if(*unfixed) *cp++ = *unfixed++;
    else *cp++ = '0';
  }
  *cp = '\0';

  Local<String> val = String::New(buff);

  return scope.Close(val);
}



void RegisterModule(Handle<Object> target) {
  target->Set(String::NewSymbol("version"), String::New(gmp_version));

  //target->Set(String::NewSymbol("toHex"),   FunctionTemplate::New(ToHex)->GetFunction());

  Local<FunctionTemplate> t_int = FunctionTemplate::New(GInt::New);
  t_int->InstanceTemplate()->SetInternalFieldCount(1);
  t_int->SetClassName(String::NewSymbol("GInt"));

  NODE_SET_PROTOTYPE_METHOD(t_int, "add", GInt::Add);
  NODE_SET_PROTOTYPE_METHOD(t_int, "sub", GInt::Sub);
  NODE_SET_PROTOTYPE_METHOD(t_int, "mul", GInt::Mul);
  NODE_SET_PROTOTYPE_METHOD(t_int, "div", GInt::Div);
  NODE_SET_PROTOTYPE_METHOD(t_int, "pow", GInt::Pow);
  NODE_SET_PROTOTYPE_METHOD(t_int, "toString", GInt::ToString);

  target->Set(String::NewSymbol("Int"), t_int->GetFunction());

  Local<FunctionTemplate> t_float = FunctionTemplate::New(GFloat::New);
  t_float->InstanceTemplate()->SetInternalFieldCount(1);
  t_float->SetClassName(String::NewSymbol("GFloat"));

  NODE_SET_PROTOTYPE_METHOD(t_float, "add", GFloat::Add);
  NODE_SET_PROTOTYPE_METHOD(t_float, "sub", GFloat::Sub);
  NODE_SET_PROTOTYPE_METHOD(t_float, "mul", GFloat::Mul);
  NODE_SET_PROTOTYPE_METHOD(t_float, "div", GFloat::Div);
  NODE_SET_PROTOTYPE_METHOD(t_float, "pow", GFloat::Pow);
  NODE_SET_PROTOTYPE_METHOD(t_float, "toString", GFloat::ToString);
  NODE_SET_PROTOTYPE_METHOD(t_float, "toValue", GFloat::ToNumber);

  target->Set(String::NewSymbol("Float"), t_float->GetFunction());
}

NODE_MODULE(gmp, RegisterModule);

