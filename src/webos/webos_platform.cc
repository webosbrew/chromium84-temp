// Copyright 2016-2018 LG Electronics, Inc.
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

#include "webos_platform.h"

#include "neva/app_runtime/browser/net/app_runtime_network_change_notifier.h"
#include "ozone/wayland/display.h"
#include "ozone/wayland/shell/webos_shell_surface.h"
#include "ozone/wayland/window.h"
#include "ui/views/widget/desktop_aura/neva/ui_constants.h"
#include "webos/common/webos_locales_mapping.h"
#include "webos/common/webos_types_conversion_utils.h"
#include "webos/public/runtime.h"

namespace webos {

WebOSPlatform* WebOSPlatform::webos_platform_ = new WebOSPlatform;

WebOSPlatform* WebOSPlatform::GetInstance() {
  return webos_platform_;
}

WebOSPlatform::WebOSPlatform() : input_pointer_(NULL) {
  Runtime::GetInstance()->InitializePlatformDelegate(this);
}

WebOSPlatform::~WebOSPlatform() {
}

void WebOSPlatform::OnCursorVisibilityChanged(bool visible) {
  if (input_pointer_)
    input_pointer_->OnCursorVisibilityChanged(visible);
}

void WebOSPlatform::OnNetworkStateChanged(bool is_connected) {
  neva_app_runtime::AppRuntimeNetworkChangeNotifier* network_change_notifier =
      neva_app_runtime::AppRuntimeNetworkChangeNotifier::GetInstance();
  if (network_change_notifier)
    network_change_notifier->OnNetworkStateChanged(is_connected);
}

void WebOSPlatform::OnLocaleInfoChanged(std::string language) {
  std::string locale = webos::MapWebOsToChromeLocales(language);
  NOTIMPLEMENTED() << " webos language: " << language
                   << ", chrome locale: " << locale;
}

void WebOSPlatform::SetInputPointer(InputPointer* input_pointer) {
  input_pointer_ = input_pointer;
}

InputPointer* WebOSPlatform::GetInputPointer() {
  return input_pointer_;
}

}  // namespace webos
