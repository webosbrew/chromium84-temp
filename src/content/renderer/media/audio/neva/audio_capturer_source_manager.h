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

#ifndef CONTENT_RENDERER_MEDIA_AUDIO_AUDIO_NEVA_CAPTURER_SOURCE_MANAGER_H
#define CONTENT_RENDERER_MEDIA_AUDIO_AUDIO_NEVA_CAPTURER_SOURCE_MANAGER_H

#include <unordered_set>

#include "base/macros.h"
#include "content/common/content_export.h"

namespace media {
class AudioCapturerSource;
}  // namespace media

namespace content {

class CONTENT_EXPORT AudioCapturerSourceManager {
 public:
  AudioCapturerSourceManager();
  virtual ~AudioCapturerSourceManager();

  void AddSource(media::AudioCapturerSource* source);
  void RemoveSource(media::AudioCapturerSource* source);

  void SuspendDevices(bool suspend);

 private:
  std::unordered_set<media::AudioCapturerSource*> sources_;

  DISALLOW_COPY_AND_ASSIGN(AudioCapturerSourceManager);
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_AUDIO_AUDIO_NEVA_CAPTURER_SOURCE_MANAGER_H
