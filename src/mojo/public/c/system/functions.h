// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains basic functions common to different Mojo system APIs.
//
// Note: This header should be compilable as C.

#ifndef MOJO_PUBLIC_C_SYSTEM_FUNCTIONS_H_
#define MOJO_PUBLIC_C_SYSTEM_FUNCTIONS_H_

#include <stddef.h>
#include <stdint.h>

#include "mojo/public/c/system/system_export.h"
#include "mojo/public/c/system/types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Note: Pointer parameters that are labelled "optional" may be null (at least
// under some circumstances). Non-const pointer parameters are also labeled
// "in", "out", or "in/out", to indicate how they are used. (Note that how/if
// such a parameter is used may depend on other parameters or the requested
// operation's success/failure. E.g., a separate |flags| parameter may control
// whether a given "in/out" parameter is used for input, output, or both.)

// Initializes Mojo in the calling application.
//
// With the exception of EDK embedders, applications using Mojo APIs must call
// this function before any others.
//
// |options| may be null.
//
// Returns:
//   |MOJO_RESULT_OK| if Mojo intiailization was successful.
//   |MOJO_RESULT_INVALID_ARGUMENT| if |options| was null or invalid.
//   |MOJO_RESULT_FAILED_PRECONDITION| if |MojoInitialize()| was already called
//       once or if the application already explicitly initialized a Mojo EDK
//       environment.
MOJO_SYSTEM_EXPORT MojoResult
MojoInitialize(const struct MojoInitializeOptions* options);

// Returns the time, in microseconds, since some undefined point in the past.
// The values are only meaningful relative to other values that were obtained
// from the same device without an intervening system restart. Such values are
// guaranteed to be monotonically non-decreasing with the passage of real time.
// Although the units are microseconds, the resolution of the clock may vary and
// is typically in the range of ~1-15 ms.
MOJO_SYSTEM_EXPORT MojoTimeTicks MojoGetTimeTicksNow(void);

// Closes the given |handle|.
//
// Returns:
//   |MOJO_RESULT_OK| on success.
//   |MOJO_RESULT_INVALID_ARGUMENT| if |handle| is not a valid handle.
//
// Concurrent operations on |handle| may succeed (or fail as usual) if they
// happen before the close, be cancelled with result |MOJO_RESULT_CANCELLED| if
// they properly overlap (this is likely the case with traps), or fail with
// |MOJO_RESULT_INVALID_ARGUMENT| if they happen after.
MOJO_SYSTEM_EXPORT MojoResult MojoClose(MojoHandle handle);

// Queries the last known signals state of a handle.
//
// Note that no guarantees can be made about the accuracy of the returned
// signals state by the time this returns, as other threads in the system may
// change the handle's state at any time. Use with appropriate discretion.
//
// Returns:
//   |MOJO_RESULT_OK| on success. |*signals_state| is populated with the
//       last known signals state of |handle|.
//   |MOJO_RESULT_INVALID_ARGUMENT| if |handle| is not a valid handle or
//       |signals_state| is null.
MOJO_SYSTEM_EXPORT MojoResult
MojoQueryHandleSignalsState(MojoHandle handle,
                            struct MojoHandleSignalsState* signals_state);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // MOJO_PUBLIC_C_SYSTEM_FUNCTIONS_H_
