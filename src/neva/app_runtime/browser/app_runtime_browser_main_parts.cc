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

#include "neva/app_runtime/browser/app_runtime_browser_main_parts.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "base/run_loop.h"
#include "base/strings/string_number_conversions.h"
#include "base/task/post_task.h"
#include "components/os_crypt/key_storage_config_linux.h"
#include "components/os_crypt/os_crypt.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/result_codes.h"
#include "net/base/network_change_notifier_factory.h"
#include "neva/app_runtime/browser/app_runtime_browser_context_adapter.h"
#include "neva/app_runtime/browser/app_runtime_browser_main_extra_parts.h"
#include "neva/app_runtime/browser/app_runtime_devtools_manager_delegate.h"
#include "neva/app_runtime/browser/app_runtime_shared_memory_manager.h"
#include "neva/app_runtime/browser/net/app_runtime_network_change_notifier.h"
#include "neva/app_runtime/browser/url_request_context_factory.h"
#include "ui/views/widget/desktop_aura/neva/views_delegate_stub.h"

#if defined(ENABLE_PLUGINS)
#include "content/public/browser/plugin_service.h"
#endif

#if defined(USE_AURA)
#include "ui/aura/env.h"
#include "ui/base/ime/init/input_method_initializer.h"
#include "ui/display/screen.h"
#include "ui/views/widget/desktop_aura/desktop_screen.h"
#endif

#if defined(USE_OZONE) && defined(OZONE_PLATFORM_WAYLAND_EXTERNAL)
#include "ozone/ui/webui/ozone_webui.h"
#endif

#if defined(USE_NEVA_EXTENSIONS)
#include "components/prefs/pref_service.h"
#include "extensions/browser/process_manager_delegate.h"
#include "extensions/shell/browser/shell_browser_context_keyed_service_factories.h"
#include "extensions/shell/browser/shell_extension_system.h"
#include "extensions/shell/browser/shell_extensions_browser_client.h"
#include "extensions/shell/browser/shell_prefs.h"
#include "extensions/shell/common/shell_extensions_client.h"

namespace neva_app_runtime {
class AppRuntimeProcessManagerDelegate
    : public extensions::ProcessManagerDelegate {
 public:
  AppRuntimeProcessManagerDelegate() = default;
  ~AppRuntimeProcessManagerDelegate() override = default;
  AppRuntimeProcessManagerDelegate(const AppRuntimeProcessManagerDelegate&) =
      delete;
  AppRuntimeProcessManagerDelegate& operator=(
      const AppRuntimeProcessManagerDelegate&) = delete;

  bool AreBackgroundPagesAllowedForContext(
      content::BrowserContext* context) const override {
    return false;
  }
  bool IsExtensionBackgroundPageAllowed(
      content::BrowserContext* context,
      const extensions::Extension& extension) const override {
    return false;
  }
  bool DeferCreatingStartupBackgroundHosts(
      content::BrowserContext* context) const override {
    return false;
  }
};

class AppRuntimeExtensionsBrowserClient
    : public extensions::ShellExtensionsBrowserClient {
 public:
  AppRuntimeExtensionsBrowserClient() {
    process_manager_delegate_.reset(new AppRuntimeProcessManagerDelegate);
  }
  ~AppRuntimeExtensionsBrowserClient() override = default;
  AppRuntimeExtensionsBrowserClient(const AppRuntimeExtensionsBrowserClient&) =
      delete;
  AppRuntimeExtensionsBrowserClient& operator=(
      const AppRuntimeExtensionsBrowserClient&) = delete;
  extensions::ProcessManagerDelegate* GetProcessManagerDelegate()
      const override {
    return process_manager_delegate_.get();
  }

 private:
  std::unique_ptr<AppRuntimeProcessManagerDelegate> process_manager_delegate_;
};

}  // namespace neva_app_runtime
#endif

namespace neva_app_runtime {

class AppRuntimeNetworkChangeNotifierFactory
    : public net::NetworkChangeNotifierFactory {
 public:
  // net::NetworkChangeNotifierFactory overrides.
  std::unique_ptr<net::NetworkChangeNotifier> CreateInstance() override {
    return base::WrapUnique(AppRuntimeNetworkChangeNotifier::GetInstance());
  }
};

AppRuntimeBrowserMainParts::AppRuntimeBrowserMainParts(
    URLRequestContextFactory* url_request_context_factory)
    : BrowserMainParts(),
      url_request_context_factory_(url_request_context_factory) {}

AppRuntimeBrowserMainParts::~AppRuntimeBrowserMainParts() {}

void AppRuntimeBrowserMainParts::AddParts(
    AppRuntimeBrowserMainExtraParts* parts) {
  app_runtime_extra_parts_.push_back(parts);
}

int AppRuntimeBrowserMainParts::DevToolsPort() const {
  return AppRuntimeDevToolsManagerDelegate::GetHttpHandlerPort();
}

void AppRuntimeBrowserMainParts::EnableDevTools() {
  if (dev_tools_enabled_)
    return;

  AppRuntimeDevToolsManagerDelegate::StartHttpHandler(
    browser_context_adapter_->GetBrowserContext());
  dev_tools_enabled_ = true;
}

void AppRuntimeBrowserMainParts::DisableDevTools() {
  if (!dev_tools_enabled_)
    return;

  AppRuntimeDevToolsManagerDelegate::StopHttpHandler();
  dev_tools_enabled_ = false;
}

int AppRuntimeBrowserMainParts::PreEarlyInitialization() {
#if defined(USE_OZONE) && defined(OZONE_PLATFORM_WAYLAND_EXTERNAL)
  // Initialization of input method factory
  views::LinuxUI::SetInstance(BuildWebUI());
#elif defined(USE_OZONE) && defined(OZONE_PLATFORM_WAYLAND)
  // Do nothing. Google ozone Wayland initializes input method itself.
#else
  // Only for testing. As stub for other platforms.
  ui::InitializeInputMethodForTesting();
#endif
  return service_manager::RESULT_CODE_NORMAL_EXIT;
}

void AppRuntimeBrowserMainParts::ToolkitInitialized() {
#if defined(USE_OZONE) && defined(OZONE_PLATFORM_WAYLAND_EXTERNAL)
  views::LinuxUI::instance()->Initialize();
#endif
  if (!views::ViewsDelegate::GetInstance())
    views_delegate_ = std::make_unique<views::ViewsDelegateStub>();
}

void AppRuntimeBrowserMainParts::PreMainMessageLoopStart() {
  // Replace the default NetworkChangeNotifierFactory with app runtime
  // implementation. This must be done before BrowserMainLoop calls
  // net::NetworkChangeNotifier::Create() in PostMainMessageLoopStart().
  net::NetworkChangeNotifier::SetFactory(
      new neva_app_runtime::AppRuntimeNetworkChangeNotifierFactory());
}

void AppRuntimeBrowserMainParts::PostMainMessageLoopStart() {
  app_runtime_mem_manager_.reset(new AppRuntimeSharedMemoryManager);
}

void AppRuntimeBrowserMainParts::PreMainMessageLoopRun() {
  url_request_context_factory_->InitializeOnUIThread(nullptr);

  browser_context_adapter_.reset(
      new BrowserContextAdapter("Default", url_request_context_factory_, true));

#if defined(ENABLE_PLUGINS)
  plugin_service_filter_.reset(new AppRuntimePluginServiceFilter);
  content::PluginService::GetInstance()->SetFilter(
      plugin_service_filter_.get());
#endif

#if defined(USE_AURA)
  if (!display::Screen::GetScreen())
    display::Screen::SetScreenInstance(views::CreateDesktopScreen());

  aura::Env::GetInstance();
#endif

  CreateOSCryptConfig();

#if defined(USE_NEVA_EXTENSIONS)
  extensions_client_ = std::make_unique<extensions::ShellExtensionsClient>();
  extensions::ExtensionsClient::Set(extensions_client_.get());

  extensions_browser_client_ =
      std::make_unique<AppRuntimeExtensionsBrowserClient>();
  extensions::ExtensionsBrowserClient::Set(extensions_browser_client_.get());

  extensions::shell::EnsureBrowserContextKeyedServiceFactoriesBuilt();

  content::BrowserContext* browser_context =
      browser_context_adapter_->GetBrowserContext();

  user_pref_service_ =
      extensions::shell_prefs::CreateUserPrefService(browser_context);

  extensions_browser_client_->InitWithBrowserContext(browser_context,
                                                     user_pref_service_.get());
#endif
  for (auto* extra_part : app_runtime_extra_parts_)
    extra_part->PreMainMessageLoopRun();
}

bool AppRuntimeBrowserMainParts::MainMessageLoopRun(int* result_code) {
  base::RunLoop run_loop;
  run_loop.Run();
  return true;
}

void AppRuntimeBrowserMainParts::PostMainMessageLoopRun() {
  DisableDevTools();
#if defined(USE_NEVA_EXTENSIONS)
  user_pref_service_->CommitPendingWrite();
  user_pref_service_.reset();
#endif
}

void AppRuntimeBrowserMainParts::CreateOSCryptConfig() {
  std::unique_ptr<os_crypt::Config> config(new os_crypt::Config());
  // Forward to os_crypt the flag to use a specific password store.
  config->store = "";
  // Forward the product name
  config->product_name = "";
  // OSCrypt may target keyring, which requires calls from the main thread.
  config->main_thread_runner =
      base::CreateSingleThreadTaskRunner({content::BrowserThread::UI});
  // OSCrypt can be disabled in a special settings file.
  config->should_use_preference = false;
  config->user_data_path = base::FilePath();
  OSCrypt::SetConfig(std::move(config));
}

}  // namespace neva_app_runtime
