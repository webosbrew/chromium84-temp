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

#ifndef NEVA_APP_RUNTIME_WEBAPP_INJECTION_MANAGER_H_
#define NEVA_APP_RUNTIME_WEBAPP_INJECTION_MANAGER_H_

#include <set>
#include <string>

#include "base/macros.h"

namespace content {
class RenderFrameHost;
}  // namespace content

namespace neva_app_runtime {

class WebAppInjectionManager {
 public:
  WebAppInjectionManager();

  virtual ~WebAppInjectionManager();

  void RequestLoadInjection(content::RenderFrameHost* render_frame_host,
                            const std::string& injection_name);

  /// @brief Reload all extensions that were loaded before in case when another
  /// render process was created
  void RequestReloadInjections(content::RenderFrameHost* render_frame_host);
  void RequestUnloadInjections(content::RenderFrameHost* render_frame_host);

 private:
  std::set<std::string> requested_injections_;

  DISALLOW_COPY_AND_ASSIGN(WebAppInjectionManager);
};

}  // namespace neva_app_runtime

#endif  // NEVA_APP_RUNTIME_WEBAPP_INJECTION_MANAGER_H_
