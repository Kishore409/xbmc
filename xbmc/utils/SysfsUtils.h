/*
 *  Copyright (C) 2011-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <string>

class CSysfs
{
public:
  CSysfs() = default;
  CSysfs(const std::string& path) : m_path(path) {}
  ~CSysfs() = default;

  void operator=(const std::string& path) { m_path = path; }

  template<typename T>
  T Get();

  template<typename T>
  bool Set(const T& value);

private:
  std::string m_path;
};
