/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DumbBufferObject.h"

#include "ServiceBroker.h"
#include "utils/BufferObjectFactory.h"
#include "utils/log.h"
#include "windowing/gbm/WinSystemGbm.h"
#include "windowing/gbm/WinSystemGbmEGLContext.h"

#include <drm/drm_fourcc.h>
#include <sys/mman.h>

using namespace KODI::WINDOWING::GBM;

CBufferObject* CDumbBufferObject::Create(int format)
{
  return new CDumbBufferObject(format);
}

void CDumbBufferObject::Register()
{
  CBufferObjectFactory::RegisterBufferObject(CDumbBufferObject::Create);
}

CDumbBufferObject::CDumbBufferObject(int format)
{
  m_device = static_cast<CWinSystemGbmEGLContext*>(CServiceBroker::GetWinSystem())->GetDrm()->GetFileDescriptor();

  switch (format)
  {
  case DRM_FORMAT_ARGB1555:
  case DRM_FORMAT_RGB565:
    m_bpp = 16;
    break;
  case DRM_FORMAT_ARGB8888:
  default:
    m_bpp = 32;
    break;
  }
}

CDumbBufferObject::~CDumbBufferObject()
{
  ReleaseMemory();
  DestroyBufferObject();
}

bool CDumbBufferObject::CreateBufferObject(int width, int height)
{
  if (m_fd >= 0)
  {
    return true;
  }

  m_width = width;
  m_height = height;

  struct drm_mode_create_dumb create_dumb =
  {
    .height = static_cast<uint32_t>(m_height),
    .width = static_cast<uint32_t>(m_width),
    .bpp = m_bpp
  };

  int ret = drmIoctl(m_device, DRM_IOCTL_MODE_CREATE_DUMB, &create_dumb);
  if (ret < 0)
  {
    CLog::Log(LOGERROR, "CDumbBufferObject::{} - ioctl DRM_IOCTL_MODE_CREATE_DUMB failed, ret={} errno={}", __FUNCTION__, ret, strerror(errno));
    return false;
  }

  m_size = create_dumb.size;
  m_stride = create_dumb.pitch;

  ret = drmPrimeHandleToFD(m_device, create_dumb.handle, 0, &m_fd);
  if (ret < 0)
  {
    CLog::Log(LOGERROR, "CDumbBufferObject::{} - failed to get fd from prime handle, ret={} errno={}", __FUNCTION__, ret, strerror(errno));
    return false;
  }

  return true;
}

void CDumbBufferObject::DestroyBufferObject()
{
  if (m_fd < 0)
  {
    return;
  }

  int ret = close(m_fd);
  if (ret < 0)
  {
    CLog::Log(LOGERROR, "CDumbBufferObject::{} - close failed, errno={}", __FUNCTION__, strerror(errno));
  }

  m_fd = -1;
  m_stride = 0;
  m_size = 0;
}

uint8_t* CDumbBufferObject::GetMemory()
{
  if (m_fd < 0)
  {
    return nullptr;
  }

  if (m_map)
  {
    CLog::Log(LOGDEBUG, "CDumbBufferObject::{} - already mapped fd={} map={}", __FUNCTION__, m_fd, fmt::ptr(m_map));
    return m_map;
  }

  uint32_t handle;
  int ret = drmPrimeFDToHandle(m_device, m_fd, &handle);
  if (ret < 0)
  {
    CLog::Log(LOGERROR, "CDumbBufferObject::{} - failed to get handle from prime fd, ret={} errno={}", __FUNCTION__, ret, strerror(errno));
    return nullptr;
  }

  struct drm_mode_map_dumb map_dumb =
  {
    .handle = handle
  };

  ret = drmIoctl(m_device, DRM_IOCTL_MODE_MAP_DUMB, &map_dumb);
  if (ret < 0)
  {
    CLog::Log(LOGERROR, "CDumbBufferObject::{} - ioctl DRM_IOCTL_MODE_MAP_DUMB failed, ret={} errno={}", __FUNCTION__, ret, strerror(errno));
    return nullptr;
  }

  m_offset = map_dumb.offset;

  m_map = static_cast<uint8_t*>(mmap(nullptr, m_size, PROT_WRITE, MAP_SHARED, m_device, m_offset));
  if (m_map == MAP_FAILED)
  {
    CLog::Log(LOGERROR, "CDumbBufferObject::{} - mmap failed, errno={}", __FUNCTION__, strerror(errno));
    return nullptr;
  }

  return m_map;
}

void CDumbBufferObject::ReleaseMemory()
{
  if (!m_map)
  {
    return;
  }

  int ret = munmap(m_map, m_size);
  if (ret < 0)
  {
    CLog::Log(LOGERROR, "CDumbBufferObject::{} - munmap failed, errno={}", __FUNCTION__, strerror(errno));
  }

  m_map = nullptr;
  m_offset = 0;
}
