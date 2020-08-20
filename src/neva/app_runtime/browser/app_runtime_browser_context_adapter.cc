// Copyright 2017-2019 LG Electronics, Inc.
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

#include "browser/app_runtime_browser_context_adapter.h"

#include "neva/app_runtime/app/app_runtime_main_delegate.h"
#include "neva/app_runtime/browser/app_runtime_browser_context.h"

namespace neva_app_runtime {

BrowserContextAdapter::BrowserContextAdapter(
    const std::string& storage_name,
    URLRequestContextFactory* url_request_context_factory,
    bool is_default)
    : storage_name_(storage_name),
      is_default_(is_default),
      browser_context_(
          new AppRuntimeBrowserContext(this, url_request_context_factory)),
      url_request_context_factory_(url_request_context_factory) {}

BrowserContextAdapter::~BrowserContextAdapter() {
  delete browser_context_;
}

BrowserContextAdapter* BrowserContextAdapter::GetDefaultContext() {
  return GetAppRuntimeContentBrowserClient()
      ->GetMainParts()
      ->GetDefaultBrowserContext();
}

AppRuntimeBrowserContext* BrowserContextAdapter::GetBrowserContext() const {
  return browser_context_;
}

std::string BrowserContextAdapter::GetStorageName() const {
  return storage_name_;
}

bool BrowserContextAdapter::IsDefault() const {
  return is_default_;
}

void BrowserContextAdapter::FlushCookieStore() {
  browser_context_->FlushCookieStore();
}

URLRequestContextFactory* BrowserContextAdapter::GetUrlRequestContextFactory() {
  return url_request_context_factory_;
}

}  // namespace neva_app_runtime
