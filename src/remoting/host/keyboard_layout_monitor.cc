// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "remoting/host/keyboard_layout_monitor.h"

#include "base/containers/span.h"
#include "ui/events/keycodes/dom/dom_code.h"

namespace remoting {

namespace {

const ui::DomCode kSupportedKeysArray[] = {
    ui::DomCode::ALT_LEFT,
    ui::DomCode::ALT_RIGHT,
    ui::DomCode::ARROW_DOWN,
    ui::DomCode::ARROW_RIGHT,
    ui::DomCode::ARROW_LEFT,
    ui::DomCode::ARROW_UP,
    ui::DomCode::BACKQUOTE,
    ui::DomCode::BACKSLASH,
    ui::DomCode::BACKSPACE,
    ui::DomCode::BRACKET_LEFT,
    ui::DomCode::BRACKET_RIGHT,
    ui::DomCode::CAPS_LOCK,
    ui::DomCode::COMMA,
    ui::DomCode::CONTEXT_MENU,
    ui::DomCode::CONTROL_LEFT,
    ui::DomCode::CONTROL_RIGHT,
    ui::DomCode::CONVERT,
    ui::DomCode::DEL,
    ui::DomCode::DIGIT0,
    ui::DomCode::DIGIT1,
    ui::DomCode::DIGIT2,
    ui::DomCode::DIGIT3,
    ui::DomCode::DIGIT4,
    ui::DomCode::DIGIT5,
    ui::DomCode::DIGIT6,
    ui::DomCode::DIGIT7,
    ui::DomCode::DIGIT8,
    ui::DomCode::DIGIT9,
    ui::DomCode::END,
    ui::DomCode::ENTER,
    ui::DomCode::EQUAL,
    ui::DomCode::ESCAPE,
    ui::DomCode::F1,
    ui::DomCode::F2,
    ui::DomCode::F3,
    ui::DomCode::F4,
    ui::DomCode::F5,
    ui::DomCode::F6,
    ui::DomCode::F7,
    ui::DomCode::F8,
    ui::DomCode::F9,
    ui::DomCode::F10,
    ui::DomCode::F11,
    ui::DomCode::F12,
    ui::DomCode::HOME,
    ui::DomCode::INSERT,
    ui::DomCode::INTL_BACKSLASH,
    ui::DomCode::INTL_RO,
    ui::DomCode::INTL_YEN,
    ui::DomCode::KANA_MODE,
    ui::DomCode::LANG1,
    ui::DomCode::LANG2,
    ui::DomCode::META_LEFT,
    ui::DomCode::META_RIGHT,
    ui::DomCode::MINUS,
    ui::DomCode::NON_CONVERT,
    ui::DomCode::NUM_LOCK,
    ui::DomCode::NUMPAD0,
    ui::DomCode::NUMPAD1,
    ui::DomCode::NUMPAD2,
    ui::DomCode::NUMPAD3,
    ui::DomCode::NUMPAD4,
    ui::DomCode::NUMPAD5,
    ui::DomCode::NUMPAD6,
    ui::DomCode::NUMPAD7,
    ui::DomCode::NUMPAD8,
    ui::DomCode::NUMPAD9,
    ui::DomCode::NUMPAD_ADD,
    ui::DomCode::NUMPAD_COMMA,
    ui::DomCode::NUMPAD_DECIMAL,
    ui::DomCode::NUMPAD_DIVIDE,
    ui::DomCode::NUMPAD_ENTER,
    ui::DomCode::NUMPAD_EQUAL,
    ui::DomCode::NUMPAD_MULTIPLY,
    ui::DomCode::NUMPAD_SUBTRACT,
    ui::DomCode::PAGE_DOWN,
    ui::DomCode::PAGE_UP,
    ui::DomCode::PAUSE,
    ui::DomCode::PERIOD,
    ui::DomCode::PRINT_SCREEN,
    ui::DomCode::QUOTE,
    ui::DomCode::SCROLL_LOCK,
    ui::DomCode::SEMICOLON,
    ui::DomCode::SHIFT_LEFT,
    ui::DomCode::SHIFT_RIGHT,
    ui::DomCode::SLASH,
    ui::DomCode::SPACE,
    ui::DomCode::TAB,
    ui::DomCode::US_A,
    ui::DomCode::US_B,
    ui::DomCode::US_C,
    ui::DomCode::US_D,
    ui::DomCode::US_E,
    ui::DomCode::US_F,
    ui::DomCode::US_G,
    ui::DomCode::US_H,
    ui::DomCode::US_I,
    ui::DomCode::US_J,
    ui::DomCode::US_K,
    ui::DomCode::US_L,
    ui::DomCode::US_M,
    ui::DomCode::US_N,
    ui::DomCode::US_O,
    ui::DomCode::US_P,
    ui::DomCode::US_Q,
    ui::DomCode::US_R,
    ui::DomCode::US_S,
    ui::DomCode::US_T,
    ui::DomCode::US_U,
    ui::DomCode::US_V,
    ui::DomCode::US_W,
    ui::DomCode::US_X,
    ui::DomCode::US_Y,
    ui::DomCode::US_Z,
};

}  // namespace

// static
const base::span<const ui::DomCode> KeyboardLayoutMonitor::kSupportedKeys(
    kSupportedKeysArray);

}  // namespace remoting