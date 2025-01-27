// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/script/module_pending_script.h"

#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/script/script_loader.h"

namespace blink {

ModulePendingScriptTreeClient::ModulePendingScriptTreeClient() {}

void ModulePendingScriptTreeClient::SetPendingScript(
    ModulePendingScript* pending_script) {
  DCHECK(!pending_script_);
  pending_script_ = pending_script;

  if (finished_) {
    pending_script_->NotifyModuleTreeLoadFinished();
  }
}

void ModulePendingScriptTreeClient::NotifyModuleTreeLoadFinished(
    ModuleScript* module_script) {
  DCHECK(!finished_);
  finished_ = true;
  module_script_ = module_script;

  if (pending_script_)
    pending_script_->NotifyModuleTreeLoadFinished();
}

void ModulePendingScriptTreeClient::Trace(blink::Visitor* visitor) {
  visitor->Trace(module_script_);
  visitor->Trace(pending_script_);
  ModuleTreeClient::Trace(visitor);
}

void ModulePendingScriptTreeClient::TraceWrappers(
    ScriptWrappableVisitor* visitor) const {
  visitor->TraceWrappers(module_script_);
  visitor->TraceWrappers(pending_script_);
  ModuleTreeClient::TraceWrappers(visitor);
}

ModulePendingScript::ModulePendingScript(ScriptElementBase* element,
                                         ModulePendingScriptTreeClient* client,
                                         bool is_external)
    : PendingScript(element, TextPosition()),
      module_tree_client_(client),
      is_external_(is_external) {
  CHECK(GetElement());
  DCHECK(module_tree_client_);
  client->SetPendingScript(this);
}

ModulePendingScript::~ModulePendingScript() {}

void ModulePendingScript::DisposeInternal() {
  module_tree_client_ = nullptr;
}

void ModulePendingScript::Trace(blink::Visitor* visitor) {
  visitor->Trace(module_tree_client_);
  PendingScript::Trace(visitor);
}

void ModulePendingScript::TraceWrappers(ScriptWrappableVisitor* visitor) const {
  visitor->TraceWrappers(module_tree_client_);
  PendingScript::TraceWrappers(visitor);
}

void ModulePendingScript::NotifyModuleTreeLoadFinished() {
  CHECK(!IsReady());
  ready_ = true;

  if (Client())
    Client()->PendingScriptFinished(this);
}

bool ModulePendingScript::CheckMIMETypeBeforeRunScript(Document*) const {
  // We don't check MIME type here because we check the MIME type in
  // ModuleScriptLoader::WasModuleLoadSuccessful().
  return true;
}

Script* ModulePendingScript::GetSource(const KURL& document_url,
                                       bool& error_occurred) const {
  CHECK(IsReady());
  error_occurred = ErrorOccurred();
  return GetModuleScript();
}

bool ModulePendingScript::ErrorOccurred() const {
  CHECK(IsReady());
  return !GetModuleScript();
}

}  // namespace blink
