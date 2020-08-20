// Copyright 2014 Intel Corporation. All rights reserved.
// Copyright 2014-2018 LG Electronics, Inc.
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

#ifndef OZONE_WAYLAND_SHELL_IVI_SHELL_SURFACE_H_
#define OZONE_WAYLAND_SHELL_IVI_SHELL_SURFACE_H_

#include "ozone/wayland/shell/shell_surface.h"

struct ivi_surface;

namespace ozonewayland {

class WaylandSurface;
class WaylandWindow;

class IVIShellSurface : public WaylandShellSurface {
 public:
  IVIShellSurface();
  ~IVIShellSurface() override;

  void InitializeShellSurface(WaylandWindow* window,
                              WaylandWindow::ShellType type) override;
  void UpdateShellSurface(WaylandWindow::ShellType type,
                          WaylandShellSurface* shell_parent,
                          int x,
                          int y) override;
  void SetWindowTitle(const base::string16& title) override;
  void Maximize() override;
  void Minimize() override;
  void Unminimize() override;
  bool IsMinimized() const override;

 private:
  ivi_surface* ivi_surface_;
  int ivi_surface_id_;
  static int last_ivi_surface_id_;
  DISALLOW_COPY_AND_ASSIGN(IVIShellSurface);
};

}  // namespace ozonewayland

#endif  // OZONE_WAYLAND_SHELL_IVI_SHELL_SURFACE_H_
