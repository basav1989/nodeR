#include <node.h>
#include <dlfcn.h>
#include "volatilityModel.h"

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Array;

using namespace v8;

  // The format to be returned back to node
  /* {
       "principalComponents": {
                                "comp1": {
                                            "EigenValue": "",
                                            "percentage of variance": ""
                                         },
                                "comp2": {
                                             "EigenValue": "",
                                             "percentage of variance": ""
                                         }
                              },
       "loadings": {
                     "variable 1": {
                                     "component1": "",
                                     "component2": ""
                                   },
                     "variable 2": {
                                     "component1": "",
                                     "component2": ""
                                   }
                   }
     }
  */

void runMethod(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  // prepare result to be returned back in above format
  Local<Object> result = Object::New(isolate);

  // extract configurations
  Handle<Object> obj = Handle<Object>::Cast(args[0]);
  Handle<Value> valDataset = obj->Get(String::NewFromUtf8(isolate, "datasetPath"));
  Handle<Value> valGraph = obj->Get(String::NewFromUtf8(isolate, "graphPath"));

  String::Utf8Value datasetPath(valDataset);
  String::Utf8Value graphPath(valGraph);

  std::cout << "datasetPath " + std::string(*datasetPath) + " graphPath " + std::string(*graphPath) << std::endl;

  // configure and run
  volatilityModel* vm = new volatilityModel();
  vm->configure(*graphPath, *datasetPath, true);
  vm->run();
  std::map<std::string, std::map<std::string, double> > principalComponentsMap = vm->getPrincipalComponents();
  std::map<std::string, std::map<std::string, double> > variableLoadingsMap = vm->getVariableLoadings();
  delete vm;

  // add principal components to results
  Local<Object> principalComponents = Object::New(isolate);
  for (auto iter = principalComponentsMap.begin(); iter != principalComponentsMap.end(); iter++) {
    std::cout << "component name " <<  (iter->first) << std::endl;
    Local<Object> component = Object::New(isolate);

    std::map<std::string, double> propertyMap = iter->second;
    for (auto it = propertyMap.begin(); it != propertyMap.end(); it++) {
      component->Set(String::NewFromUtf8(isolate, (it->first.c_str())),
                          Number::New(isolate, (it->second)));
    }
    principalComponents->Set(String::NewFromUtf8(isolate, (iter->first.c_str())), component);
  }
  result->Set(String::NewFromUtf8(isolate, "principalComponent"), principalComponents);

  // add variable loadings to results
  Local<Object> variableLoadings = Object::New(isolate);
  for (auto iter = variableLoadingsMap.begin(); iter != variableLoadingsMap.end(); iter++) {
    std::cout << "variable name " <<  (iter->first) << std::endl;
    Local<Object> loading = Object::New(isolate);

    std::map<std::string, double> propertyMap = iter->second;
    for (auto it = propertyMap.begin(); it != propertyMap.end(); it++) {
      loading->Set(String::NewFromUtf8(isolate, (it->first.c_str())),
                          Number::New(isolate, (it->second)));
    }
    variableLoadings->Set(String::NewFromUtf8(isolate, (iter->first.c_str())), loading);
  }
  result->Set(String::NewFromUtf8(isolate, "loading"), variableLoadings);

  // return matrix
  args.GetReturnValue().Set(result);
}

void configureMethod(const FunctionCallbackInfo<Value>& args) {
  // placeholder
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "run", runMethod);
  NODE_SET_METHOD(exports, "configure", configureMethod);
}

NODE_MODULE(addon, init)
