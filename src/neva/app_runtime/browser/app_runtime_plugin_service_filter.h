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

#ifndef NEVA_APP_RUNTIME_BROWSER_APP_RUNTIME_PLUGIN_SERVICE_FILTER_H_
#define NEVA_APP_RUNTIME_BROWSER_APP_RUNTIME_PLUGIN_SERVICE_FILTER_H_

#include "content/public/browser/plugin_service_filter.h"

namespace neva_app_runtime {

class AppRuntimePluginServiceFilter : public content::PluginServiceFilter {
 public:
  AppRuntimePluginServiceFilter() {}
  ~AppRuntimePluginServiceFilter() override {}

  bool IsPluginAvailable(int render_process_id,
                         int render_frame_id,
                         const GURL& url,
                         const url::Origin& main_frame_origin,
                         content::WebPluginInfo* plugin) override;

  bool CanLoadPlugin(int render_process_id,
                     const base::FilePath& path) override;
};

}  // namespace neva_app_runtime

#endif  // NEVA_APP_RUNTIME_BROWSER_APP_RUNTIME_PLUGIN_SERVICE_FILTER_H_
