// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_BLUETOOTH_BLUETOOTH_CHOOSER_CONTEXT_FACTORY_H_
#define CHROME_BROWSER_BLUETOOTH_BLUETOOTH_CHOOSER_CONTEXT_FACTORY_H_

#include "base/macros.h"
#include "base/no_destructor.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"

class BluetoothChooserContext;
class Profile;

class BluetoothChooserContextFactory
    : public BrowserContextKeyedServiceFactory {
 public:
  static BluetoothChooserContext* GetForProfile(Profile* profile);
  static BluetoothChooserContextFactory* GetInstance();

  // Move-only class.
  BluetoothChooserContextFactory(const BluetoothChooserContextFactory&) =
      delete;
  BluetoothChooserContextFactory& operator=(
      const BluetoothChooserContextFactory&) = delete;

 private:
  friend base::NoDestructor<BluetoothChooserContextFactory>;

  BluetoothChooserContextFactory();
  ~BluetoothChooserContextFactory() override;

  // BrowserContextKeyedBaseFactory implementation:
  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* profile) const override;
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
};

#endif  // CHROME_BROWSER_BLUETOOTH_BLUETOOTH_CHOOSER_CONTEXT_FACTORY_H_