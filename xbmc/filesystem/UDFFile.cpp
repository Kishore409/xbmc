/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "UDFFile.h"

#include "URL.h"

#include <cmath>

#include <cdio/udf.h>

using namespace XFILE;

bool CUDFFile::Open(const CURL& url)
{
  if (m_udf && m_path)
    return true;

  m_udf = udf_open(url.GetHostName().c_str());

  if (!m_udf)
    return false;

  udf_dirent_t* root = udf_get_root(m_udf, true, 0);

  if (!root)
  {
    Close();
    return false;
  }

  m_path = udf_fopen(root, url.GetFileName().c_str());

  udf_dirent_free(root);

  if (!m_path)
  {
    Close();
    return false;
  }

  return true;
}

void CUDFFile::Close()
{
  if (m_path)
  {
    udf_dirent_free(m_path);
    m_path = nullptr;
  }

  if (m_udf)
  {
    udf_close(m_udf);
    m_udf = nullptr;
  }
}

int CUDFFile::Stat(const CURL& url, struct __stat64* buffer)
{
  if (!m_udf || !m_path)
    return -1;

  buffer = {};
  buffer->st_size = udf_get_file_length(m_path);

  if (m_path->b_dir)
  {
    buffer->st_mode = S_IFDIR;
  }
  else
  {
    buffer->st_mode = S_IFREG;
  }

  return 0;
}

ssize_t CUDFFile::Read(void* buffer, size_t size)
{
  const int blocks = std::ceil(size / UDF_BLOCKSIZE);
  int block;
  for (block = 0; block < blocks; block++)
  {
    if (udf_read_block(m_path, buffer, 1) != UDF_BLOCKSIZE)
      break;
  }

  m_offset += block;

  return block * UDF_BLOCKSIZE;
}

int64_t CUDFFile::Seek(int64_t filePosition, int whence)
{
  int block = std::floor(filePosition / UDF_BLOCKSIZE);

  switch (whence)
  {
  case SEEK_SET:
    m_offset = block;
    break;
  case SEEK_CUR:
    m_offset += block;
    break;
  case SEEK_END:
    m_offset = std::ceil(GetLength() / UDF_BLOCKSIZE) + block;
    break;
  }

  return m_offset * UDF_BLOCKSIZE;
}

int64_t CUDFFile::GetLength()
{
  return udf_get_file_length(m_path);
}

int64_t CUDFFile::GetPosition()
{
  return m_offset * UDF_BLOCKSIZE;
}

bool CUDFFile::Exists(const CURL& url)
{
  return Open(url);
}

int CUDFFile::GetChunkSize()
{
  return UDF_BLOCKSIZE;
}
