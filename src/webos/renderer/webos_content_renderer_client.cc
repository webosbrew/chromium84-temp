// Copyright 2018-2019 LG Electronics, Inc.
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

#include "webos/renderer/webos_content_renderer_client.h"

#include "base/i18n/rtl.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "components/url_formatter/url_formatter.h"
#include "content/public/renderer/render_frame.h"
#include "content/renderer/render_frame_impl.h"
#include "net/base/net_errors.h"
#include "neva/app_runtime/grit/app_runtime_network_error_strings.h"
#include "neva/app_runtime/renderer/app_runtime_localized_error.h"
#include "third_party/blink/public/platform/web_url_error.h"
#include "third_party/blink/public/web/web_frame.h"
#include "third_party/blink/public/web/web_frame_widget.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_view.h"
#include "third_party/blink/public/web/web_widget.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/webui/jstemplate_builder.h"
#include "webos/grit/webos_network_error_resources.h"
#include "webos/grit/webos_network_error_strings.h"
#include "webos/renderer/webos_network_error_helper.h"
#include "webos/renderer/webos_network_error_template_builder.h"

namespace {

void GetWebOSLocalizedErrorStrings(int error_code, const GURL& failed_url,
    base::DictionaryValue& error_strings) {
  error_strings.SetString("textdirection",
      base::i18n::IsRTL() ? "rtl" : "ltr");

  base::string16 failed_url_string(url_formatter::FormatUrl(failed_url,
      url_formatter::kFormatUrlOmitNothing, net::UnescapeRule::NORMAL, nullptr,
      nullptr, nullptr));

  error_strings.SetString("url_to_reload", failed_url_string);

  failed_url_string = base::UTF8ToUTF16(failed_url.host());

  switch (error_code) {
    case net::ERR_CERT_DATE_INVALID: {
      // We need to change some strings in case of ATSC30LegacyBoxSupport
      error_strings.SetString(
          "error_title_atsc_legacy_box",
          l10n_util::GetStringUTF16(IDS_NET_ERROR_UNABLE_TO_LOAD));
      error_strings.SetString(
          "error_details_atsc_legacy_box",
          l10n_util::GetStringUTF16(IDS_NET_ERROR_CERTIFICATE_HAS_EXPIRED_UHD));
      error_strings.SetString(
          "error_guide_atsc_legacy_box",
          l10n_util::GetStringUTF16(IDS_NET_ERROR_CHECK_TIME_SETTINGS));
      error_strings.SetString(
          "error_info_atsc_legacy_box",
          l10n_util::GetStringUTF16(IDS_NET_ERROR_CURRENT_TIME_SETTINGS));
      break;
    }
    case net::ERR_INTERNET_DISCONNECTED:
    case net::ERR_NAME_NOT_RESOLVED:
    case net::ERR_NAME_RESOLUTION_FAILED:
      error_strings.SetString("error_info", failed_url_string);
      break;
  }

  // We want localized Buttons on page
  // Error page uses Buttons
  // 1. Exit App
  // 2. Nwtwork Settings
  // 3. Retry
  // 4. Settings
  error_strings.SetString(
      "exit_app_button_text",
      l10n_util::GetStringUTF16(IDS_NET_ERROR_EXIT_APP_BUTTON_TEXT));
  error_strings.SetString(
      "network_settings_button_text",
      l10n_util::GetStringUTF16(IDS_NET_ERROR_NETWORK_SETTINGS_BUTTON_TEXT));
  error_strings.SetString(
      "retry_button_text",
      l10n_util::GetStringUTF16(IDS_NET_ERROR_RETRY_BUTTON_TEXT));
  error_strings.SetString(
      "settings_button_text",
      l10n_util::GetStringUTF16(IDS_NET_ERROR_SETTINGS_UPPER_CASE));
}

}  // namespace

namespace webos {

void WebOSContentRendererClient::RenderFrameCreated(
    content::RenderFrame* render_frame) {
  AppRuntimeContentRendererClient::RenderFrameCreated(render_frame);
  new WebOSNetworkErrorHelper(render_frame);
}

void WebOSContentRendererClient::PrepareErrorPage(
    content::RenderFrame* render_frame,
    const blink::WebURLError& error,
    const std::string& http_method,
    std::string* error_html) {
  AppRuntimeContentRendererClient::PrepareErrorPage(render_frame, error,
                                                    http_method, error_html);
  if (error_html) {
    error_html->clear();

    // Resource will change to net error specific page
    int resource_id = IDR_WEBOS_NETWORK_ERROR_PAGE;
    const std::string template_html =
        ui::ResourceBundle::GetSharedInstance().LoadDataResourceString(
            resource_id);
    if (template_html.empty()) {
      LOG(ERROR) << "unable to load template.";
    } else {
      base::DictionaryValue error_strings;
      neva_app_runtime::AppRuntimeLocalizedError::GetStrings(error.reason(),
          error_strings);

      GetWebOSLocalizedErrorStrings(error.reason(), error.url(),
          error_strings);

      content::RenderFrameImpl* impl =
          static_cast<content::RenderFrameImpl*>(render_frame);

      if (impl->GetRendererPreferences().is_enact_browser)
        error_strings.SetString(
            "exit_app_button_text",
            l10n_util::GetStringUTF16(IDS_NET_ERROR_CLOSE_TAB_BUTTON_TEXT));
      // "t" is the id of the template's root node.
      *error_html = webui::GetTemplatesHtml(template_html,
          &error_strings, "t");

      // viewport width and height
      int viewport_width =
          render_frame->GetWebFrame()->View()->MainFrameWidget()->Size().width;
      int viewport_height =
          render_frame->GetWebFrame()->View()->MainFrameWidget()->Size().height;

      // Add webos specific functionality
      *error_html = webos::GetTemplatesHtml(*error_html, &error_strings,
          error.reason(), viewport_width, viewport_height);
    }
  }
}

}  // namespace webos
