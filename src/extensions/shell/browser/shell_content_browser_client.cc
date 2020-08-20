// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/shell/browser/shell_content_browser_client.h"

#include <stddef.h>

#include <utility>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/stl_util.h"
#include "base/task/post_task.h"
#include "components/guest_view/browser/guest_view_message_filter.h"
#include "components/nacl/common/buildflags.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/site_instance.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_descriptors.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "content/public/common/user_agent.h"
#include "content/shell/browser/shell_browser_context.h"
#include "content/shell/browser/shell_devtools_manager_delegate.h"
#include "extensions/browser/api/web_request/web_request_api.h"
#include "extensions/browser/extension_message_filter.h"
#include "extensions/browser/extension_navigation_throttle.h"
#include "extensions/browser/extension_navigation_ui_data.h"
#include "extensions/browser/extension_protocols.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/guest_view/extensions_guest_view_message_filter.h"
#include "extensions/browser/guest_view/web_view/web_view_guest.h"
#include "extensions/browser/info_map.h"
#include "extensions/browser/process_map.h"
#include "extensions/browser/url_loader_factory_manager.h"
#include "extensions/common/constants.h"
#include "extensions/common/extension.h"
#include "extensions/common/switches.h"
#include "extensions/shell/browser/shell_browser_context.h"
#include "extensions/shell/browser/shell_browser_main_parts.h"
#include "extensions/shell/browser/shell_extension_system.h"
#include "extensions/shell/browser/shell_navigation_ui_data.h"
#include "extensions/shell/browser/shell_speech_recognition_manager_delegate.h"
#include "extensions/shell/common/version.h"  // Generated file.
#include "url/gurl.h"

#if BUILDFLAG(ENABLE_NACL)
#include "components/nacl/browser/nacl_browser.h"
#include "components/nacl/browser/nacl_host_message_filter.h"
#include "components/nacl/browser/nacl_process_host.h"
#include "components/nacl/common/nacl_process_type.h"  // nogncheck
#include "components/nacl/common/nacl_switches.h"      // nogncheck
#include "content/public/browser/browser_child_process_host.h"
#include "content/public/browser/child_process_data.h"
#endif

#if defined(USE_NEVA_APPRUNTIME)
#include "base/neva/base_switches.h"
#include "content/public/browser/login_delegate.h"
#include "content/public/browser/network_service_instance.h"
#include "content/shell/common/shell_neva_switches.h"
#include "extensions/browser/extension_util.h"
#include "extensions/common/manifest_handlers/app_isolation_info.h"
#include "neva/pal_service/public/mojom/constants.mojom.h"
#include "services/network/public/mojom/network_service.mojom.h"
#include "ui/base/ui_base_neva_switches.h"
#endif

using base::CommandLine;
using content::BrowserContext;
using content::BrowserThread;

namespace extensions {
namespace {
#if defined(USE_NEVA_APPRUNTIME)
const char kCacheStoreFile[] = "Cache";
const char kCookieStoreFile[] = "Cookies";
const int kDefaultDiskCacheSize = 16 * 1024 * 1024;  // default size is 16MB

void AuthRequestCallback(
    LoginAuthRequiredCallback callback,
    const base::Optional<net::AuthCredentials>& credentials,
    bool should_cancel) {
  if (credentials) {
    std::move(callback).Run(credentials);
    return;
  }
}

bool SiteHasIsolatedStorage(const GURL& extension_site_url,
                            content::BrowserContext* context) {
  const Extension* extension = ExtensionRegistry::Get(context)
                                   ->enabled_extensions()
                                   .GetExtensionOrAppByURL(extension_site_url);

  return extension && AppIsolationInfo::HasIsolatedStorage(extension);
}
#endif

ShellContentBrowserClient* g_instance = nullptr;

}  // namespace

ShellContentBrowserClient::ShellContentBrowserClient(
    ShellBrowserMainDelegate* browser_main_delegate)
    : browser_main_parts_(nullptr),
      browser_main_delegate_(browser_main_delegate) {
  DCHECK(!g_instance);
  g_instance = this;
}

ShellContentBrowserClient::~ShellContentBrowserClient() {
  g_instance = nullptr;
}

// static
ShellContentBrowserClient* ShellContentBrowserClient::Get() {
  return g_instance;
}

content::BrowserContext* ShellContentBrowserClient::GetBrowserContext() {
  return browser_main_parts_->browser_context();
}

std::unique_ptr<content::BrowserMainParts>
ShellContentBrowserClient::CreateBrowserMainParts(
    const content::MainFunctionParams& parameters) {
  auto browser_main_parts =
      CreateShellBrowserMainParts(parameters, browser_main_delegate_);

  browser_main_parts_ = browser_main_parts.get();

  return browser_main_parts;
}

void ShellContentBrowserClient::RenderProcessWillLaunch(
    content::RenderProcessHost* host) {
  int render_process_id = host->GetID();
  BrowserContext* browser_context = browser_main_parts_->browser_context();
  host->AddFilter(
      new ExtensionMessageFilter(render_process_id, browser_context));
  host->AddFilter(
      new ExtensionsGuestViewMessageFilter(
          render_process_id, browser_context));
  // PluginInfoMessageFilter is not required because app_shell does not have
  // the concept of disabled plugins.
#if BUILDFLAG(ENABLE_NACL)
  host->AddFilter(new nacl::NaClHostMessageFilter(
      render_process_id, browser_context->IsOffTheRecord(),
      browser_context->GetPath()));
#endif
}

bool ShellContentBrowserClient::ShouldUseProcessPerSite(
    content::BrowserContext* browser_context,
    const GURL& site_url) {
  // This ensures that all render views created for a single app will use the
  // same render process (see content::SiteInstance::GetProcess). Otherwise the
  // default behavior of ContentBrowserClient will lead to separate render
  // processes for the background page and each app window view.
  return true;
}

bool ShellContentBrowserClient::IsHandledURL(const GURL& url) {
  if (!url.is_valid())
    return false;
  // Keep in sync with ProtocolHandlers added in
  // ShellBrowserContext::CreateRequestContext() and in
  // content::ShellURLRequestContextGetter::GetURLRequestContext().
  static const char* const kProtocolList[] = {
      url::kBlobScheme,
      content::kChromeDevToolsScheme,
      content::kChromeUIScheme,
      url::kDataScheme,
      url::kFileScheme,
      url::kFileSystemScheme,
      kExtensionScheme,
  };
  for (const char* scheme : kProtocolList) {
    if (url.SchemeIs(scheme))
      return true;
  }
  return false;
}

void ShellContentBrowserClient::SiteInstanceGotProcess(
    content::SiteInstance* site_instance) {
  // If this isn't an extension renderer there's nothing to do.
  const Extension* extension = GetExtension(site_instance);
  if (!extension)
    return;

#if defined(USE_NEVA_APPRUNTIME)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          ::switches::kV8SnapshotBlobPath)) {
    v8_snapshot_path_ =
        std::make_pair(site_instance->GetProcess()->GetID(),
                       base::CommandLine::ForCurrentProcess()
                           ->GetSwitchValuePath(::switches::kV8SnapshotBlobPath)
                           .value());
  }
#endif
  ProcessMap::Get(browser_main_parts_->browser_context())
      ->Insert(extension->id(),
               site_instance->GetProcess()->GetID(),
               site_instance->GetId());

  base::PostTask(
      FROM_HERE, {BrowserThread::IO},
      base::BindOnce(&InfoMap::RegisterExtensionProcess,
                     browser_main_parts_->extension_system()->info_map(),
                     extension->id(), site_instance->GetProcess()->GetID(),
                     site_instance->GetId()));
}

void ShellContentBrowserClient::SiteInstanceDeleting(
    content::SiteInstance* site_instance) {
  // Don't do anything if we're shutting down.
  if (content::BrowserMainRunner::ExitedMainMessageLoop())
    return;

  // If this isn't an extension renderer there's nothing to do.
  const Extension* extension = GetExtension(site_instance);
  if (!extension)
    return;

  ProcessMap::Get(browser_main_parts_->browser_context())
      ->Remove(extension->id(),
               site_instance->GetProcess()->GetID(),
               site_instance->GetId());

  base::PostTask(
      FROM_HERE, {BrowserThread::IO},
      base::BindOnce(&InfoMap::UnregisterExtensionProcess,
                     browser_main_parts_->extension_system()->info_map(),
                     extension->id(), site_instance->GetProcess()->GetID(),
                     site_instance->GetId()));
}

void ShellContentBrowserClient::AppendExtraCommandLineSwitches(
    base::CommandLine* command_line,
    int child_process_id) {
  std::string process_type =
      command_line->GetSwitchValueASCII(::switches::kProcessType);
  if (process_type == ::switches::kRendererProcess)
    AppendRendererSwitches(command_line);
#if defined(USE_NEVA_APPRUNTIME)
  // Append v8 snapshot path if given
  if (v8_snapshot_path_.first == child_process_id) {
    command_line->AppendSwitchPath(::switches::kV8SnapshotBlobPath,
                                   base::FilePath(v8_snapshot_path_.second));
  }

  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          ::switches::kUseOzoneWaylandVkb))
    command_line->AppendSwitch(::switches::kUseOzoneWaylandVkb);

  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          ::switches::kOzoneWaylandUseXDGShell))
    command_line->AppendSwitch(::switches::kOzoneWaylandUseXDGShell);
#endif
}

content::SpeechRecognitionManagerDelegate*
ShellContentBrowserClient::CreateSpeechRecognitionManagerDelegate() {
  return new speech::ShellSpeechRecognitionManagerDelegate();
}

content::BrowserPpapiHost*
ShellContentBrowserClient::GetExternalBrowserPpapiHost(int plugin_process_id) {
#if BUILDFLAG(ENABLE_NACL)
  content::BrowserChildProcessHostIterator iter(PROCESS_TYPE_NACL_LOADER);
  while (!iter.Done()) {
    nacl::NaClProcessHost* host = static_cast<nacl::NaClProcessHost*>(
        iter.GetDelegate());
    if (host->process() &&
        host->process()->GetData().id == plugin_process_id) {
      // Found the plugin.
      return host->browser_ppapi_host();
    }
    ++iter;
  }
#endif
  return nullptr;
}

void ShellContentBrowserClient::GetAdditionalAllowedSchemesForFileSystem(
    std::vector<std::string>* additional_allowed_schemes) {
  ContentBrowserClient::GetAdditionalAllowedSchemesForFileSystem(
      additional_allowed_schemes);
  additional_allowed_schemes->push_back(kExtensionScheme);
}

content::DevToolsManagerDelegate*
ShellContentBrowserClient::GetDevToolsManagerDelegate() {
  return new content::ShellDevToolsManagerDelegate(GetBrowserContext());
}

std::vector<std::unique_ptr<content::NavigationThrottle>>
ShellContentBrowserClient::CreateThrottlesForNavigation(
    content::NavigationHandle* navigation_handle) {
  std::vector<std::unique_ptr<content::NavigationThrottle>> throttles;
  throttles.push_back(
      std::make_unique<ExtensionNavigationThrottle>(navigation_handle));
  return throttles;
}

std::unique_ptr<content::NavigationUIData>
ShellContentBrowserClient::GetNavigationUIData(
    content::NavigationHandle* navigation_handle) {
  return std::make_unique<ShellNavigationUIData>(navigation_handle);
}

void ShellContentBrowserClient::RegisterNonNetworkNavigationURLLoaderFactories(
    int frame_tree_node_id,
    NonNetworkURLLoaderFactoryMap* factories) {
  content::WebContents* web_contents =
      content::WebContents::FromFrameTreeNodeId(frame_tree_node_id);
  factories->emplace(
      extensions::kExtensionScheme,
      extensions::CreateExtensionNavigationURLLoaderFactory(
          web_contents->GetBrowserContext(),
          !!extensions::WebViewGuest::FromWebContents(web_contents)));
}

void ShellContentBrowserClient::
    RegisterNonNetworkWorkerMainResourceURLLoaderFactories(
        content::BrowserContext* browser_context,
        NonNetworkURLLoaderFactoryMap* factories) {
  DCHECK(browser_context);
  DCHECK(factories);
  factories->emplace(
      extensions::kExtensionScheme,
      extensions::CreateExtensionWorkerMainResourceURLLoaderFactory(
          browser_context));
}

void ShellContentBrowserClient::
    RegisterNonNetworkServiceWorkerUpdateURLLoaderFactories(
        content::BrowserContext* browser_context,
        NonNetworkURLLoaderFactoryMap* factories) {
  DCHECK(browser_context);
  DCHECK(factories);
  factories->emplace(
      extensions::kExtensionScheme,
      extensions::CreateExtensionServiceWorkerScriptURLLoaderFactory(
          browser_context));
}

void ShellContentBrowserClient::RegisterNonNetworkSubresourceURLLoaderFactories(
    int render_process_id,
    int render_frame_id,
    NonNetworkURLLoaderFactoryMap* factories) {
  auto factory = extensions::CreateExtensionURLLoaderFactory(render_process_id,
                                                             render_frame_id);
  if (factory)
    factories->emplace(extensions::kExtensionScheme, std::move(factory));
}

bool ShellContentBrowserClient::WillCreateURLLoaderFactory(
    content::BrowserContext* browser_context,
    content::RenderFrameHost* frame,
    int render_process_id,
    URLLoaderFactoryType type,
    const url::Origin& request_initiator,
    base::Optional<int64_t> navigation_id,
    mojo::PendingReceiver<network::mojom::URLLoaderFactory>* factory_receiver,
    mojo::PendingRemote<network::mojom::TrustedURLLoaderHeaderClient>*
        header_client,
    bool* bypass_redirect_checks,
    bool* disable_secure_dns,
    network::mojom::URLLoaderFactoryOverridePtr* factory_override) {
  auto* web_request_api =
      extensions::BrowserContextKeyedAPIFactory<extensions::WebRequestAPI>::Get(
          browser_context);
  bool use_proxy = web_request_api->MaybeProxyURLLoaderFactory(
      browser_context, frame, render_process_id, type, std::move(navigation_id),
      factory_receiver, header_client);
  if (bypass_redirect_checks)
    *bypass_redirect_checks = use_proxy;
  return use_proxy;
}

bool ShellContentBrowserClient::HandleExternalProtocol(
    const GURL& url,
    content::WebContents::OnceGetter web_contents_getter,
    int child_id,
    content::NavigationUIData* navigation_data,
    bool is_main_frame,
    ui::PageTransition page_transition,
    bool has_user_gesture,
    const base::Optional<url::Origin>& initiating_origin,
    mojo::PendingRemote<network::mojom::URLLoaderFactory>* out_factory) {
  return false;
}

void ShellContentBrowserClient::OverrideURLLoaderFactoryParams(
    content::BrowserContext* browser_context,
    const url::Origin& origin,
    bool is_for_isolated_world,
    network::mojom::URLLoaderFactoryParams* factory_params) {
  URLLoaderFactoryManager::OverrideURLLoaderFactoryParams(
      browser_context, origin, is_for_isolated_world, factory_params);
}

std::string ShellContentBrowserClient::GetUserAgent() {
  // Must contain a user agent string for version sniffing. For example,
  // pluginless WebRTC Hangouts checks the Chrome version number.
  return content::BuildUserAgentFromProduct("Chrome/" PRODUCT_VERSION);
}

std::unique_ptr<ShellBrowserMainParts>
ShellContentBrowserClient::CreateShellBrowserMainParts(
    const content::MainFunctionParams& parameters,
    ShellBrowserMainDelegate* browser_main_delegate) {
  return std::make_unique<ShellBrowserMainParts>(parameters,
                                                 browser_main_delegate);
}

void ShellContentBrowserClient::AppendRendererSwitches(
    base::CommandLine* command_line) {
  static const char* const kSwitchNames[] = {
      switches::kWhitelistedExtensionID,
      // TODO(jamescook): Should we check here if the process is in the
      // extension service process map, or can we assume all renderers are
      // extension renderers?
      switches::kExtensionProcess,
  };
  command_line->CopySwitchesFrom(*base::CommandLine::ForCurrentProcess(),
                                 kSwitchNames, base::size(kSwitchNames));

#if BUILDFLAG(ENABLE_NACL)
  // NOTE: app_shell does not support non-SFI mode, so it does not pass through
  // SFI switches either here or for the zygote process.
  static const char* const kNaclSwitchNames[] = {
      ::switches::kEnableNaClDebug,
  };
  command_line->CopySwitchesFrom(*base::CommandLine::ForCurrentProcess(),
                                 kNaclSwitchNames,
                                 base::size(kNaclSwitchNames));
#endif  // BUILDFLAG(ENABLE_NACL)
}

const Extension* ShellContentBrowserClient::GetExtension(
    content::SiteInstance* site_instance) {
  ExtensionRegistry* registry =
      ExtensionRegistry::Get(site_instance->GetBrowserContext());
  return registry->enabled_extensions().GetExtensionOrAppByURL(
      site_instance->GetSiteURL());
}

#if defined(USE_NEVA_APPRUNTIME)
std::unique_ptr<content::LoginDelegate>
ShellContentBrowserClient::CreateLoginDelegate(
    const net::AuthChallengeInfo& auth_info,
    content::WebContents* web_contents,
    const content::GlobalRequestID& request_id,
    bool is_request_for_main_frame,
    const GURL& url,
    scoped_refptr<net::HttpResponseHeaders> response_headers,
    bool first_auth_attempt,
    LoginAuthRequiredCallback auth_required_callback) {
  BrowserContext* browser_context = GetBrowserContext();
  extensions::WebRequestAPI* api =
      extensions::BrowserContextKeyedAPIFactory<extensions::WebRequestAPI>::Get(
          browser_context);
  auto continuation =
      base::BindOnce(&AuthRequestCallback, std::move(auth_required_callback));

  if (api->MaybeProxyAuthRequest(
          browser_context, auth_info, std::move(response_headers), request_id,
          is_request_for_main_frame, std::move(continuation))) {
  }

  return std::make_unique<content::LoginDelegate>();
}

void ShellContentBrowserClient::GetStoragePartitionConfigForSite(
    content::BrowserContext* browser_context,
    const GURL& site,
    bool can_be_default,
    std::string* partition_domain,
    std::string* partition_name,
    bool* in_memory) {
  // Default to the browser-wide storage partition and override based on |site|
  // below.
  partition_domain->clear();
  partition_name->clear();
  *in_memory = false;

  bool success = extensions::WebViewGuest::GetGuestPartitionConfigForSite(
      site, partition_domain, partition_name, in_memory);

  if (!success && site.SchemeIs(extensions::kExtensionScheme)) {
    // If |can_be_default| is false, the caller is stating that the |site|
    // should be parsed as if it had isolated storage. In particular it is
    // important to NOT check ExtensionService for the is_storage_isolated()
    // attribute because this code path is run during Extension uninstall
    // to do cleanup after the Extension has already been unloaded from the
    // ExtensionService.
    bool is_isolated = !can_be_default;
    if (can_be_default) {
      if (SiteHasIsolatedStorage(site, browser_context))
        is_isolated = true;
    }

    if (is_isolated) {
      CHECK(site.has_host());
      // For extensions with isolated storage, the the host of the |site| is
      // the |partition_domain|. The |in_memory| and |partition_name| are only
      // used in guest schemes so they are cleared here.
      *partition_domain = site.host();
      *in_memory = false;
      partition_name->clear();
    }
    success = true;
  }

  // Assert that if |can_be_default| is false, the code above must have found a
  // non-default partition.  If this fails, the caller has a serious logic
  // error about which StoragePartition they expect to be in and it is not
  // safe to continue.
  CHECK(can_be_default || !partition_domain->empty());
}

void ShellContentBrowserClient::ConfigureNetworkContextParams(
    content::BrowserContext* context,
    bool in_memory,
    const base::FilePath& relative_partition_path,
    network::mojom::NetworkContextParams* network_context_params,
    network::mojom::CertVerifierCreationParams* cert_verifier_creation_params) {
  network_context_params->user_agent = GetUserAgent();
  network_context_params->accept_language = "en-us,en";

  int disk_cache_size = kDefaultDiskCacheSize;
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
  if (cmd_line->HasSwitch(::switches::kShellDiskCacheSize))
    base::StringToInt(
        cmd_line->GetSwitchValueASCII(::switches::kShellDiskCacheSize),
        &disk_cache_size);

  network_context_params->cookie_path =
      context->GetPath().Append(kCookieStoreFile);
  network_context_params->enable_encrypted_cookies = false;
  network_context_params->http_cache_max_size = disk_cache_size;
  network_context_params->http_cache_path =
      context->GetPath().Append(kCacheStoreFile);

  // Only firstly created NetworkContext should be marked as primary.
  // For app-shell case first creation request has empty argument
  // relative_partition_path.
  // Next requests (if any) will have this argument not empty.
  if (relative_partition_path.empty())
    network_context_params->primary_network_context = true;
}
#endif

}  // namespace extensions
