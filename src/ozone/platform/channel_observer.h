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

#ifndef OZONE_PLATFORM_CHANNEL_OBSERVER_H_
#define OZONE_PLATFORM_CHANNEL_OBSERVER_H_

#include "base/observer_list_types.h"

namespace ui {

// Observes the channel state.
class ChannelObserver : public base::CheckedObserver {
 public:
  ~ChannelObserver() override = default;

  virtual void OnGpuProcessLaunched() = 0;
  virtual void OnChannelDestroyed() = 0;
};

}  // namespace ui

#endif  // OZONE_PLATFORM_CHANNEL_OBSERVER_H_
