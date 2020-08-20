// Copyright 2019 LG Electronics, Inc.
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

#ifndef COMPONENTS_CDM_RENDERER_NEVA_KEY_SYSTEMS_UTIL_H_
#define COMPONENTS_CDM_RENDERER_NEVA_KEY_SYSTEMS_UTIL_H_

#include <vector>

#include "media/base/key_system_properties.h"

namespace cdm {

void AddSupportedKeySystems(
    std::vector<std::unique_ptr<media::KeySystemProperties>>& key_systems);

}  // namespace cdm

#endif  // COMPONENTS_CDM_RENDERER_NEVA_KEY_SYSTEMS_UTIL_H_
