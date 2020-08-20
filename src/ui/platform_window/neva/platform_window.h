// Copyright 2017-2020 LG Electronics, Inc.
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

#ifndef UI_PLATFORM_WINDOW_NEVA_PLATFORM_WINDOW_H_
#define UI_PLATFORM_WINDOW_NEVA_PLATFORM_WINDOW_H_

#include <string>
#include <vector>

#include "base/strings/string16.h"
#include "neva/app_runtime/public/app_runtime_constants.h"
#include "ui/base/ime/text_input_type.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/platform_window/neva/window_group_configuration.h"
#include "ui/platform_window/neva/xinput_types.h"
#include "ui/views/widget/desktop_aura/neva/ui_constants.h"

class SkPath;

namespace ui {

class PlatformWindow;

namespace neva {

enum class PlatformWindowType {
  kUnknown,
  kTooltip,
  kPopup,
  kMenu,
  kBubble,
  kWindow,
  kWindowFrameless
};

// Additions for platform window.
class PlatformWindow {
 public:
  virtual void InitPlatformWindow(PlatformWindowType type,
                                  gfx::AcceleratedWidget parent_window) {}
  virtual void SetWindowShape(const SkPath& path) {}

  // Asks the GPU process to send data of type |mime_type|.
  virtual void RequestDragData(const std::string& mime_type) {}
  virtual void RequestSelectionData(const std::string& mime_type) {}

  // Indicates to the drag source that the data will or will not be accepted
  // at the current (x, y) coordinates. Note that there is a harmless race here.
  // The browser process could decide to accept or reject the data based on
  // old (x, y) coordinates that have since been updated by a new DragMotion
  // event in the GPU process. This doesn't matter because the browser process
  // will promptly correct the matter by calling one of these functions again
  // when it receives the DragMotion event, and these functions are only used to
  // provide user feedback: they don't affect correctness.
  virtual void DragWillBeAccepted(uint32_t serial,
                                  const std::string& mime_type) {}
  virtual void DragWillBeRejected(uint32_t serial) {}
  virtual void SetInputRegion(const std::vector<gfx::Rect>& region) {}
  virtual void SetGroupKeyMask(KeyMask key_mask) {}
  virtual void SetKeyMask(KeyMask key_mask, bool set) {}
  virtual void SetWindowProperty(const std::string& name,
                                 const std::string& value) {}
  virtual void ShowInputPanel() {}
  virtual void HideInputPanel() {}
  virtual void SetInputContentType(ui::TextInputType text_input_type,
                                   int text_input_flags) {}
  virtual void SetSurroundingText(const std::string& text,
                                  size_t cursor_position,
                                  size_t anchor_position) {}
  virtual void XInputActivate(const std::string& type) {}
  virtual void XInputDeactivate() {}
  virtual void XInputInvokeAction(uint32_t keysym,
                                  XInputKeySymbolType symbol_type,
                                  XInputEventType event_type) {}
  virtual void SetCustomCursor(neva_app_runtime::CustomCursorType type,
                               const std::string& path,
                               int hotspot_x,
                               int hotspot_y,
                               bool allowed_cursor_overriding) {}
  virtual void ResetCustomCursor() {}
  virtual void CreateGroup(const ui::WindowGroupConfiguration& config) {}
  virtual void AttachToGroup(const std::string& name,
                             const std::string& layer) {}
  virtual void FocusGroupOwner() {}
  virtual void FocusGroupLayer() {}
  virtual void DetachGroup() {}
};

}  // namespace neva

}  // namespace ui

#endif  // UI_PLATFORM_WINDOW_NEVA_PLATFORM_WINDOW_H_
