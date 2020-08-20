// Copyright 2017 LG Electronics, Inc.
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

#ifndef UI_PLATFORM_WINDOW_NEVA_WINDOW_GROUP_CONFIGURATION_PARAM_TRAITS_MACROS_H
#define UI_PLATFORM_WINDOW_NEVA_WINDOW_GROUP_CONFIGURATION_PARAM_TRAITS_MACROS_H

#include "base/component_export.h"
#include "base/pickle.h"
#include "ipc/ipc_message_macros.h"
#include "ipc/ipc_param_traits.h"
#include "ui/platform_window/neva/window_group_configuration.h"

#undef IPC_MESSAGE_EXPORT
#define IPC_MESSAGE_EXPORT COMPONENT_EXPORT(PLATFORM_WINDOW)

#endif  // UI_PLATFORM_WINDOW_NEVA_WINDOW_GROUP_CONFIGURATION_PARAM_TRAITS_MACROS_H
