/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/xbmc_pvr_types.h>
#include <string>

bool rtsp_open(const std::string& name, const std::string& url_str);
void rtsp_close();
int rtsp_read(void* buf, unsigned buf_size);
void rtsp_fill_signal_status(PVR_SIGNAL_STATUS* signal_status);

