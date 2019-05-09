/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "utils/SysfsUtils.h"

#include <fstream>

#include <gtest/gtest.h>
#include <unistd.h>

struct TestSysfsUtils : public ::testing::Test
{
  TestSysfsUtils() : path("/tmp/kodi-test"), output("/tmp/kodi-test") {}
  ~TestSysfsUtils() { std::remove("/tmp/kodi-test"); }

  CSysfs path;
  std::ofstream output;
};

TEST_F(TestSysfsUtils, String)
{
  std::string value(getpagesize(), '0');
  ASSERT_TRUE(path.Set<std::string>(value));
  value.resize(getpagesize() + 1, '0');
  ASSERT_FALSE(path.Set<std::string>(value));

  ASSERT_TRUE(path.Set<std::string>("test"));

  ASSERT_TRUE(path.Get<std::string>() == "test");
  ASSERT_FALSE(path.Get<std::string>() == "kodi");

  path = "/thispathdoesnotexist";
  ASSERT_FALSE(path.Get<std::string>() == "test");
  ASSERT_TRUE(path.Get<std::string>() == "");
}

TEST_F(TestSysfsUtils, Int)
{
  ASSERT_TRUE(path.Set<int>(1234));

  ASSERT_TRUE(path.Get<int>() == 1234);
  ASSERT_FALSE(path.Get<int>() == 4321);

  path = "/thispathdoesnotexist";
  ASSERT_FALSE(path.Get<int>() == 1234);
  ASSERT_TRUE(path.Get<int>() == -1);
}
