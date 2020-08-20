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

#include "neva/app_runtime/webapp_injection_manager.h"

#include "content/public/browser/render_frame_host.h"
#include "neva/app_runtime/public/mojom/app_runtime_webview.mojom.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"

#if defined(OS_WEBOS)
#include "neva/injection/netcast/netcast_injection.h"
#if defined(USE_GAV)
#include "neva/injection/webosgavplugin/webosgavplugin_injection.h"
#endif  // defined(USE_GAV)
#include "neva/injection/webosservicebridge/webosservicebridge_injection.h"
#include "neva/injection/webossystem/webossystem_injection.h"
#endif  // defined(OS_WEBOS)

#if defined(ENABLE_BROWSER_CONTROL_WEBAPI)
#include "neva/injection/browser_control/browser_control_injection.h"
#endif

#if defined(ENABLE_MEMORYMANAGER_WEBAPI)
#include "neva/injection/memorymanager/memorymanager_injection.h"
#endif

#if defined(ENABLE_SAMPLE_WEBAPI)
#include "neva/injection/sample/sample_injection.h"
#endif

namespace neva_app_runtime {
namespace {

std::set<std::string> allowed_injections = {
#if defined(OS_WEBOS)
    std::string(injections::NetCastWebAPI::kInjectionName),
    std::string(injections::WebOSServiceBridgeWebAPI::kInjectionName),
    std::string(injections::WebOSServiceBridgeWebAPI::kObsoleteName),
    std::string(injections::WebOSSystemWebAPI::kInjectionName),
    std::string(injections::WebOSSystemWebAPI::kObsoleteName),
#if defined(USE_GAV)
    std::string(injections::WebOSGAVWebAPI::kInjectionName),
#endif  // defined(USE_GAV)
#endif  // defined(OS_WEBOS)
#if defined(ENABLE_SAMPLE_WEBAPI)
    std::string(injections::SampleWebAPI::kInjectionName),
#endif
#if defined(ENABLE_BROWSER_CONTROL_WEBAPI)
    std::string(injections::BrowserControlWebAPI::kInjectionName),
#endif
#if defined(ENABLE_MEMORYMANAGER_WEBAPI)
    std::string(injections::MemoryManagerWebAPI::kInjectionName),
#endif
};

}  // namespace

WebAppInjectionManager::WebAppInjectionManager() {}

WebAppInjectionManager::~WebAppInjectionManager() {}

void WebAppInjectionManager::RequestLoadInjection(
    content::RenderFrameHost* render_frame_host,
    const std::string& injection_name) {
  if (!render_frame_host)
    return;

  if (allowed_injections.count(injection_name) == 0)
    return;

  if (requested_injections_.find(injection_name) != requested_injections_.end())
    return;

  mojo::AssociatedRemote<mojom::AppRuntimeWebViewClient> client;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(&client);
  requested_injections_.insert(injection_name);
  client->AddInjectionToLoad(injection_name);
}

void WebAppInjectionManager::RequestReloadInjections(
    content::RenderFrameHost* render_frame_host) {
  if (!render_frame_host)
    return;

  mojo::AssociatedRemote<mojom::AppRuntimeWebViewClient> client;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(&client);
  for (const auto& injection_name : requested_injections_)
    client->AddInjectionToLoad(injection_name);
}

void WebAppInjectionManager::RequestUnloadInjections(
    content::RenderFrameHost* render_frame_host) {
  if (!render_frame_host)
    return;

  mojo::AssociatedRemote<mojom::AppRuntimeWebViewClient> client;
  render_frame_host->GetRemoteAssociatedInterfaces()->GetInterface(&client);
  requested_injections_.clear();
  client->UnloadInjections();
}

}  // namespace neva_app_runtime
