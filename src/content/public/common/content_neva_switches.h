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

#ifndef CONTENT_PUBLIC_COMMON_CONTENT_NEVA_SWITCHES_H_
#define CONTENT_PUBLIC_COMMON_CONTENT_NEVA_SWITCHES_H_

#include "content/common/content_export.h"

namespace switches {

CONTENT_EXPORT extern const char kAllowScriptsToCloseWindows[];
extern const char kDisableSuspendAudioCapture[];
extern const char kDisableSuspendVideoCapture[];
CONTENT_EXPORT extern const char kEnableBrowserControlInjection[];
CONTENT_EXPORT extern const char kEnableFileAPIDirectoriesAndSystem[];
CONTENT_EXPORT extern const char kEnableSampleInjection[];
extern const char kUseExternalInputControls[];
extern const char kUserAgentSuffix[];

}  // namespace switches

#endif  // CONTENT_PUBLIC_COMMON_CONTENT_NEVA_SWITCHES_H_
