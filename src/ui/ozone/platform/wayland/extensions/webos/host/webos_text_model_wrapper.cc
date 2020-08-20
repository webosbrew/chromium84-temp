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

#include "ui/ozone/platform/wayland/extensions/webos/host/webos_text_model_wrapper.h"

#include <wayland-text-client-protocol.h>

#include "ui/base/ime/text_input_flags.h"
#include "ui/ozone/platform/wayland/extensions/webos/common/wayland_webos_util.h"
#include "ui/ozone/platform/wayland/host/wayland_input_method_context.h"
#include "ui/ozone/platform/wayland/host/wayland_seat.h"
#include "ui/ozone/platform/wayland/host/wayland_window.h"

namespace ui {

namespace {

constexpr std::uint32_t kSerial = 0;

// The enter key appearance on the (virtual keyboard) input panel.
enum class EnterKeyType : std::uint32_t {
  kDefault = 0,   // the default Enter key
  kReturn = 1,    // show a Return button that inserts a new line
  kDone = 2,      // show a Done button
  kGo = 3,        // show a Go button
  kSend = 4,      // show a Send button
  kSearch = 5,    // show a Search button
  kNext = 6,      // show a Next button
  kPrevious = 7,  // show a Previous button
};

// The input panel content hint (a bitmask capable of modifying its behavior).
enum class ContentHint : std::uint32_t {
  kNone = 0x0,            // no special behavior
  kDefault = 0x7,         // auto completion, correction and capitalization
  kPassword = 0xc0,       // hidden and sensitive text
  kAutoCompletion = 0x1,  // suggest word completions
  kAutoCorrection = 0x2,  // suggest word corrections
  kAutoCapitalization =
      0x4,            // switch to uppercase letters at the start of a sentence
  kLowercase = 0x8,   // prefer lowercase letters
  kUppercase = 0x10,  // prefer uppercase letters
  kTitlecase =
      0x20,  // prefer casing for titles and headings (language dependent)
  kHiddenText = 0x40,     // characters should be hidden
  kSensitiveData = 0x80,  // typed text should not be stored
  kLatin = 0x100,         // just Latin characters should be entered
  kMultiline = 0x200      // the text input is multiline
};

// The input panel primary content purpose.
enum class ContentPurpose : std::uint32_t {
  kNormal = 0,     // default input, allowing all characters
  kAlpha = 1,      // allow only alphabetic characters
  kDigits = 2,     // allow only digits
  kNumber = 3,     // input a number (including decimal separator and sign)
  kPhone = 4,      // input a phone number
  kUrl = 5,        // input an URL
  kEmail = 6,      // input an email address
  kName = 7,       // input a person name
  kPassword = 8,   // input a password (combine with |kPassword| or
                   // |kSensitiveData| hint)
  kDate = 9,       // input a date
  kTime = 10,      // input a time
  kDatetime = 11,  // input a date and time
  kTerminal = 12,  // input for a terminal
};

std::uint32_t ContentHintFromTextInputTraits(TextInputType type, int flags) {
  std::uint32_t result =
      (ContentHint::kAutoCompletion | ContentHint::kAutoCapitalization);
  if (type == TEXT_INPUT_TYPE_PASSWORD)
    result |= ContentHint::kPassword;

  // TODO(sergey.kipet@lge.com): TEXT_INPUT_FLAG_SPELLCHECK_ON remains
  // (to be also mapped as soon as LSM would support spellchecking).
  if (flags & TEXT_INPUT_FLAG_SENSITIVE_ON)
    result |= ContentHint::kSensitiveData;
  if (flags & TEXT_INPUT_FLAG_AUTOCOMPLETE_ON)
    result |= ContentHint::kAutoCompletion;
  if (flags & TEXT_INPUT_FLAG_AUTOCORRECT_ON)
    result |= ContentHint::kAutoCorrection;

  return result;
}

std::uint32_t ContentPurposeFromTextInputType(TextInputType type) {
  ContentPurpose result = ContentPurpose::kNormal;
  switch (type) {
    case TEXT_INPUT_TYPE_PASSWORD:
      result = ContentPurpose::kPassword;
      break;
    case TEXT_INPUT_TYPE_EMAIL:
      result = ContentPurpose::kEmail;
      break;
    case TEXT_INPUT_TYPE_NUMBER:
      result = ContentPurpose::kNumber;
      break;
    case TEXT_INPUT_TYPE_TELEPHONE:
      result = ContentPurpose::kPhone;
      break;
    case TEXT_INPUT_TYPE_URL:
      result = ContentPurpose::kUrl;
      break;
    case TEXT_INPUT_TYPE_DATE:
      result = ContentPurpose::kDate;
      break;
    case TEXT_INPUT_TYPE_DATE_TIME:
    case TEXT_INPUT_TYPE_DATE_TIME_LOCAL:
      result = ContentPurpose::kDatetime;
      break;
    case TEXT_INPUT_TYPE_TIME:
      result = ContentPurpose::kTime;
      break;
    default:
      break;
  }
  return static_cast<std::uint32_t>(result);
}

}  // namespace

WebosTextModelWrapper::WebosTextModelWrapper(text_model* text_model,
                                             WaylandSeat* seat,
                                             WaylandWindow* window)
    : input_method_context_(window->GetInputMethodContext()),
      seat_(seat),
      window_(window) {
  text_model_.reset(text_model);

  static const text_model_listener text_model_listener = {
      WebosTextModelWrapper::CommitString,
      WebosTextModelWrapper::PreeditString,
      WebosTextModelWrapper::DeleteSurroundingText,
      WebosTextModelWrapper::CursorPosition,
      WebosTextModelWrapper::PreeditStyling,
      WebosTextModelWrapper::PreeditCursor,
      WebosTextModelWrapper::ModifiersMap,
      WebosTextModelWrapper::Keysym,
      WebosTextModelWrapper::Enter,
      WebosTextModelWrapper::Leave,
      WebosTextModelWrapper::InputPanelState,
      WebosTextModelWrapper::InputPanelRect};

  text_model_add_listener(text_model_.get(), &text_model_listener, this);
}

void WebosTextModelWrapper::SetSurroundingText(const std::string& text,
                                               std::uint32_t cursor,
                                               std::uint32_t anchor) {
  text_model_set_surrounding_text(text_model_.get(), text.c_str(), cursor,
                                  anchor);
}

void WebosTextModelWrapper::Activate() {
  text_model_activate(text_model_.get(), kSerial, seat_->seat(),
                      window_->surface());
}

void WebosTextModelWrapper::Deactivate() {
  text_model_deactivate(text_model_.get(), seat_->seat());
}

void WebosTextModelWrapper::Reset() {
  text_model_reset(text_model_.get(), kSerial);
}

void WebosTextModelWrapper::SetCursorRectangle(std::int32_t x,
                                               std::int32_t y,
                                               std::int32_t width,
                                               std::int32_t height) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::SetContentType(TextInputType type, int flags) {
  text_model_set_content_type(text_model_.get(),
                              ContentHintFromTextInputTraits(type, flags),
                              ContentPurposeFromTextInputType(type));
}

void WebosTextModelWrapper::InvokeAction(std::uint32_t button,
                                         std::uint32_t index) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::Commit() {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::ShowInputPanel() {
  text_model_show_input_panel(text_model_.get());
}

void WebosTextModelWrapper::HideInputPanel() {
  text_model_hide_input_panel(text_model_.get());
}

void WebosTextModelWrapper::SetMaxTextLength(std::uint32_t length) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::SetPlatformData(const std::string& text) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::SetEnterKeyType(std::uint32_t enter_key_type) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::SetInputPanelRect(std::int32_t x,
                                              std::int32_t y,
                                              std::uint32_t width,
                                              std::uint32_t height) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::ResetInputPanelRect() {
  NOTIMPLEMENTED_LOG_ONCE();
}

// static
void WebosTextModelWrapper::CommitString(void* data,
                                         text_model* text_model,
                                         std::uint32_t serial,
                                         const char* text) {
  WebosTextModelWrapper* text_model_wrapper =
      static_cast<WebosTextModelWrapper*>(data);
  DCHECK(text_model_wrapper);

  if (text_model_wrapper->input_method_context_)
    text_model_wrapper->input_method_context_->OnCommitString(
        std::string(text));
}

void WebosTextModelWrapper::PreeditString(void* data,
                                          text_model* text_model,
                                          std::uint32_t serial,
                                          const char* text,
                                          const char* commit) {
  WebosTextModelWrapper* text_model_wrapper =
      static_cast<WebosTextModelWrapper*>(data);
  DCHECK(text_model_wrapper);

  if (text_model_wrapper->input_method_context_)
    text_model_wrapper->input_method_context_->OnPreeditString(
        std::string(text), text_model_wrapper->preedit_cursor_);
}

void WebosTextModelWrapper::DeleteSurroundingText(void* data,
                                                  text_model* text_model,
                                                  std::uint32_t serial,
                                                  std::int32_t index,
                                                  std::uint32_t length) {
  WebosTextModelWrapper* text_model_wrapper =
      static_cast<WebosTextModelWrapper*>(data);
  DCHECK(text_model_wrapper);

  if (text_model_wrapper->input_method_context_)
    text_model_wrapper->input_method_context_->OnDeleteSurroundingText(index,
                                                                       length);
}

void WebosTextModelWrapper::CursorPosition(void* data,
                                           text_model* text_model,
                                           std::uint32_t serial,
                                           std::int32_t index,
                                           std::int32_t anchor) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::PreeditStyling(void* data,
                                           text_model* text_model,
                                           std::uint32_t serial,
                                           std::uint32_t index,
                                           std::uint32_t length,
                                           std::uint32_t style) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::PreeditCursor(void* data,
                                          text_model* text_model,
                                          std::uint32_t serial,
                                          std::int32_t index) {
  WebosTextModelWrapper* text_model_wrapper =
      static_cast<WebosTextModelWrapper*>(data);
  DCHECK(text_model_wrapper);

  text_model_wrapper->preedit_cursor_ = index;
}

void WebosTextModelWrapper::ModifiersMap(void* data,
                                         text_model* text_model,
                                         wl_array* map) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::Keysym(void* data,
                                   text_model* text_model,
                                   std::uint32_t serial,
                                   std::uint32_t time,
                                   std::uint32_t sym,
                                   std::uint32_t state,
                                   std::uint32_t modifiers) {
  WebosTextModelWrapper* text_model_wrapper =
      static_cast<WebosTextModelWrapper*>(data);
  DCHECK(text_model_wrapper);

  if (text_model_wrapper->input_method_context_)
    text_model_wrapper->input_method_context_->OnKeysym(sym, state, modifiers);
}

void WebosTextModelWrapper::Enter(void* data,
                                  text_model* text_model,
                                  wl_surface* surface) {
  WebosTextModelWrapper* text_model_wrapper =
      static_cast<WebosTextModelWrapper*>(data);
  DCHECK(text_model_wrapper);

  text_model_wrapper->is_activated_ = true;
}

void WebosTextModelWrapper::Leave(void* data, text_model* text_model) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::InputPanelState(void* data,
                                            text_model* text_model,
                                            std::uint32_t state) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void WebosTextModelWrapper::InputPanelRect(void* data,
                                           text_model* text_model,
                                           std::int32_t x,
                                           std::int32_t y,
                                           std::uint32_t width,
                                           std::uint32_t height) {
  NOTIMPLEMENTED_LOG_ONCE();
}

}  // namespace ui
