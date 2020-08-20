// Copyright 2018 LG Electronics, Inc.
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

#ifndef COMPONENTS_MEDIA_CAPTURE_UTIL_DEVICES_DISPATCHER_H_
#define COMPONENTS_MEDIA_CAPTURE_UTIL_DEVICES_DISPATCHER_H_

#include "base/callback.h"
#include "base/memory/singleton.h"
#include "content/public/browser/media_stream_request.h"

namespace content {
class WebContents;
}

namespace media_capture_util {

class DevicesDispatcher {
 public:
  static DevicesDispatcher* GetInstance();

  void ProcessMediaAccessRequest(content::WebContents* web_contents,
                                 const content::MediaStreamRequest& request,
                                 bool accepts_video,
                                 bool accepts_audio,
                                 content::MediaResponseCallback callback);

 private:
  friend struct base::DefaultSingletonTraits<DevicesDispatcher>;

  DevicesDispatcher() {}
  ~DevicesDispatcher() {}

  DISALLOW_COPY_AND_ASSIGN(DevicesDispatcher);
};

}  // namespace media_capture_util

#endif  // COMPONENTS_MEDIA_CAPTURE_UTIL_DEVICES_DISPATCHER_H_
