/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "BufferObject.h"

#include <vector>

typedef CBufferObject* (*CreateBufferObject)(int format);

class CBufferObjectFactory
{
public:
  static CBufferObject* CreateBufferObject(int format);

  static void RegisterBufferObject(::CreateBufferObject createFunc);
  static void ClearBufferObjects();

protected:

  static std::vector<::CreateBufferObject> m_bufferObjects;
};
