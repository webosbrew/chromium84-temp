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

#include "third_party/blink/renderer/platform/neva/partition_alloc_memory_trace_provider.h"

#include <unordered_map>

#include "base/allocator/partition_allocator/neva/partition_trace.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/neva/memory_trace/memory_trace_manager.h"
#include "third_party/blink/renderer/platform/wtf/allocator/partitions.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/std_lib_extras.h"

namespace blink {
namespace neva {

static constexpr int KB = 1024;

// This class is used to invert the dependency of PartitionAlloc on the
// PartitionAllocMemoryTraceProvider. This implements an interface that will
// be called with memory statistics for each bucket in the allocator.
class PartitionStatsTracerImpl final : public base::neva::PartitionStatsTracer {
  DISALLOW_NEW();
  WTF_MAKE_NONCOPYABLE(PartitionStatsTracerImpl);
 public:
  PartitionStatsTracerImpl() : total_active_bytes_(0) {}

  // PartitionStatsTracer implementation.
  void PartitionTraceTotals(const char* partition_name,
                            const base::PartitionMemoryStats*) override;

  size_t TotalActiveBytes() const { return total_active_bytes_; }

 private:
  size_t total_active_bytes_;
};

void PartitionStatsTracerImpl::PartitionTraceTotals(
    const char* partition_name,
    const base::PartitionMemoryStats* memory_stats) {
  const char* print_fmt;
  base::trace_event::neva::MemoryTraceManager* mtm =
      base::trace_event::neva::MemoryTraceManager::GetInstance();
  FILE* trace_fp = mtm->GetTraceFile();
  bool is_trace_log_csv = mtm->IsTraceLogCSV();
  bool use_mega_bytes = mtm->GetUseMegaBytes();
  int mb = use_mega_bytes ? 1024 : 1;

  total_active_bytes_ += memory_stats->total_active_bytes;

  size_t total_resident_bytes = memory_stats->total_resident_bytes / KB / mb;
  size_t total_active_bytes = memory_stats->total_active_bytes / KB / mb;
  size_t total_mmapped_bytes = memory_stats->total_mmapped_bytes / KB / mb;
  size_t total_committed_bytes = memory_stats->total_committed_bytes / KB / mb;
  size_t total_decommittable_bytes = memory_stats->total_decommittable_bytes / KB / mb;
  size_t total_discardable_bytes = memory_stats->total_discardable_bytes / KB / mb;

  if (is_trace_log_csv) {
    fprintf(trace_fp, "%zd, ", total_active_bytes);
  } else {
    fprintf(trace_fp, "[PA/%12s] ", partition_name);
    if (use_mega_bytes) {
      print_fmt = "resident = %3zd, alloc = %3zd, mmap = %3zd, "
                  "commit = %3zd, decommitable = %2zd, discardable = %3zd\n";
    } else {
      print_fmt = "resident = %6zd, alloc = %6zd, mmap = %6zd, "
                  "commit = %6zd, decommitable = %4zd, discardable = %6zd\n";
    }
    fprintf(trace_fp, print_fmt,
            total_resident_bytes, total_active_bytes,
            total_mmapped_bytes, total_committed_bytes,
            total_decommittable_bytes, total_discardable_bytes);
  }
}

PartitionAllocMemoryTraceProvider* PartitionAllocMemoryTraceProvider::Instance() {
  DEFINE_STATIC_LOCAL(PartitionAllocMemoryTraceProvider, instance, ());
  return &instance;
}

bool PartitionAllocMemoryTraceProvider::OnMemoryTrace() {
  PartitionStatsTracerImpl partition_stats_tracer;
  const char* print_fmt;
  base::trace_event::neva::MemoryTraceManager* mtm =
      base::trace_event::neva::MemoryTraceManager::GetInstance();
  FILE* trace_fp = mtm->GetTraceFile();
  bool is_trace_log_csv = mtm->IsTraceLogCSV();
  bool use_mega_bytes = mtm->GetUseMegaBytes();
  int mb = use_mega_bytes ? 1024 : 1;

  // This method calls memory_stats.partitionsTraceBucketStats with memory statistics.
  WTF::Partitions::TraceMemoryStats(&partition_stats_tracer);

  size_t total_active_bytes = partition_stats_tracer.TotalActiveBytes() / KB / mb;
  if (is_trace_log_csv) {
    print_fmt = "%zd";
  } else {
    print_fmt = "[Partition Alloc] total active (alloced) = %8zd\n";
  }
  fprintf(trace_fp, print_fmt, total_active_bytes);

  return true;
}

std::string PartitionAllocMemoryTraceProvider::GetCSVHeader() {
  std::string header;
  std::vector<std::string> v = { std::string("PA:fast_malloc"),
                                 std::string("PA:array_buffer"),
                                 std::string("PA:buffer"),
                                 std::string("PA:layout") };
  for (auto& s : v) {
    header += s + ", ";
  }
  header += "PA:total_active";
  return header;
}

}  // namespace neva
}  // namespace blink
