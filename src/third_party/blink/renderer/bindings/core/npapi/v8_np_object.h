/*
 * Copyright (C) 2006, 2007, 2008, 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_NPAPI_V8_NP_OBJECT_H_
#define THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_NPAPI_V8_NP_OBJECT_H_

#include "third_party/blink/renderer/core/core_export.h"
#include <v8.h>

struct NPObject;

namespace blink {

// Get a wrapper for a NPObject.
// If the object is already wrapped, the pre-existing wrapper will be
// returned. If the object is not wrapped, wrap it, and give V8 a weak
// reference to the wrapper which will cleanup when there are no more
// JS references to the object.
CORE_EXPORT v8::Local<v8::Object> CreateV8ObjectForNPObject(v8::Isolate*,
    NPObject*, NPObject* root);

// Tell V8 to forcibly remove an object.
// This is used at plugin teardown so that the caller can aggressively
// unload the plugin library. After calling this function, the persistent
// handle to the wrapper will be gone, and the wrapped NPObject will be
// removed so that it cannot be referred to.
CORE_EXPORT void ForgetV8ObjectForNPObject(NPObject*);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_NPAPI_V8_NP_OBJECT_H_
