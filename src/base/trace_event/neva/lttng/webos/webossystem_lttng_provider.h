/*
 * Copyright 2015-2019 LG Electronics, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Tracepoint provider file for LTTng UST tracing in webossystem-injection
 *
 * For more information on see:
 *    http://lttng.org/files/doc/man-pages/man3/lttng-ust.3.html
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER webossystem_lttng_provider

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE \
    base/trace_event/neva/lttng/webos/webossystem_lttng_provider.h

#if !defined(BASE_TRACE_EVENT_NEVA_LTTNG_WEBOS_WEBOSSYSTEM_LTTNG_PROVIDER_H_) || \
    defined(TRACEPOINT_HEADER_MULTI_READ)
#define BASE_TRACE_EVENT_NEVA_LTTNG_WEBOS_WEBOSSYSTEM_LTTNG_PROVIDER_H_

#include <lttng/tracepoint.h>

/* "message" tracepoint should be used for single event trace points */
TRACEPOINT_EVENT(
    webossystem_lttng_provider,
    message,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
/* "item" tracepoint should be used for name/value pairs */
TRACEPOINT_EVENT(
    webossystem_lttng_provider,
    item,
    TP_ARGS(char*, text1, char*, text2),
    TP_FIELDS(ctf_string(name, text1) ctf_string(value, text2)))
/*
 * "before"/"after" tracepoint should be used for measuring the duration of
 * something that doesn't correspond with a function call or scope
 */
TRACEPOINT_EVENT(
    webossystem_lttng_provider,
    before,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
TRACEPOINT_EVENT(
    webossystem_lttng_provider,
    after,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
/*
 * "scope_entry"/"scope_exit" tracepoints should be used only by
 * PmtraceTraceScope class to measure the duration of a scope within
 * a function in C++ code. In C code these may be used directly for
 * the same purpose, just make sure you trace any early exit from the
 * scope such as break statements or gotos.
 */
TRACEPOINT_EVENT(
    webossystem_lttng_provider,
    scope_entry,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
TRACEPOINT_EVENT(
    webossystem_lttng_provider,
    scope_exit,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
/*
 * "function_entry"/"function_exit" tracepoints should be used only by
 * PmtraceTraceFunction class to measure the duration of a function
 * in C++ code. In C code it may be used directly for the same
 * purpose, just make sure you capture any early exit from the
 * function such as return statements.
 */
TRACEPOINT_EVENT(
    webossystem_lttng_provider,
    function_entry,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))
TRACEPOINT_EVENT(
    webossystem_lttng_provider,
    function_exit,
    TP_ARGS(char*, text),
    TP_FIELDS(ctf_string(scope, text)))

#endif  /* BASE_TRACE_EVENT_NEVA_LTTNG_WEBOS_WEBOSSYSTEM_LTTNG_PROVIDER_H_ */

#include <lttng/tracepoint-event.h>
