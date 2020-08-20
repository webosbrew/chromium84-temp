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

/*
 * These #defines alter the behavior of webossystem_lttng_provider.h to define
 * the tracing primitives rather than just declaring them.
 */
#define TRACEPOINT_DEFINE
#define TRACEPOINT_PROBE_DYNAMIC_LINKAGE

/* The header containing our TRACEPOINT_EVENTs. */
#include "base/trace_event/neva/lttng/webos/webossystem_lttng_provider.h"
