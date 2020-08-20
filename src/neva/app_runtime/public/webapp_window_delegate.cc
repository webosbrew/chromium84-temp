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

#include "neva/app_runtime/public/webapp_window_delegate.h"

namespace neva_app_runtime {

WebAppWindowDelegate::~WebAppWindowDelegate() {}

void WebAppWindowDelegate::OnWindowClosing() {}

void WebAppWindowDelegate::CursorVisibilityChanged(bool) {}

bool WebAppWindowDelegate::HandleEvent(AppRuntimeEvent* e) {
  return false;
}

}  // namespace neva_app_runtime
