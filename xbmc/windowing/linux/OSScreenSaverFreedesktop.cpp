/*
 *  Copyright (C) 2017-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "OSScreenSaverFreedesktop.h"

#include "guilib/LocalizeStrings.h"
#include "platform/linux/PlatformConstants.h"
#include "utils/log.h"

using namespace KODI::WINDOWING::LINUX;

namespace
{
const std::string SCREENSAVER_DEST = "org.freedesktop.ScreenSaver";
const std::string SCREENSAVER_OBJECT = "/org/freedesktop/ScreenSaver";
const std::string SCREENSAVER_INTERFACE = "org.freedesktop.ScreenSaver";
}

bool COSScreenSaverFreedesktop::IsAvailable()
{
  bool available;
  auto proxy = sdbus::createObjectProxy(SCREENSAVER_DEST, SCREENSAVER_OBJECT);

  {
    try
    {
      proxy->callMethod("GetActive").onInterface(SCREENSAVER_INTERFACE).storeResultsTo(available);
    }
    catch(const sdbus::Error& e)
    {
      available = false;
    }
  }

  // We do not care whether GetActive() is actually supported, we're just checking for the name to be there
  // (GNOME for example does not support GetActive)
  return available;
}

COSScreenSaverFreedesktop::COSScreenSaverFreedesktop()
{
  m_proxy = sdbus::createObjectProxy(SCREENSAVER_DEST, SCREENSAVER_OBJECT);
}

void COSScreenSaverFreedesktop::Inhibit()
{
  if (m_inhibited)
  {
    return;
  }

  m_proxy->callMethod("Inhibit").onInterface(SCREENSAVER_INTERFACE).withArguments(KODI::LINUX::DESKTOP_FILE_NAME, g_localizeStrings.Get(14086)).storeResultsTo(m_cookie);

  m_inhibited = true;
}

void COSScreenSaverFreedesktop::Uninhibit()
{
  if (!m_inhibited)
  {
    return;
  }

  m_proxy->callMethod("Inhibit").onInterface(SCREENSAVER_INTERFACE).withArguments(m_cookie);

  m_inhibited = false;
}
