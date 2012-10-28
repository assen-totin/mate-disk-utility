/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu.h
 *
 * Copyright (C) 2007 David Zeuthen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __MDU_H
#define __MDU_H

#ifndef MDU_API_IS_SUBJECT_TO_CHANGE
#error  libmdu is unstable API. You must define MDU_API_IS_SUBJECT_TO_CHANGE before including mdu/mdu.h
#endif

#define __MDU_INSIDE_MDU_H 1

#include <mdu/mdu-types.h>
#include <mdu/mdu-linux-md-drive.h>
#include <mdu/mdu-linux-lvm2-volume-group.h>
#include <mdu/mdu-linux-lvm2-volume.h>
#include <mdu/mdu-linux-lvm2-volume-hole.h>
#include <mdu/mdu-device.h>
#include <mdu/mdu-adapter.h>
#include <mdu/mdu-expander.h>
#include <mdu/mdu-port.h>
#include <mdu/mdu-drive.h>
#include <mdu/mdu-error.h>
#include <mdu/mdu-known-filesystem.h>
#include <mdu/mdu-pool.h>
#include <mdu/mdu-presentable.h>
#include <mdu/mdu-process.h>
#include <mdu/mdu-util.h>
#include <mdu/mdu-volume.h>
#include <mdu/mdu-volume-hole.h>
#include <mdu/mdu-hub.h>
#include <mdu/mdu-machine.h>
#include <mdu/mdu-callbacks.h>

#undef __MDU_INSIDE_MDU_H

G_END_DECLS

#endif /* __MDU_H */
