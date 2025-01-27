// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/gpu/android/codec_image.h"

#include <string.h>

#include <memory>

#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "media/gpu/android/surface_texture_gl_owner.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/scoped_make_current.h"

namespace media {
namespace {

// Makes |surface_texture|'s context current if it isn't already.
std::unique_ptr<ui::ScopedMakeCurrent> MakeCurrentIfNeeded(
    SurfaceTextureGLOwner* surface_texture) {
  // Note: this works for virtual contexts too, because IsCurrent() returns true
  // if their shared platform context is current, regardless of which virtual
  // context is current.
  return std::unique_ptr<ui::ScopedMakeCurrent>(
      surface_texture->GetContext()->IsCurrent(nullptr)
          ? nullptr
          : new ui::ScopedMakeCurrent(surface_texture->GetContext(),
                                      surface_texture->GetSurface()));
}

}  // namespace

CodecImage::CodecImage(
    std::unique_ptr<CodecOutputBuffer> output_buffer,
    scoped_refptr<SurfaceTextureGLOwner> surface_texture,
    PromotionHintAggregator::NotifyPromotionHintCB promotion_hint_cb)
    : phase_(Phase::kInCodec),
      output_buffer_(std::move(output_buffer)),
      surface_texture_(std::move(surface_texture)),
      promotion_hint_cb_(std::move(promotion_hint_cb)) {}

CodecImage::~CodecImage() {
  if (destruction_cb_)
    std::move(destruction_cb_).Run(this);
}

void CodecImage::SetDestructionCb(DestructionCb destruction_cb) {
  destruction_cb_ = std::move(destruction_cb);
}

gfx::Size CodecImage::GetSize() {
  // Return a nonzero size, to avoid GL errors, even if we dropped the codec
  // buffer already.  Note that if we dropped it, there's no data in the
  // texture anyway, so the old size doesn't matter.
  return output_buffer_ ? output_buffer_->size() : gfx::Size(1, 1);
}

unsigned CodecImage::GetInternalFormat() {
  return GL_RGBA;
}

bool CodecImage::BindTexImage(unsigned target) {
  return false;
}

void CodecImage::ReleaseTexImage(unsigned target) {}

bool CodecImage::CopyTexImage(unsigned target) {
  if (!surface_texture_ || target != GL_TEXTURE_EXTERNAL_OES)
    return false;

  GLint bound_service_id = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &bound_service_id);
  // The currently bound texture should be the surface texture's texture.
  if (bound_service_id != static_cast<GLint>(surface_texture_->GetTextureId()))
    return false;

  RenderToSurfaceTextureFrontBuffer(BindingsMode::kDontRestore);
  return true;
}

bool CodecImage::CopyTexSubImage(unsigned target,
                                 const gfx::Point& offset,
                                 const gfx::Rect& rect) {
  return false;
}

bool CodecImage::ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
                                      int z_order,
                                      gfx::OverlayTransform transform,
                                      const gfx::Rect& bounds_rect,
                                      const gfx::RectF& crop_rect,
                                      bool enable_blend) {
  if (surface_texture_) {
    DVLOG(1) << "Invalid call to ScheduleOverlayPlane; this image is "
                "SurfaceTexture backed.";
    return false;
  }

  // Move the overlay if needed.
  if (most_recent_bounds_ != bounds_rect) {
    most_recent_bounds_ = bounds_rect;
    // Note that, if we're actually promoted to overlay, that this is where the
    // hint is sent to the callback.  NotifyPromotionHint detects this case and
    // lets us do it.  If we knew that we were going to get promotion hints,
    // then we could always let NotifyPromotionHint do it.  Unfortunately, we
    // don't know that.
    promotion_hint_cb_.Run(PromotionHintAggregator::Hint(bounds_rect, true));
  }

  RenderToOverlay();
  return true;
}

void CodecImage::OnMemoryDump(base::trace_event::ProcessMemoryDump* pmd,
                              uint64_t process_tracing_id,
                              const std::string& dump_name) {}

void CodecImage::GetTextureMatrix(float matrix[16]) {
  // Default to identity.
  static constexpr float kYInvertedIdentity[16]{
      1, 0,  0, 0,  //
      0, -1, 0, 0,  //
      0, 0,  1, 0,  //
      0, 1,  0, 1   //
  };
  memcpy(matrix, kYInvertedIdentity, sizeof(kYInvertedIdentity));
  if (!surface_texture_)
    return;

  // The matrix is available after we render to the front buffer. If that fails
  // we'll return the matrix from the previous frame, which is more likely to be
  // correct than the identity matrix anyway.
  RenderToSurfaceTextureFrontBuffer(BindingsMode::kDontRestore);
  surface_texture_->GetTransformMatrix(matrix);
  YInvertMatrix(matrix);
}

void CodecImage::NotifyPromotionHint(bool promotion_hint,
                                     int display_x,
                                     int display_y,
                                     int display_width,
                                     int display_height) {
  // If this is promotable, and we're using an overlay, then skip sending this
  // hint.  ScheduleOverlayPlane will do it.
  if (promotion_hint && !surface_texture_)
    return;

  promotion_hint_cb_.Run(PromotionHintAggregator::Hint(
      gfx::Rect(display_x, display_y, display_width, display_height),
      promotion_hint));
}

bool CodecImage::RenderToFrontBuffer() {
  return surface_texture_
             ? RenderToSurfaceTextureFrontBuffer(BindingsMode::kRestore)
             : RenderToOverlay();
}

bool CodecImage::RenderToSurfaceTextureBackBuffer() {
  DCHECK(surface_texture_);
  DCHECK_NE(phase_, Phase::kInFrontBuffer);
  if (phase_ == Phase::kInBackBuffer)
    return true;
  if (phase_ == Phase::kInvalidated)
    return false;

  // Wait for a previous frame available so we don't confuse it with the one
  // we're about to release.
  if (surface_texture_->IsExpectingFrameAvailable())
    surface_texture_->WaitForFrameAvailable();
  if (!output_buffer_->ReleaseToSurface()) {
    phase_ = Phase::kInvalidated;
    return false;
  }
  phase_ = Phase::kInBackBuffer;
  surface_texture_->SetReleaseTimeToNow();
  return true;
}

bool CodecImage::RenderToSurfaceTextureFrontBuffer(BindingsMode bindings_mode) {
  DCHECK(surface_texture_);
  if (phase_ == Phase::kInFrontBuffer)
    return true;
  if (phase_ == Phase::kInvalidated)
    return false;

  // Render it to the back buffer if it's not already there.
  if (!RenderToSurfaceTextureBackBuffer())
    return false;

  // The image is now in the back buffer, so promote it to the front buffer.
  phase_ = Phase::kInFrontBuffer;
  if (surface_texture_->IsExpectingFrameAvailable())
    surface_texture_->WaitForFrameAvailable();

  std::unique_ptr<ui::ScopedMakeCurrent> scoped_make_current =
      MakeCurrentIfNeeded(surface_texture_.get());
  // If we have to switch contexts, then we always want to restore the
  // bindings.
  bool should_restore_bindings =
      bindings_mode == BindingsMode::kRestore || !!scoped_make_current;

  GLint bound_service_id = 0;
  if (should_restore_bindings)
    glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &bound_service_id);
  surface_texture_->UpdateTexImage();
  if (should_restore_bindings)
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, bound_service_id);
  return true;
}

bool CodecImage::RenderToOverlay() {
  if (phase_ == Phase::kInFrontBuffer)
    return true;
  if (phase_ == Phase::kInvalidated)
    return false;

  if (!output_buffer_->ReleaseToSurface()) {
    phase_ = Phase::kInvalidated;
    return false;
  }
  phase_ = Phase::kInFrontBuffer;
  return true;
}

void CodecImage::SurfaceDestroyed() {
  output_buffer_ = nullptr;
  phase_ = Phase::kInvalidated;
}

}  // namespace media
