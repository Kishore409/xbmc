/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "BufferObjectFactory.h"

#include "threads/SingleLock.h"

CCriticalSection bufferObjectSection;
std::vector<::CreateBufferObject> CBufferObjectFactory::m_bufferObjects;

CBufferObject* CBufferObjectFactory::CreateBufferObject(int format)
{
  CSingleLock lock(bufferObjectSection);

  return m_bufferObjects.back()(format);
}

void CBufferObjectFactory::RegisterBufferObject(::CreateBufferObject createFunc)
{
  CSingleLock lock(bufferObjectSection);

  m_bufferObjects.push_back(createFunc);
}

void CBufferObjectFactory::ClearBufferObjects()
{
  CSingleLock lock(bufferObjectSection);

  m_bufferObjects.clear();
}
