/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-types.h
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

#if !defined (__MDU_INSIDE_MDU_H) && !defined (MDU_COMPILATION)
#error "Only <mdu/mdu.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __MDU_TYPES_H
#define __MDU_TYPES_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/* TODO: should have enum type files etc */

typedef enum {
        MDU_HUB_USAGE_ADAPTER,
        MDU_HUB_USAGE_EXPANDER,
        MDU_HUB_USAGE_MULTI_DISK_DEVICES,
        MDU_HUB_USAGE_MULTI_PATH_DEVICES,
} MduHubUsage;


/* forward type definitions */

typedef struct _MduPool                   MduPool;
typedef struct _MduDevice                 MduDevice;
typedef struct _MduAdapter                MduAdapter;
typedef struct _MduExpander               MduExpander;
typedef struct _MduPort                   MduPort;

typedef struct _MduPresentable            MduPresentable; /* Dummy typedef */

typedef struct _MduDrive                  MduDrive;
typedef struct _MduLinuxMdDrive           MduLinuxMdDrive;
typedef struct _MduLinuxLvm2VolumeGroup   MduLinuxLvm2VolumeGroup;
typedef struct _MduVolume                 MduVolume;
typedef struct _MduVolumeHole             MduVolumeHole;
typedef struct _MduLinuxLvm2Volume        MduLinuxLvm2Volume;
typedef struct _MduLinuxLvm2VolumeHole    MduLinuxLvm2VolumeHole;
typedef struct _MduHub                    MduHub;
typedef struct _MduMachine                MduMachine;

typedef struct _MduKnownFilesystem        MduKnownFilesystem;
typedef struct _MduProcess                MduProcess;

G_END_DECLS

#endif /* __MDU_TYPES_H */
