// Copyright 2020 LG Electronics, Inc.
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

#ifndef NEVA_PAL_SERVICE_PLATFORM_SYSTEM_HANDLER_
#define NEVA_PAL_SERVICE_PLATFORM_SYSTEM_HANDLER_

#include <memory>
#include <string>
#include <vector>

#include "base/callback.h"
#include "base/component_export.h"
#include "base/optional.h"
#include "base/values.h"

namespace pal {

class PlatformApplicationDelegate;
class PlatformSystemHandlerExtension;
class PlatformSystemDelegate;

class COMPONENT_EXPORT(PAL_SERVICE) PlatformSystemHandler {
 public:
  PlatformSystemHandler(
      PlatformApplicationDelegate& application_delegate,
      PlatformSystemDelegate& system_delegate);

  PlatformSystemHandler(const PlatformSystemHandler&) = delete;
  PlatformSystemHandler& operator = (const PlatformSystemHandler&) = delete;

  ~PlatformSystemHandler();

  int AddExtension(PlatformSystemHandlerExtension* extension);
  void RemoveExtension(int id);

  std::string Handle(const std::string& msg,
                     const std::vector<std::string>& args);

  std::string GetSettingsJSON() const;

 private:
  PlatformApplicationDelegate& application_delegate_;
  PlatformSystemDelegate& system_delegate_;
  std::map<int, PlatformSystemHandlerExtension*> extensions_;
  int extension_id_ = 0;
};

}  // namespace pal

#endif  // NEVA_PAL_SERVICE_PLATFORM_SYSTEM_HANDLER_
