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

#ifndef MEDIA_FILTERS_NEVA_WEBOS_AUDIO_DECODER_WEBOS_H_
#define MEDIA_FILTERS_NEVA_WEBOS_AUDIO_DECODER_WEBOS_H_

#include <list>
#include <memory>

#include "base/callback.h"
#include "base/time/time.h"
#include "media/base/audio_decoder.h"
#include "media/base/demuxer_stream.h"
#include "media/base/sample_format.h"
#include "media/filters/neva/emptybuffer_audio_decoder.h"
#include "media/neva/media_platform_api.h"

struct AVCodecContext;
struct AVFrame;

namespace base {
class SingleThreadTaskRunner;
}

namespace media {

class AudioDiscardHelper;
class DecoderBuffer;

class MEDIA_EXPORT AudioDecoderWebOS : public EmptyBufferAudioDecoder {
 public:
  AudioDecoderWebOS(
      const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
      const scoped_refptr<MediaPlatformAPI>& media_platform_api);
  virtual ~AudioDecoderWebOS();

  // AudioDecoder implementation.
  std::string GetDisplayName() const override;
  void Initialize(const AudioDecoderConfig& config,
                  CdmContext* cdm_context,
                  InitCB init_cb,
                  const OutputCB& output_cb,
                  const WaitingCB& waiting_for_decryption_key_cb) override;

  // EmptyBufferAudioDecoder implementation.
  bool FeedForPlatformMediaAudioDecoder(
      const scoped_refptr<DecoderBuffer>& buffer) override;

  bool NeedsBitstreamConversion() const override;

 private:
  scoped_refptr<MediaPlatformAPI> media_platform_api_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(AudioDecoderWebOS);
};

}  // namespace media

#endif  // MEDIA_FILTERS_NEVA_WEBOS_AUDIO_DECODER_WEBOS_H_
