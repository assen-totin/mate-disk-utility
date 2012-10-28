/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-volume.h
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

#ifndef __MDU_VOLUME_H
#define __MDU_VOLUME_H

#include <mdu/mdu-types.h>

G_BEGIN_DECLS

/**
 * MduVolumeFlags:
 * @MDU_VOLUME_FLAGS_NONE: No flags set
 * @MDU_VOLUME_FLAGS_PARTITION: The volume is a partition.
 * @MDU_VOLUME_FLAGS_PARTITION_MBR_LOGICAL: The volume is a Logical Partition in the MBR partitioning scheme.
 * @MDU_VOLUME_FLAGS_PARTITION_MBR_EXTENDED: The volume is an Extended Partition in the MBR partitioning scheme.
 *
 * Various flags for describing a volume.
 */
typedef enum {
        MDU_VOLUME_FLAGS_NONE                   = 0x0000,
        MDU_VOLUME_FLAGS_PARTITION              = (1<<0),
        MDU_VOLUME_FLAGS_PARTITION_MBR_LOGICAL  = (1<<1),
        MDU_VOLUME_FLAGS_PARTITION_MBR_EXTENDED = (1<<2)
} MduVolumeFlags;

#define MDU_TYPE_VOLUME           (mdu_volume_get_type ())
#define MDU_VOLUME(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_VOLUME, MduVolume))
#define MDU_VOLUME_CLASS(k)       (G_TYPE_CHECK_CLASS_CAST ((k), MDU_VOLUME,  MduVolumeClass))
#define MDU_IS_VOLUME(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_VOLUME))
#define MDU_IS_VOLUME_CLASS(k)    (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_VOLUME))
#define MDU_VOLUME_GET_CLASS(k)   (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_VOLUME, MduVolumeClass))

typedef struct _MduVolumeClass       MduVolumeClass;
typedef struct _MduVolumePrivate     MduVolumePrivate;

struct _MduVolume
{
        GObject parent;

        /*< private >*/
        MduVolumePrivate *priv;
};

struct _MduVolumeClass
{
        GObjectClass parent_class;

        gboolean        (*is_allocated)    (MduVolume *volume);
        gboolean        (*is_recognized)   (MduVolume *volume);
        MduVolumeFlags  (*get_flags)       (MduVolume *volume);
};

GType           mdu_volume_get_type        (void);
gboolean        mdu_volume_is_allocated    (MduVolume *volume);
gboolean        mdu_volume_is_recognized   (MduVolume *volume);
MduVolumeFlags  mdu_volume_get_flags       (MduVolume *volume);
MduDrive       *mdu_volume_get_drive       (MduVolume *volume);


G_END_DECLS

#endif /* __MDU_VOLUME_H */
