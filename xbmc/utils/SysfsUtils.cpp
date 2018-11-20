/*
 *  Copyright (C) 2011-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "SysfsUtils.h"

#include "utils/log.h"

#include <fstream>

#include <unistd.h>

namespace
{
const int PAGESIZE = getpagesize();
}

template<>
std::string CSysfs::Get()
{
  std::ifstream file(m_path);

  if (!file.is_open())
    return "";

  std::string value;
  file >> value;

  if ((file.rdstate() & std::ifstream::badbit) != 0)
  {
    CLog::Log(LOGERROR, "CSysfs::{} error reading from '{}'", __FUNCTION__, m_path);
    return "";
  }

  return value;
}

template<>
int CSysfs::Get()
{
  try
  {
    return std::stoi(Get<std::string>());
  }
  catch (std::invalid_argument& error)
  {
    return -1;
  }
}

template<>
bool CSysfs::Set(const std::string& value)
{
  if (Get<std::string>() == value)
    return true;

  if (static_cast<int>(value.size()) > PAGESIZE)
  {
    CLog::Log(LOGERROR, "CSysfs::{} value '{}' is larger then the system pagesize: {} vs {}",
              __FUNCTION__, value, value.size(), PAGESIZE);
    return false;
  }

  std::ofstream file(m_path);

  if (!file.is_open())
  {
    CLog::Log(LOGERROR, "CSysfs::{} {} is not open for writing", __FUNCTION__, m_path);
    return false;
  }

  file << value;

  if ((file.rdstate() & std::ifstream::badbit) != 0)
  {
    CLog::Log(LOGERROR, "CSysfs::{} error writing to '{}'", __FUNCTION__, m_path);
    return false;
  }

  return true;
}

template<>
bool CSysfs::Set(const int& value)
{
  return Set(std::to_string(value));
}
