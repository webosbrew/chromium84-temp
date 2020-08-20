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

#ifndef UI_OZONE_PLATFORM_WAYLAND_HOST_INPUT_MANAGER_WRAPPER_H_
#define UI_OZONE_PLATFORM_WAYLAND_HOST_INPUT_MANAGER_WRAPPER_H_

#include <memory>

#include "ui/ozone/platform/wayland/common/wayland_object.h"

namespace ui {

class ExtendedSeatWrapper;

// Wrapper for the input manager interface capable of retrieving extended seat
// object that provides even more information than regular seat does.
class InputManagerWrapper {
 public:
  virtual ~InputManagerWrapper() = default;

  // Returns extended seat wrapped object associated with the regular |seat|.
  virtual std::unique_ptr<ExtendedSeatWrapper> GetExtendedSeat(
      wl_seat* seat) = 0;

  // Sets the cursor visibility.
  virtual void SetCursorVisibility(bool is_visible) = 0;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_WAYLAND_HOST_INPUT_MANAGER_WRAPPER_H_
