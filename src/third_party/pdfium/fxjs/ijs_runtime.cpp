// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxjs/ijs_runtime.h"

#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fxjs/cjs_runtimestub.h"
#include "third_party/base/ptr_util.h"

#ifdef PDF_ENABLE_V8
#include "fxjs/cfxjs_engine.h"
#include "fxjs/cjs_runtime.h"
#endif

// static
void IJS_Runtime::Initialize(unsigned int slot, void* isolate) {
#ifdef PDF_ENABLE_V8
  FXJS_Initialize(slot, static_cast<v8::Isolate*>(isolate));
#endif
}

// static
void IJS_Runtime::Destroy() {
#ifdef PDF_ENABLE_V8
  FXJS_Release();
#endif
}

// static
std::unique_ptr<IJS_Runtime> IJS_Runtime::Create(
    CPDFSDK_FormFillEnvironment* pFormFillEnv) {
#ifdef PDF_ENABLE_V8
  if (pFormFillEnv->IsJSPlatformPresent())
    return pdfium::MakeUnique<CJS_Runtime>(pFormFillEnv);
#endif
  return pdfium::MakeUnique<CJS_RuntimeStub>(pFormFillEnv);
}

IJS_Runtime::~IJS_Runtime() = default;
