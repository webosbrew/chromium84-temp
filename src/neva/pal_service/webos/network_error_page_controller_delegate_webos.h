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

#ifndef NEVA_PAL_SERVICE_WEBOS_NETWORK_ERROR_PAGE_CONTROLLER_DELEGATE_WEBOS_H_
#define NEVA_PAL_SERVICE_WEBOS_NETWORK_ERROR_PAGE_CONTROLLER_DELEGATE_WEBOS_H_

#include <string>

#include "base/callback.h"
#include "neva/pal_service/network_error_page_controller_delegate.h"
#include "neva/pal_service/webos/luna/luna_client.h"

namespace pal {
namespace webos {

class NetworkErrorPageControllerDelegateWebOS
    : public NetworkErrorPageControllerDelegate {
 public:
  NetworkErrorPageControllerDelegateWebOS();
  ~NetworkErrorPageControllerDelegateWebOS() override;

  NetworkErrorPageControllerDelegateWebOS(
      const NetworkErrorPageControllerDelegateWebOS&) = delete;
  NetworkErrorPageControllerDelegateWebOS& operator=(
      const NetworkErrorPageControllerDelegateWebOS&) = delete;

  void LaunchNetworkSettings(int target_id) override;

 private:
  void OnLaunchNetworkSettings(luna::Client::ResponseStatus,
                               unsigned token,
                               const std::string& json);
  std::unique_ptr<luna::Client> luna_client_;
};

}  // namespace webos
}  // namespace pal

#endif  // NEVA_PAL_SERVICE_WEBOS_NETWORK_ERROR_PAGE_CONTROLLER_DELEGATE_WEBOS_H_
