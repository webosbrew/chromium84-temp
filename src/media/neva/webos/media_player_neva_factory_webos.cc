// Copyright 2017-2020 LG Electronics, Inc.
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

#include "media/neva/media_player_neva_factory.h"

#include "base/command_line.h"
#include "media/base/media_switches_neva.h"
#include "media/neva/webos/media_player_ums.h"
#include "net/base/mime_util.h"

#if defined(USE_GST_MEDIA) || defined(USE_SIGNAGE_MEDIA)
#include "media/neva/webos/media_player_camera.h"
#endif

#if defined(USE_TV_MEDIA) || defined(USE_SIGNAGE_MEDIA)
#include "media/neva/webos/media_player_extinput.h"
#include "media/neva/webos/media_player_photo.h"
#endif

namespace media {

// also see media/base/mime_util_internal.cc(webos_codecs).
const char kPrefixOfWebOSBroadcastCableMimeType[] =
    "service/webos-broadcast-cable";
const char kPrefixOfWebOSBroadcastMimeType[] = "service/webos-broadcast";
const char kPrefixOfWebOSCameraMimeType[] = "service/webos-camera";
const char kPrefixOfWebOSDvrMimeType[] = "service/webos-dvr";
const char kPrefixOfWebOSExternalMimeType[] = "service/webos-external";
const char kPrefixOfWebOSMediaMimeType[] = "service/webos-*";
const char kPrefixOfWebOSPhotoCameraMimeType[] = "service/webos-photo-camera";
const char kPrefixOfWebOSPhotoMimeType[] = "service/webos-photo";

media::MediaPlayerType MediaPlayerNevaFactory::GetMediaPlayerType(
    const std::string& mime_type) {
  if (net::MatchesMimeType(kPrefixOfWebOSCameraMimeType, mime_type)) {
    return media::MediaPlayerType::kMediaPlayerTypeCamera;
  }
  if (net::MatchesMimeType(kPrefixOfWebOSMediaMimeType, mime_type)) {
    if (net::MatchesMimeType(kPrefixOfWebOSExternalMimeType, mime_type))
      return media::MediaPlayerType::kMediaPlayerTypeExtInput;
    else if (net::MatchesMimeType(kPrefixOfWebOSPhotoMimeType, mime_type) ||
             net::MatchesMimeType(kPrefixOfWebOSPhotoCameraMimeType, mime_type))
      return media::MediaPlayerType::kMediaPlayerTypePhoto;
    else
      return media::MediaPlayerType::kMediaPlayerTypeNone;
  }
  // If url has no webOS prefix than we should handle it as a normal
  // LoadTypeURL
  return media::MediaPlayerType::kMediaPlayerTypeUMS;
}

MediaPlayerNeva* MediaPlayerNevaFactory::CreateMediaPlayerNeva(
    MediaPlayerNevaClient* client,
    const media::MediaPlayerType media_type,
    const scoped_refptr<base::SingleThreadTaskRunner>& main_task_runner,
    const std::string& app_id) {
  switch (media_type) {
#if defined(USE_GST_MEDIA) || defined(USE_SIGNAGE_MEDIA)
    case media::MediaPlayerType::kMediaPlayerTypeCamera:
      return new MediaPlayerCamera(client, main_task_runner, app_id);
      break;
#endif
#if defined(USE_TV_MEDIA) || defined(USE_SIGNAGE_MEDIA)
    case media::MediaPlayerType::kMediaPlayerTypeExtInput:
      return new MediaPlayerExtInput(client, main_task_runner, app_id);
      break;
    case media::MediaPlayerType::kMediaPlayerTypePhoto:
      return new MediaPlayerPhoto(client, main_task_runner, app_id);
      break;
#endif
    case media::MediaPlayerType::kMediaPlayerTypeUMS:
      return new MediaPlayerUMS(client, main_task_runner, app_id);
      break;
    default:
      break;
  }
  return nullptr;
}

}  // namespace media
