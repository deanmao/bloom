/* This code is PUBLIC DOMAIN, and is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND. See the accompanying 
 * LICENSE file.
 */

#include <v8.h>
#include <node.h>
#include <iostream>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <deque>
#include <set>
#include <string>

#include "bloom_filter.hpp"

using namespace node;
using namespace v8;

class BloomFilter: ObjectWrap
{
private:
  bloom_filter* thefilter;
public:

  static Persistent<FunctionTemplate> s_ct;
  static void Init(Handle<Object> target)
  {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);

    s_ct = Persistent<FunctionTemplate>::New(t);
    s_ct->InstanceTemplate()->SetInternalFieldCount(1);
    s_ct->SetClassName(String::NewSymbol("BloomFilter"));

    NODE_SET_PROTOTYPE_METHOD(s_ct, "insert", Insert);
    NODE_SET_PROTOTYPE_METHOD(s_ct, "contains", Contains);

    target->Set(String::NewSymbol("BloomFilter"),
                s_ct->GetFunction());
  }

  BloomFilter(const std::size_t& size)
  {
    unsigned int random_seed = 0xA57EC3B2;
    const double desired_probability_of_false_positive = 1.0 / size;
    this->thefilter = new bloom_filter(size, desired_probability_of_false_positive, random_seed);
  }

  ~BloomFilter()
  {
    if(this->thefilter != NULL) free(this->thefilter);
  }

  static Handle<Value> New(const Arguments& args)
  {
    HandleScope scope;
    if(args.Length() == 1 && args[0]->IsNumber()) {
      int value = args[0]->Int32Value();
      BloomFilter* hw = new BloomFilter((size_t) value);
      hw->Wrap(args.This());
    } else {
      BloomFilter* hw = new BloomFilter((size_t) 10000);
      hw->Wrap(args.This());
    }
    return args.This();
  }

  static Handle<Value> Insert(const Arguments& args)
  {
    bool success = false;
    HandleScope scope;
    BloomFilter* hw = ObjectWrap::Unwrap<BloomFilter>(args.This());
    if (hw->thefilter == NULL) {
      return scope.Close(Boolean::New(false));
    }
    if(args.Length() == 1 && args[0]->IsString()) {
      Local<String> str = args[0]->ToString();
      char *stringBytes = (char *)malloc(str->Length() + 1);
      *(stringBytes + str->Length()) = '\0';
      node::DecodeWrite(stringBytes, str->Length(), str, node::BINARY);
      hw->thefilter->insert(stringBytes, (str->Length() + 1));
      success = true;
    }
    return scope.Close(Boolean::New(success));
  }

  static Handle<Value> Contains(const Arguments& args)
  {
    bool result = false;
    HandleScope scope;
    BloomFilter* hw = ObjectWrap::Unwrap<BloomFilter>(args.This());
    if (hw->thefilter == NULL) {
      return scope.Close(Boolean::New(false));
    }
    if(args.Length() == 1 && args[0]->IsString()) {
      Local<String> str = args[0]->ToString();
      char *stringBytes = (char *)malloc(str->Length() + 1);
      *(stringBytes + str->Length()) = '\0';
      node::DecodeWrite(stringBytes, str->Length(), str, node::BINARY);
      if(hw->thefilter->contains(stringBytes, (str->Length() + 1))) {
        result = true;
      }
    }
    return scope.Close(Boolean::New(result));
  }
};

Persistent<FunctionTemplate> BloomFilter::s_ct;

extern "C" {
  static void init (Handle<Object> target)
  {
    BloomFilter::Init(target);
  }

  NODE_MODULE(bloomfilter, init);
}
