// Copyright 2019 LG Electronics, Inc.
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

#ifndef NET_NEVA_APPDRM_COMMON_H_
#define NET_NEVA_APPDRM_COMMON_H_

#include <memory>

#include "base/files/file.h"
#include "mojo/public/cpp/system/data_pipe_producer.h"
#include "net/base/net_export.h"

namespace base {
class FilePath;
}  // namespace base

namespace net {

class AppDRMFileManager;

class NET_EXPORT AppDRMFileDataSource
    : public mojo::DataPipeProducer::DataSource {
 public:
  explicit AppDRMFileDataSource(base::File file, const base::FilePath& path);
  ~AppDRMFileDataSource() override;

  // |end| should be greater than or equal to |start|. Otherwise subsequent
  // Read() fails with MOJO_RESULT_INVALID_ARGUMENT. [start, end) will be read.
  void SetRange(uint64_t start, uint64_t end);

 private:
  // DataPipeProducer::DataSource:
  uint64_t GetLength() const override;
  ReadResult Read(uint64_t offset, base::span<char> buffer) override;

  AppDRMFileManager* appdrm_ = nullptr;
  std::unique_ptr<mojo::DataPipeProducer::DataSource> file_data_source_;
};

class NET_EXPORT AppDRMFileManagerWrapper {
 public:
  AppDRMFileManagerWrapper(const std::string url_str);
  ~AppDRMFileManagerWrapper();

  bool Check();
  const std::string Decrypt();

 private:
  std::string url_;
};

}  // namespace net

#endif  // NET_NEVA_APPDRM_COMMON_H_
