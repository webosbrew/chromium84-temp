// Copyright 2015-2019 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

// Tracepoint API for using LTTng UST tracing in webossystem-injection
//
// For more information on see:
//    http://lttng.org/files/doc/man-pages/man3/lttng-ust.3.html

#ifndef BASE_TRACE_EVENT_NEVA_LTTNG_WEBOS_PMTRACER_H_
#define BASE_TRACE_EVENT_NEVA_LTTNG_WEBOS_PMTRACER_H_

#if defined(USE_LTTNG)

#include "base/trace_event/neva/lttng/webos/webossystem_lttng_provider.h"

// PMTRACE_LOG is for free form tracing. Provide a string in
// "label" which uniquely identifies your trace point.
#define PMTRACE_LOG(label) \
    tracepoint(webossystem_lttng_provider, message, label)

// PMTRACE_ITEM is for tracing (name, value) pairs.
// Both name and value are strings.
#define PMTRACE_ITEM(name, value) \
    tracepoint(webossystem_lttng_provider, item, name, value)

// PMTRACE_BEFORE / AFTER is for tracing a time duration
// which is not contained within a scope (curly braces) or function,
// or in C code where there is no mechanism to automatically detect
// exiting a scope or function.
#define PMTRACE_BEFORE(label) \
    tracepoint(webossystem_lttng_provider, before, label)
#define PMTRACE_AFTER(label) \
    tracepoint(webossystem_lttng_provider, after, label)

// PMTRACE_SCOPE* is for tracing a the duration of a scope.  In
// C++ code use PMTRACE_SCOPE only, in C code use the
// ENTRY/EXIT macros and be careful to catch all exit cases.
#define PMTRACE_SCOPE_ENTRY(label) \
    tracepoint(webossystem_lttng_provider, scope_entry, label)
#define PMTRACE_SCOPE_EXIT(label) \
    tracepoint(webossystem_lttng_provider, scope_exit, label)
#define PMTRACE_SCOPE(label) \
    PmTraceTraceScope traceScope(label)

// PMTRACE_FUNCTION* is for tracing a the duration of a scope.
// In C++ code use PMTRACE_FUNCTION only, in C code use the
// ENTRY/EXIT macros and be careful to catch all exit cases.
#define PMTRACE_FUNCTION_ENTRY(label) \
    tracepoint(webossystem_lttng_provider, function_entry, label)
#define PMTRACE_FUNCTION_EXIT(label) \
    tracepoint(webossystem_lttng_provider, function_exit, label)
#define PMTRACE_FUNCTION \
    PmTraceTraceFunction traceFunction(const_cast<char*>(Q_FUNC_INFO))

class PmTraceTraceScope {
 public:
  PmTraceTraceScope(char* label)
      : scopeLabel(label) {
    PMTRACE_SCOPE_ENTRY(scopeLabel);
  }

  ~PmTraceTraceScope() { PMTRACE_SCOPE_EXIT(scopeLabel); }

 private:
  char* scopeLabel;

  // Prevent heap allocation
  void operator delete(void*);
  void* operator new(size_t);
  PmTraceTraceScope(const PmTraceTraceScope&);
  PmTraceTraceScope& operator=(const PmTraceTraceScope&);
};

class PmTraceTraceFunction {
 public:
  PmTraceTraceFunction(char* label)
      : fnLabel(label) {
    PMTRACE_FUNCTION_ENTRY(fnLabel);
  }

  ~PmTraceTraceFunction() { PMTRACE_FUNCTION_EXIT(fnLabel); }

 private:
  char* fnLabel;

  // Prevent heap allocation
  void operator delete(void*);
  void* operator new(size_t);
  PmTraceTraceFunction(const PmTraceTraceFunction&);
  PmTraceTraceFunction& operator=(const PmTraceTraceFunction&);
};

#else  // !defined(USE_LTTNG)

#define PMTRACE_LOG(label)
#define PMTRACE_ITEM(name, value)
#define PMTRACE_BEFORE(label)
#define PMTRACE_AFTER(label)
#define PMTRACE_SCOPE_ENTRY(label)
#define PMTRACE_SCOPE_EXIT(label)
#define PMTRACE_SCOPE(label)
#define PMTRACE_FUNCTION_ENTRY(label)
#define PMTRACE_FUNCTION_EXIT(label)
#define PMTRACE_FUNCTION

#endif  // defined(USE_LTTNG)

#endif  // BASE_TRACE_EVENT_NEVA_LTTNG_WEBOS_PMTRACER_H_
