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

#ifndef __MDU_LINUX_LVM2_VOLUME_HOLE_H
#define __MDU_LINUX_LVM2_VOLUME_HOLE_H

#include <mdu/mdu-types.h>
#include <mdu/mdu-callbacks.h>
#include <mdu/mdu-volume-hole.h>

G_BEGIN_DECLS

#define MDU_TYPE_LINUX_LVM2_VOLUME_HOLE         (mdu_linux_lvm2_volume_hole_get_type ())
#define MDU_LINUX_LVM2_VOLUME_HOLE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_LINUX_LVM2_VOLUME_HOLE, MduLinuxLvm2VolumeHole))
#define MDU_LINUX_LVM2_VOLUME_HOLE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_LINUX_LVM2_VOLUME_HOLE,  MduLinuxLvm2VolumeHoleClass))
#define MDU_IS_LINUX_LVM2_VOLUME_HOLE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_LINUX_LVM2_VOLUME_HOLE))
#define MDU_IS_LINUX_LVM2_VOLUME_HOLE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_LINUX_LVM2_VOLUME_HOLE))
#define MDU_LINUX_LVM2_VOLUME_HOLE_GET_CLASS(k) (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_LINUX_LVM2_VOLUME_HOLE, MduLinuxLvm2VolumeHoleClass))

typedef struct _MduLinuxLvm2VolumeHoleClass    MduLinuxLvm2VolumeHoleClass;
typedef struct _MduLinuxLvm2VolumeHolePrivate  MduLinuxLvm2VolumeHolePrivate;

struct _MduLinuxLvm2VolumeHole
{
        MduVolumeHole parent;

        /*< private >*/
        MduLinuxLvm2VolumeHolePrivate *priv;
};

struct _MduLinuxLvm2VolumeHoleClass
{
        MduVolumeHoleClass parent_class;
};

GType      mdu_linux_lvm2_volume_hole_get_type (void);

G_END_DECLS

#endif /* __MDU_LINUX_LVM2_VOLUME_HOLE_H */
