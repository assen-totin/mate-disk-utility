/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2007-2010 David Zeuthen
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

#ifndef __MDU_LINUX_LVM2_VOLUME_H
#define __MDU_LINUX_LVM2_VOLUME_H

#include <mdu/mdu-types.h>
#include <mdu/mdu-callbacks.h>
#include <mdu/mdu-volume.h>

G_BEGIN_DECLS

#define MDU_TYPE_LINUX_LVM2_VOLUME         (mdu_linux_lvm2_volume_get_type ())
#define MDU_LINUX_LVM2_VOLUME(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_LINUX_LVM2_VOLUME, MduLinuxLvm2Volume))
#define MDU_LINUX_LVM2_VOLUME_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_LINUX_LVM2_VOLUME,  MduLinuxLvm2VolumeClass))
#define MDU_IS_LINUX_LVM2_VOLUME(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_LINUX_LVM2_VOLUME))
#define MDU_IS_LINUX_LVM2_VOLUME_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_LINUX_LVM2_VOLUME))
#define MDU_LINUX_LVM2_VOLUME_GET_CLASS(k) (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_LINUX_LVM2_VOLUME, MduLinuxLvm2VolumeClass))

typedef struct _MduLinuxLvm2VolumeClass    MduLinuxLvm2VolumeClass;
typedef struct _MduLinuxLvm2VolumePrivate  MduLinuxLvm2VolumePrivate;

struct _MduLinuxLvm2Volume
{
        MduVolume parent;

        /*< private >*/
        MduLinuxLvm2VolumePrivate *priv;
};

struct _MduLinuxLvm2VolumeClass
{
        MduVolumeClass parent_class;
};

GType        mdu_linux_lvm2_volume_get_type       (void);
const gchar *mdu_linux_lvm2_volume_get_name       (MduLinuxLvm2Volume *volume);
const gchar *mdu_linux_lvm2_volume_get_uuid       (MduLinuxLvm2Volume *volume);
const gchar *mdu_linux_lvm2_volume_get_group_uuid (MduLinuxLvm2Volume *volume);

G_END_DECLS

#endif /* __MDU_LINUX_LVM2_VOLUME_H */
