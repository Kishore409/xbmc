/*
 *  Copyright (C) 2012 Denis Yantarev
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "LogindUPowerSyscall.h"
#include "utils/log.h"

#include <string.h>
#include <unistd.h>

// logind DBus interface specification:
// http://www.freedesktop.org/wiki/Software/Logind/logind
//
// Inhibitor Locks documentation:
// http://www.freedesktop.org/wiki/Software/Logind/inhibit/

namespace
{
const std::string LOGIND_DEST{"org.freedesktop.login1"};
const std::string LOGIND_PATH{"/org/freedesktop/login1"};
const std::string LOGIND_IFACE{"org.freedesktop.login1.Manager"};
}

CLogindUPowerSyscall::CLogindUPowerSyscall()
{
  m_lowBattery = false;

  CLog::Log(LOGINFO, "Selected Logind/UPower as PowerSyscall");

  bool m_hasUPower;
  auto proxy = sdbus::createObjectProxy("org.freedesktop.UPower", "/org/freedesktop/UPower");

  {
    try
    {
      proxy->callMethod("EnumerateDevices").onInterface("org.freedesktop.UPower").storeResultsTo(m_hasUPower);
    }
    catch(const sdbus::Error& e)
    {
      m_hasUPower = false;
    }
  }

  if (!m_hasUPower)
    CLog::Log(LOGINFO, "LogindUPowerSyscall - UPower not found, battery information will not be available");

  m_proxy = sdbus::createObjectProxy(LOGIND_DEST, LOGIND_PATH);

  m_canPowerdown = LogindCheckCapability("CanPowerOff");
  m_canReboot    = LogindCheckCapability("CanReboot");
  m_canHibernate = LogindCheckCapability("CanHibernate");
  m_canSuspend   = LogindCheckCapability("CanSuspend");

  InhibitDelayLockSleep();

  m_batteryLevel = 0;
  if (m_hasUPower)
    UpdateBatteryLevel();

  //! @todo: subscribe signals
}

CLogindUPowerSyscall::~CLogindUPowerSyscall()
{
  ReleaseDelayLockSleep();
  ReleaseDelayLockShutdown();
}

bool CLogindUPowerSyscall::Powerdown()
{
  // delay shutdown so that the app can close properly
  InhibitDelayLockShutdown();
  return LogindSetPowerState("PowerOff");
}

bool CLogindUPowerSyscall::Reboot()
{
  return LogindSetPowerState("Reboot");
}

bool CLogindUPowerSyscall::Suspend()
{
  return LogindSetPowerState("Suspend");
}

bool CLogindUPowerSyscall::Hibernate()
{
  return LogindSetPowerState("Hibernate");
}

bool CLogindUPowerSyscall::CanPowerdown()
{
  return m_canPowerdown;
}

bool CLogindUPowerSyscall::CanSuspend()
{
  return m_canSuspend;
}

bool CLogindUPowerSyscall::CanHibernate()
{
  return m_canHibernate;
}

bool CLogindUPowerSyscall::CanReboot()
{
  return m_canReboot;
}

bool CLogindUPowerSyscall::HasLogind()
{
  // recommended method by systemd devs. The seats directory
  // doesn't exist unless logind created it and therefore is running.
  // see also https://mail.gnome.org/archives/desktop-devel-list/2013-March/msg00092.html
  return (access("/run/systemd/seats/", F_OK) >= 0);
}

bool CLogindUPowerSyscall::LogindSetPowerState(std::string state)
{
  m_proxy->callMethod(state).onInterface(LOGIND_IFACE).withArguments(false);

  return true;
}

bool CLogindUPowerSyscall::LogindCheckCapability(std::string capability)
{
  std::string reply;
  m_proxy->callMethod(capability).onInterface(LOGIND_IFACE).storeResultsTo(reply);

  return (reply == "yes") ? true : false;
}

int CLogindUPowerSyscall::BatteryLevel()
{
  return m_batteryLevel;
}

void CLogindUPowerSyscall::UpdateBatteryLevel()
{
  double batteryLevelSum{0};
  int batteryCount{0};

  auto proxy = sdbus::createObjectProxy("org.freedesktop.UPower", "/org/freedesktop/UPower");

  std::vector<sdbus::ObjectPath> reply;
  proxy->callMethod("EnumerateDevices").onInterface("org.freedesktop.UPower").storeResultsTo(reply);

  for (auto const &i : reply)
  {
    auto device = sdbus::createObjectProxy("org.freedesktop.UPower", i);
    auto rechargable = device->getProperty("IsRechargeable").onInterface("org.freedesktop.UPower.Device");
    auto percentage = device->getProperty("Percentage").onInterface("org.freedesktop.UPower.Device");

    if (rechargable)
    {
      batteryCount++;
      batteryLevelSum += percentage.get<double>();
    }
  }

  if (batteryCount > 0)
  {
    m_batteryLevel = static_cast<int>(batteryLevelSum / batteryCount);
  }
}

bool CLogindUPowerSyscall::PumpPowerEvents(IPowerEventsCallback *callback)
{
  //! @todo: implement signals

  return true;
}

void CLogindUPowerSyscall::InhibitDelayLockSleep()
{
  m_delayLockSleepFd = InhibitDelayLock("sleep");
}

void CLogindUPowerSyscall::InhibitDelayLockShutdown()
{
  m_delayLockShutdownFd = InhibitDelayLock("shutdown");
}

int CLogindUPowerSyscall::InhibitDelayLock(const char *what)
{
  int fd{-1};
  m_proxy->callMethod("Inhibit").onInterface("org.freedesktop.login1.Manager").withArguments(what, "Kodi", "", "delay").storeResultsTo(fd);

  return fd;
}

void CLogindUPowerSyscall::ReleaseDelayLockSleep()
{
  ReleaseDelayLock(m_delayLockSleepFd, "sleep");
  m_delayLockSleepFd = -1;
}

void CLogindUPowerSyscall::ReleaseDelayLockShutdown()
{
  ReleaseDelayLock(m_delayLockShutdownFd, "shutdown");
  m_delayLockShutdownFd = -1;
}

void CLogindUPowerSyscall::ReleaseDelayLock(int lockFd, const char *what)
{
  if (lockFd != -1)
  {
    close(lockFd);
    CLog::Log(LOGDEBUG, "LogindUPowerSyscall - delay lock %s released", what);
  }
}
