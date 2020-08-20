// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/shell/browser/shell_app_delegate.h"

#include "content/public/browser/file_select_listener.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"
#include "extensions/browser/media_capture_util.h"
#include "extensions/common/constants.h"
#include "extensions/shell/browser/shell_extension_web_contents_observer.h"

#if defined(USE_NEVA_APPRUNTIME)
#include "content/public/browser/render_view_host.h"
#include "extensions/common/switches.h"
#include "neva/app_runtime/public/mojom/app_runtime_webview.mojom.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#endif  // defined(USE_NEVA_APPRUNTIME

#if defined(USE_NEVA_APPRUNTIME) && defined(OS_WEBOS)
#include "neva/app_runtime/browser/app_runtime_webview_controller_impl.h"
#include "neva/app_runtime/public/webview_controller_delegate.h"
#endif  // defined(USE_NEVA_APPRUNTIME) && defined(OS_WEBOS)

#if defined(OS_WEBOS)
#include "extensions/shell/neva/webos_language_listener.h"
#include "extensions/shell/neva/webos_register_app.h"
#endif

namespace extensions {

#if defined(USE_NEVA_APPRUNTIME) && defined(OS_WEBOS)
namespace {

class ShellAppWebViewControllerDelegate
    : public neva_app_runtime::WebViewControllerDelegate {
 public:
  void RunCommand(const std::string& name,
                  const std::vector<std::string>& arguments) override {}

  std::string RunFunction(const std::string& name,
                          const std::vector<std::string>&) override {
    if (name == std::string("initialize")) {
      base::CommandLine* cmd = base::CommandLine::ForCurrentProcess();
      if (cmd->HasSwitch(extensions::switches::kWebOSAppId)) {
        std::stringstream result_stream;
        result_stream << "{\"identifier\":\""
                      << cmd->GetSwitchValueASCII(
                             extensions::switches::kWebOSAppId)
                      << "\",\"devicePixelRatio\":2}";
        return result_stream.str();
      }
    } else if (name == std::string("identifier")) {
      base::CommandLine* cmd = base::CommandLine::ForCurrentProcess();
      if (cmd->HasSwitch(extensions::switches::kWebOSAppId))
        return cmd->GetSwitchValueASCII(extensions::switches::kWebOSAppId);
    } else if (name == std::string("devicePixelRatio")) {
      return std::string("2");
    }
    return std::string();
  }
};

}  // namespace
#endif

ShellAppDelegate::ShellAppDelegate() {
}

ShellAppDelegate::~ShellAppDelegate() {
}

void ShellAppDelegate::InitWebContents(content::WebContents* web_contents) {
  ShellExtensionWebContentsObserver::CreateForWebContents(web_contents);
#if defined(OS_WEBOS)
  content::WebContentsUserData<webos::LanguageListener>::CreateForWebContents(
      web_contents);
  content::WebContentsUserData<webos::RegisterApp>::CreateForWebContents(
      web_contents);
#endif
}

void ShellAppDelegate::RenderViewCreated(
    content::RenderViewHost* render_view_host) {
  // The views implementation of AppWindow takes focus via SetInitialFocus()
  // and views::WebView but app_shell is aura-only and must do it manually.
  content::WebContents::FromRenderViewHost(render_view_host)->Focus();

#if defined(USE_NEVA_APPRUNTIME)
  mojo::AssociatedRemote<neva_app_runtime::mojom::AppRuntimeWebViewClient>
      client;
  render_view_host->GetMainFrame()->GetRemoteAssociatedInterfaces()
      ->GetInterface(&client);

#if defined(ENABLE_MEMORYMANAGER_WEBAPI)
  client->AddInjectionToLoad(std::string("v8/memorymanager"));
#endif  // defined(ENABLE_MEMORYMANAGER_WEBAPI)

#if defined(OS_WEBOS)
  content::WebContents* web_contents =
      content::WebContents::FromRenderViewHost(render_view_host);
  shell_app_webview_controller_impl_ =
      std::make_unique<neva_app_runtime::AppRuntimeWebViewControllerImpl>(
          web_contents);

  shell_app_webview_controller_delegate_ =
      std::make_unique<ShellAppWebViewControllerDelegate>();
  shell_app_webview_controller_impl_->SetDelegate(
      shell_app_webview_controller_delegate_.get());

  client->AddInjectionToLoad(std::string("v8/webosservicebridge"));
#endif  // defined(OS_WEBOS)
#endif  // defined(USE_NEVA_APPRUNTIME)
}

void ShellAppDelegate::ResizeWebContents(content::WebContents* web_contents,
                                         const gfx::Size& size) {
  NOTIMPLEMENTED();
}

content::WebContents* ShellAppDelegate::OpenURLFromTab(
    content::BrowserContext* context,
    content::WebContents* source,
    const content::OpenURLParams& params) {
  NOTIMPLEMENTED();
  return NULL;
}

void ShellAppDelegate::AddNewContents(
    content::BrowserContext* context,
    std::unique_ptr<content::WebContents> new_contents,
    const GURL& target_url,
    WindowOpenDisposition disposition,
    const gfx::Rect& initial_rect,
    bool user_gesture) {
  NOTIMPLEMENTED();
}

content::ColorChooser* ShellAppDelegate::ShowColorChooser(
    content::WebContents* web_contents,
    SkColor initial_color) {
  NOTIMPLEMENTED();
  return NULL;
}

void ShellAppDelegate::RunFileChooser(
    content::RenderFrameHost* render_frame_host,
    std::unique_ptr<content::FileSelectListener> listener,
    const blink::mojom::FileChooserParams& params) {
  NOTIMPLEMENTED();
  listener->FileSelectionCanceled();
}

void ShellAppDelegate::RequestMediaAccessPermission(
    content::WebContents* web_contents,
    const content::MediaStreamRequest& request,
    content::MediaResponseCallback callback,
    const extensions::Extension* extension) {
  media_capture_util::GrantMediaStreamRequest(web_contents, request,
                                              std::move(callback), extension);
}

bool ShellAppDelegate::CheckMediaAccessPermission(
    content::RenderFrameHost* render_frame_host,
    const GURL& security_origin,
    blink::mojom::MediaStreamType type,
    const Extension* extension) {
  if (type == blink::mojom::MediaStreamType::DEVICE_AUDIO_CAPTURE ||
      type == blink::mojom::MediaStreamType::DEVICE_VIDEO_CAPTURE) {
    // VerifyMediaAccessPermission() will crash if there is
    // no permission for audio capture / video capture.
    // Let's make an error log and return false instead.
    // TODO(alexander.trofimov@lge.com): Remove this patch
    // right after corresponding features are supported
    // and crash removed from VerifyMediaAccessPermission().
    LOG(ERROR) << "Audio capture/video capture request but "
               << "this feature is not supported yet.";
    return false;
  }
  media_capture_util::VerifyMediaAccessPermission(type, extension);
  return true;
}

int ShellAppDelegate::PreferredIconSize() const {
  return extension_misc::EXTENSION_ICON_SMALL;
}

void ShellAppDelegate::SetWebContentsBlocked(
    content::WebContents* web_contents,
    bool blocked) {
  NOTIMPLEMENTED();
}

bool ShellAppDelegate::IsWebContentsVisible(
    content::WebContents* web_contents) {
  return true;
}

void ShellAppDelegate::SetTerminatingCallback(base::OnceClosure callback) {
  // TODO(jamescook): Should app_shell continue to close the app window
  // manually or should it use a browser termination callback like Chrome?
}

bool ShellAppDelegate::TakeFocus(content::WebContents* web_contents,
                                 bool reverse) {
  return false;
}

content::PictureInPictureResult ShellAppDelegate::EnterPictureInPicture(
    content::WebContents* web_contents,
    const viz::SurfaceId& surface_id,
    const gfx::Size& natural_size) {
  NOTREACHED();
  return content::PictureInPictureResult::kNotSupported;
}

void ShellAppDelegate::ExitPictureInPicture() {
  NOTREACHED();
}

}  // namespace extensions
