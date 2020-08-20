// Copyright 2017-2018 LG Electronics, Inc.
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

#ifndef NEVA_APP_RUNTIME_BROWSER_APP_RUNTIME_DEVTOOLS_MANAGER_DELEGATE_H_
#define NEVA_APP_RUNTIME_BROWSER_APP_RUNTIME_DEVTOOLS_MANAGER_DELEGATE_H_

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "content/public/browser/devtools_manager_delegate.h"

namespace content {
class BrowserContext;
}

namespace neva_app_runtime {

using namespace content;

class AppRuntimeDevToolsManagerDelegate : public DevToolsManagerDelegate {
 public:
  static void StartHttpHandler(BrowserContext* browser_context);
  static void StopHttpHandler();
  static int GetHttpHandlerPort();

  explicit AppRuntimeDevToolsManagerDelegate();
  ~AppRuntimeDevToolsManagerDelegate() override;

  // DevToolsManagerDelegate implementation.
  std::string GetDiscoveryPageHTML() override;
  bool HasBundledFrontendResources() override;

 private:
  DISALLOW_COPY_AND_ASSIGN(AppRuntimeDevToolsManagerDelegate);
};

}  // namespace neva_app_runtime

#endif  // NEVA_APP_RUNTIME_BROWSER_APP_RUNTIME_DEVTOOLS_MANAGER_DELEGATE_H_
