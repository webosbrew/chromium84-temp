// Copyright 2016-2020 LG Electronics, Inc.
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

#ifndef NEVA_APP_RUNTIME_BROWSER_APP_RUNTIME_BROWSER_MAIN_PARTS_H_
#define NEVA_APP_RUNTIME_BROWSER_APP_RUNTIME_BROWSER_MAIN_PARTS_H_

#include "content/public/browser/browser_main_parts.h"
#include "neva/app_runtime/browser/app_runtime_browser_context.h"
#include "neva/app_runtime/public/webapp_window_base.h"
#include "neva/app_runtime/public/webview_base.h"

#if defined(ENABLE_PLUGINS)
#include "neva/app_runtime/browser/app_runtime_plugin_service_filter.h"
#endif

namespace devtools_http_handler {
class DevToolsHttpHandler;
}  // namespace devtools_http_handler

#if defined(USE_NEVA_EXTENSIONS)
class PrefService;
namespace extensions {
class ExtensionsClient;
class ShellExtensionsBrowserClient;
}  // namespace extensions
#endif

namespace views {
class ViewsDelegateStub;
}

namespace neva_app_runtime {

class AppRuntimeBrowserMainExtraParts;
class AppRuntimeRemoteDebuggingServer;
class AppRuntimeSharedMemoryManager;
class BrowserContextAdapter;
class URLRequestContextFactory;

class AppRuntimeBrowserMainParts : public content::BrowserMainParts {
 public:
  AppRuntimeBrowserMainParts(
      URLRequestContextFactory* url_request_context_factory);
  ~AppRuntimeBrowserMainParts() override;

  void AddParts(AppRuntimeBrowserMainExtraParts* parts);
  int DevToolsPort() const;
  void EnableDevTools();
  void DisableDevTools();

  int PreEarlyInitialization() override;
  void ToolkitInitialized() override;
  void PreMainMessageLoopRun() override;
  void PreMainMessageLoopStart() override;
  void PostMainMessageLoopStart() override;
  bool MainMessageLoopRun(int* result_code) override;
  void PostMainMessageLoopRun() override;

  BrowserContextAdapter* GetDefaultBrowserContext() const {
    return browser_context_adapter_.get();
  }

  URLRequestContextFactory* GetURLRequestContextFactory() const {
    return url_request_context_factory_;
  }

 private:
  bool dev_tools_enabled_ = false;
  void CreateOSCryptConfig();

  std::unique_ptr<BrowserContextAdapter> browser_context_adapter_;

  URLRequestContextFactory* const url_request_context_factory_;

#if defined(USE_NEVA_EXTENSIONS)
  std::unique_ptr<PrefService> user_pref_service_;
  std::unique_ptr<extensions::ExtensionsClient> extensions_client_;
  std::unique_ptr<extensions::ShellExtensionsBrowserClient>
      extensions_browser_client_;
#endif
#if defined(ENABLE_PLUGINS)
  std::unique_ptr<AppRuntimePluginServiceFilter> plugin_service_filter_;
#endif
  std::vector<AppRuntimeBrowserMainExtraParts*> app_runtime_extra_parts_;
  std::unique_ptr<AppRuntimeSharedMemoryManager> app_runtime_mem_manager_;
  std::unique_ptr<views::ViewsDelegateStub> views_delegate_;
};

}  // namespace neva_app_runtime

#endif  // NEVA_APP_RUNTIME_BROWSER_APP_RUNTIME_BROWSER_MAIN_PARTS_H_
