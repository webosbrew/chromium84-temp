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

#include "content/public/common/content_neva_switches.h"

namespace switches {

// Allow script to close windows that was opened by another scripts
const char kAllowScriptsToCloseWindows[] = "allow-scripts-to-close-windows";

// Don't suspend audio capture device when the renderer view is hidden
const char kDisableSuspendAudioCapture[] = "disable-suspend-audio-capture";

// Don't suspend video capture device when the renderer view is hidden
const char kDisableSuspendVideoCapture[] = "disable-suspend-video-capture";

// Loads browser control injection
const char kEnableBrowserControlInjection[] =
    "enable-browser-control-injection";

// Allow making a File System according to File API: Directories and System.
const char kEnableFileAPIDirectoriesAndSystem[] =
    "enable-file-api-directories-and-system";

// Loads sample injection
const char kEnableSampleInjection[] = "enable-sample-injection";

// Use platform implementation for Input Controls (File picker,
// Color chooser, e.t.c.)
const char kUseExternalInputControls[] = "use-external-input-controls";

// Specify User Agent Suffix, if required (e.g. SmartTV, SmartWatch etc.)
const char kUserAgentSuffix[] = "user-agent-suffix";
}  // namespace switches
