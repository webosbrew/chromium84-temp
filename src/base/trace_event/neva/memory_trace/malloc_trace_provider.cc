// Copyright 2018-2019 LG Electronics, Inc.
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

#include "base/trace_event/neva/memory_trace/malloc_trace_provider.h"

#include <malloc.h>
#include <stddef.h>

#include "base/allocator/allocator_extension.h"
#include "base/allocator/allocator_shim.h"
#include "base/trace_event/neva/memory_trace/memory_trace_manager.h"
#include "build/build_config.h"

namespace base {
namespace trace_event {
namespace neva {

// static
MallocTraceProvider* MallocTraceProvider::GetInstance() {
  return Singleton<MallocTraceProvider,
                   LeakySingletonTraits<MallocTraceProvider>>::get();
}

MallocTraceProvider::MallocTraceProvider() {}

MallocTraceProvider::~MallocTraceProvider() {}

bool MallocTraceProvider::OnMemoryTrace() {
  const char* print_fmt;
  size_t total_virtual_size = 0;
  size_t resident_size = 0;
  size_t allocated_objects_size = 0;

  MemoryTraceManager* mtm = MemoryTraceManager::GetInstance();

  bool is_trace_log_csv = mtm->IsTraceLogCSV();
  bool use_mega_bytes = mtm->GetUseMegaBytes();
  int mb = use_mega_bytes ? 1024 : 1;

  struct mallinfo info = mallinfo();
  DCHECK_GE(info.arena + info.hblkhd, info.uordblks);

  // In case of Android's jemalloc |arena| is 0 and the outer pages size is
  // reported by |hblkhd|. In case of dlmalloc the total is given by
  // |arena| + |hblkhd|. For more details see link: http://goo.gl/fMR8lF.
  total_virtual_size = (info.arena + info.hblkhd) / KB / mb;
  resident_size = info.uordblks / KB / mb;
  allocated_objects_size = info.uordblks / KB / mb;

  if (is_trace_log_csv) {
    fprintf(mtm->GetTraceFile(), "%zd", allocated_objects_size);
  } else {
    if (use_mega_bytes) {
      print_fmt = "[malloc] virtual = %4zd MB, resident = %4zd MB, "
                           "allocated = %4zd MB\n";
    } else {
      print_fmt = "[malloc] virtual = %8zd KB, resident = %8zd KB, "
                           "allocated = %8zd KB\n";
    }
    fprintf(mtm->GetTraceFile(), print_fmt,
            total_virtual_size, resident_size, allocated_objects_size);
  }

  return true;
}

std::string MallocTraceProvider::GetCSVHeader() {
  return std::string("malloc");
}

}  // namespace neva
}  // namespace trace_event
}  // namespace base
