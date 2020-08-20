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

#include "media/neva/webos/media_player_ums.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/command_line.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/cdm_context.h"
#include "media/base/media_switches.h"
#include "media/base/pipeline.h"
#include "ui/gfx/geometry/rect_f.h"

#define FUNC_LOG(x) DVLOG(x) << __func__

namespace {

const char* WebOSClientBufferingStateToString(
    media::WebOSMediaClient::BufferingState state) {
#define STRINGIFY_WEBOSCLIENT_BUFFERINGSTATE_CASE(state) \
  case media::WebOSMediaClient::BufferingState::state:   \
    return #state

  switch (state) {
    STRINGIFY_WEBOSCLIENT_BUFFERINGSTATE_CASE(kHaveMetadata);
    STRINGIFY_WEBOSCLIENT_BUFFERINGSTATE_CASE(kLoadCompleted);
    STRINGIFY_WEBOSCLIENT_BUFFERINGSTATE_CASE(kPreloadCompleted);
    STRINGIFY_WEBOSCLIENT_BUFFERINGSTATE_CASE(kPrerollCompleted);
    STRINGIFY_WEBOSCLIENT_BUFFERINGSTATE_CASE(kWebOSBufferingStart);
    STRINGIFY_WEBOSCLIENT_BUFFERINGSTATE_CASE(kWebOSBufferingEnd);
    STRINGIFY_WEBOSCLIENT_BUFFERINGSTATE_CASE(kWebOSNetworkStateLoading);
    STRINGIFY_WEBOSCLIENT_BUFFERINGSTATE_CASE(kWebOSNetworkStateLoaded);
  }
  return "null";
}

}  // namespace

namespace media {

#define BIND_TO_RENDER_LOOP(function)                   \
  (DCHECK(main_task_runner_->BelongsToCurrentThread()), \
   media::BindToCurrentLoop(base::Bind(function, AsWeakPtr())))

static MediaPlayerNeva::MediaError convertToMediaError(PipelineStatus status) {
  switch (status) {
    case PIPELINE_OK:
      return MediaPlayerNeva::MEDIA_ERROR_NONE;

    case PIPELINE_ERROR_NETWORK:
      return MediaPlayerNeva::MEDIA_ERROR_INVALID_CODE;

    case PIPELINE_ERROR_DECODE:
      return MediaPlayerNeva::MEDIA_ERROR_DECODE;

    case PIPELINE_ERROR_DECRYPT:
    case PIPELINE_ERROR_ABORT:
    case PIPELINE_ERROR_INITIALIZATION_FAILED:
    case PIPELINE_ERROR_COULD_NOT_RENDER:
    case PIPELINE_ERROR_READ:
    case PIPELINE_ERROR_INVALID_STATE:
      return MediaPlayerNeva::MEDIA_ERROR_FORMAT;

    // Demuxer related errors.
    case DEMUXER_ERROR_COULD_NOT_OPEN:
    case DEMUXER_ERROR_COULD_NOT_PARSE:
    case DEMUXER_ERROR_NO_SUPPORTED_STREAMS:
      return MediaPlayerNeva::MEDIA_ERROR_INVALID_CODE;

    // Decoder related errors.
    case DECODER_ERROR_NOT_SUPPORTED:
      return MediaPlayerNeva::MEDIA_ERROR_FORMAT;

    // resource is released by policy action
    case DECODER_ERROR_RESOURCE_IS_RELEASED:
      return MediaPlayerNeva::MEDIA_ERROR_INVALID_CODE;

    // ChunkDemuxer related errors.
    case CHUNK_DEMUXER_ERROR_APPEND_FAILED:
    case CHUNK_DEMUXER_ERROR_EOS_STATUS_DECODE_ERROR:
    case CHUNK_DEMUXER_ERROR_EOS_STATUS_NETWORK_ERROR:
      return MediaPlayerNeva::MEDIA_ERROR_INVALID_CODE;

    // Audio rendering errors.
    case AUDIO_RENDERER_ERROR:
      return MediaPlayerNeva::MEDIA_ERROR_INVALID_CODE;

    default:
      return MediaPlayerNeva::MEDIA_ERROR_INVALID_CODE;
  }
  return MediaPlayerNeva::MEDIA_ERROR_NONE;
}

MediaPlayerUMS::MediaPlayerUMS(
    MediaPlayerNevaClient* client,
    const scoped_refptr<base::SingleThreadTaskRunner>& main_task_runner,
    const std::string& app_id)
    : client_(client), main_task_runner_(main_task_runner), app_id_(app_id) {
  LOG(ERROR) << __func__;
  umedia_client_ =
      WebOSMediaClient::Create(main_task_runner_, AsWeakPtr(), app_id_);
}

MediaPlayerUMS::~MediaPlayerUMS() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
}

void MediaPlayerUMS::Initialize(const bool is_video,
                                const double current_time,
                                const std::string& url,
                                const std::string& mime_type,
                                const std::string& referrer,
                                const std::string& user_agent,
                                const std::string& cookies,
                                const std::string& media_option,
                                const std::string& custom_option) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1) << __func__ << " app_id: " << app_id_ << " / url: " << url
              << " / media_option: " << media_option;

  current_time_ = base::TimeDelta::FromSecondsD(current_time);
  umedia_client_->Load(is_video, current_time, false, url, mime_type, referrer,
                       user_agent, cookies, media_option);
}

void MediaPlayerUMS::Start() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  umedia_client_->SetPlaybackRate(playback_rate_);
  paused_ = false;
}

void MediaPlayerUMS::Pause() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  umedia_client_->SetPlaybackRate(0.0f);
  paused_ = true;
  paused_time_ = current_time_;
}

void MediaPlayerUMS::Seek(const base::TimeDelta& time) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  umedia_client_->Seek(time, BIND_TO_RENDER_LOOP(&MediaPlayerUMS::OnSeekDone));
}

void MediaPlayerUMS::SetVolume(double volume) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  umedia_client_->SetPlaybackVolume(volume);
}

void MediaPlayerUMS::SetPoster(const GURL& poster) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
}

void MediaPlayerUMS::SetRate(double rate) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  if (!umedia_client_->IsSupportedBackwardTrickPlay() && rate < 0.0)
    return;

  playback_rate_ = rate;
  if (!paused_)
    umedia_client_->SetPlaybackRate(playback_rate_);
}

void MediaPlayerUMS::SetPreload(MediaPlayerNeva::Preload preload) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  switch (preload) {
    case MediaPlayerNeva::PreloadNone:
      umedia_client_->SetPreload(WebOSMediaClient::Preload::kPreloadNone);
      return;
    case MediaPlayerNeva::PreloadMetaData:
      umedia_client_->SetPreload(WebOSMediaClient::Preload::kPreloadMetaData);
      return;
    case MediaPlayerNeva::PreloadAuto:
      umedia_client_->SetPreload(WebOSMediaClient::Preload::kPreloadAuto);
      return;
  }
  NOTREACHED();
}

bool MediaPlayerUMS::IsPreloadable(const std::string& content_media_option) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  return umedia_client_->IsPreloadable(content_media_option);
}

bool MediaPlayerUMS::HasVideo() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  return umedia_client_->HasVideo();
}

bool MediaPlayerUMS::HasAudio() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  return umedia_client_->HasAudio();
}

bool MediaPlayerUMS::SelectTrack(const MediaTrackType type,
                                 const std::string& id) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  return umedia_client_->SelectTrack(type, id);
}

void MediaPlayerUMS::SwitchToAutoLayout() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  umedia_client_->SwitchToAutoLayout();
}

void MediaPlayerUMS::SetDisplayWindow(const gfx::Rect& outRect,
                                      const gfx::Rect& inRect,
                                      bool fullScreen,
                                      bool forced) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1) << "outRect:" << outRect.ToString()
              << " inRect:" << inRect.ToString();
  display_window_out_rect_ = outRect;
  display_window_in_rect_ = inRect;
  umedia_client_->SetDisplayWindow(outRect, inRect, fullScreen, forced);
}

bool MediaPlayerUMS::UsesIntrinsicSize() const {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  return umedia_client_->UsesIntrinsicSize();
}

std::string MediaPlayerUMS::MediaId() const {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  return umedia_client_->MediaId();
}

bool MediaPlayerUMS::HasAudioFocus() const {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  return umedia_client_ ? umedia_client_->Focus() : false;
}

void MediaPlayerUMS::SetAudioFocus(bool focus) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  if (umedia_client_ && focus && !umedia_client_->Focus()) {
    umedia_client_->SetFocus();
  }
}

bool MediaPlayerUMS::HasVisibility(void) const {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  return umedia_client_->Visibility();
}

void MediaPlayerUMS::SetVisibility(bool visibility) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  VLOG(1) << __func__ << " visibility=" << visibility;
  if (!is_video_offscreen_)
    umedia_client_->SetVisibility(visibility);
}

void MediaPlayerUMS::Suspend(SuspendReason reason) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  if (is_suspended_)
    return;

  is_suspended_ = true;
  umedia_client_->Suspend(reason);
}

void MediaPlayerUMS::Resume() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  FUNC_LOG(1);
  if (!is_suspended_)
    return;

  is_suspended_ = false;
  umedia_client_->Resume();
}

bool MediaPlayerUMS::RequireMediaResource() const {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  return true;
}

bool MediaPlayerUMS::IsRecoverableOnResume() const {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  return umedia_client_->IsRecoverableOnResume();
}

void MediaPlayerUMS::SetDisableAudio(bool disable) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  umedia_client_->SetDisableAudio(disable);
}

void MediaPlayerUMS::SetMediaLayerId(const std::string& media_layer_id) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  umedia_client_->SetMediaLayerId(media_layer_id);
}

media::Ranges<base::TimeDelta> MediaPlayerUMS::GetBufferedTimeRanges() const {
  return umedia_client_->GetBufferedTimeRanges();
}

void MediaPlayerUMS::OnPlaybackStateChanged(bool playing) {
  FUNC_LOG(1);
  if (playing)
    client_->OnMediaPlayerPlay();
  else
    client_->OnMediaPlayerPause();
}

void MediaPlayerUMS::OnPlaybackEnded() {
  FUNC_LOG(1);
  client_->OnPlaybackComplete();
}

void MediaPlayerUMS::OnSeekDone(PipelineStatus status) {
  FUNC_LOG(1);
  if (status != media::PIPELINE_OK) {
    client_->OnMediaError(convertToMediaError(status));
    return;
  }
  client_->OnSeekComplete(
      base::TimeDelta::FromSecondsD(umedia_client_->GetCurrentTime()));
}

void MediaPlayerUMS::OnError(PipelineStatus error) {
  FUNC_LOG(1);
  client_->OnMediaError(convertToMediaError(error));
}

void MediaPlayerUMS::OnBufferingStatusChanged(
    WebOSMediaClient::BufferingState buffering_state) {
  FUNC_LOG(2) << " state:"
              << WebOSClientBufferingStateToString(buffering_state);

  // TODO(neva): Ensure following states.
  switch (buffering_state) {
    case WebOSMediaClient::BufferingState::kHaveMetadata: {
      gfx::Size videoSize = umedia_client_->GetNaturalVideoSize();
      client_->OnMediaMetadataChanged(
          base::TimeDelta::FromSecondsD(umedia_client_->GetDuration()),
          videoSize.width(), videoSize.height(), true);
    } break;
    case WebOSMediaClient::BufferingState::kLoadCompleted:
      client_->OnLoadComplete();
      break;
    case WebOSMediaClient::BufferingState::kPreloadCompleted:
      client_->OnLoadComplete();
      break;
    case WebOSMediaClient::BufferingState::kPrerollCompleted:
      break;
    case WebOSMediaClient::BufferingState::kWebOSBufferingStart:
      break;
    case WebOSMediaClient::BufferingState::kWebOSBufferingEnd:
      break;
    case WebOSMediaClient::BufferingState::kWebOSNetworkStateLoading:
      break;
    case WebOSMediaClient::BufferingState::kWebOSNetworkStateLoaded:
      break;
  }
}

void MediaPlayerUMS::OnDurationChanged() {
  FUNC_LOG(1);
}

void MediaPlayerUMS::OnVideoSizeChanged() {
  FUNC_LOG(1);
  gfx::Size size = umedia_client_->GetNaturalVideoSize();
  client_->OnVideoSizeChanged(size.width(), size.height());
}

void MediaPlayerUMS::OnDisplayWindowChanged() {
  FUNC_LOG(1);
  umedia_client_->SetDisplayWindow(display_window_out_rect_,
                                   display_window_in_rect_, fullscreen_, true);
}

void MediaPlayerUMS::OnUMSInfoUpdated(const std::string& detail) {
  FUNC_LOG(1);
  if (!detail.empty())
    client_->OnCustomMessage(
        media::MediaEventType::kMediaEventUpdateUMSMediaInfo, detail);
}

void MediaPlayerUMS::OnAudioTrackAdded(
    const std::vector<MediaTrackInfo>& audio_track_info) {
  client_->OnAudioTracksUpdated(audio_track_info);
}

void MediaPlayerUMS::OnVideoTrackAdded(const std::string& id,
                                       const std::string& kind,
                                       const std::string& language,
                                       bool enabled) {
  NOTIMPLEMENTED();
}

void MediaPlayerUMS::OnAudioFocusChanged() {
  FUNC_LOG(1);
  client_->OnAudioFocusChanged();
}

void MediaPlayerUMS::OnActiveRegionChanged(const gfx::Rect& active_region) {
  FUNC_LOG(1) << gfx::Rect(active_region).ToString();

  if (active_video_region_ != active_region) {
    active_video_region_ = active_region;
    active_video_region_changed_ = true;
  }
  client_->OnActiveRegionChanged(active_video_region_);
}

void MediaPlayerUMS::OnWaitingForDecryptionKey() {
  NOTIMPLEMENTED();
}

void MediaPlayerUMS::OnEncryptedMediaInitData(
    const std::string& init_data_type,
    const std::vector<uint8_t>& init_data) {
  NOTIMPLEMENTED();
}

void MediaPlayerUMS::OnTimeUpdated(base::TimeDelta current_time) {
  current_time_ = current_time;
  if (client_)
    client_->OnTimeUpdate(current_time_, base::TimeTicks::Now());
}

}  // namespace media
