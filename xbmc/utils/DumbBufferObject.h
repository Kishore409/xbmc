/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "utils/BufferObject.h"

#include <stdint.h>

class CDumbBufferObject : public CBufferObject
{
public:
  CDumbBufferObject(int format);
  virtual ~CDumbBufferObject() override;

  // Registration
  static CBufferObject* Create(int format);
  static void Register();

  bool CreateBufferObject(int width, int height) override;
  void DestroyBufferObject() override;
  uint8_t* GetMemory() override;
  void ReleaseMemory() override;

private:
  int m_device = -1;
  uint32_t m_bpp = 0;
  uint64_t m_size = 0;
  uint64_t m_offset = 0;
  uint8_t *m_map = nullptr;
};
