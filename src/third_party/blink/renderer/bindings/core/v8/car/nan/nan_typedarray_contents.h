/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2018 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

#ifndef NAN_TYPEDARRAY_CONTENTS_H_
#define NAN_TYPEDARRAY_CONTENTS_H_

template<typename T>
class TypedArrayContents {
 public:
  inline explicit TypedArrayContents(v8::Local<v8::Value> from) :
      length_(0), data_(NULL) {
    HandleScope scope;

    size_t length = 0;
    void*  data = NULL;


    if (from->IsArrayBufferView()) {
      v8::Local<v8::ArrayBufferView> array =
        v8::Local<v8::ArrayBufferView>::Cast(from);

      const size_t    byte_length = array->ByteLength();
      const ptrdiff_t byte_offset = array->ByteOffset();
      v8::Local<v8::ArrayBuffer> buffer = array->Buffer();

      length = byte_length / sizeof(T);
      data   = static_cast<char*>(buffer->GetContents().Data()) + byte_offset;
    }


#if defined(_MSC_VER) && _MSC_VER >= 1900 || __cplusplus >= 201103L
    assert(reinterpret_cast<uintptr_t>(data) % alignof (T) == 0);
#elif defined(_MSC_VER) && _MSC_VER >= 1600 || defined(__GNUC__)
    assert(reinterpret_cast<uintptr_t>(data) % __alignof(T) == 0);
#else
    assert(reinterpret_cast<uintptr_t>(data) % sizeof (T) == 0);
#endif

    length_ = length;
    data_   = static_cast<T*>(data);
  }

  inline size_t length() const      { return length_; }
  inline T* operator*()             { return data_;   }
  inline const T* operator*() const { return data_;   }

 private:
  NAN_DISALLOW_ASSIGN_COPY_MOVE(TypedArrayContents)

  //Disable heap allocation
  void *operator new(size_t size);
  void operator delete(void *, size_t) {
    abort();
  }

  size_t  length_;
  T*      data_;
};

#endif  // NAN_TYPEDARRAY_CONTENTS_H_
